/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:47:26 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/20 08:01:45 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
#include "Config.hpp"
#include <cctype>
#include <fstream>
#include <iostream>

ConfigParser::ConfigParser() : default_path(""), env("") {
}

void ConfigParser::_defaultPathSet() {
	if (default_path == "") {
		char* v = getenv(env.c_str());
		if (v) {
			default_path = v;
		}
	}
	if (default_path == "") {
		std::cerr << "Default configuration is not set" << std::endl;
		return;
	}
	std::ifstream file(default_path.c_str());
	if (!file.good()) {
		std::cerr << "Default configuration cannot be read or doesn't exist." << std::endl;
	}
}

ConfigParser::ConfigParser(std::string env, std::string default_path) : default_path(default_path), env(env) {
	_defaultPathSet();
}

ConfigParser::~ConfigParser() {
}

ConfigParser::ConfigParser(ConfigParser const& other) {
	if (this == &other)
		return;
	*this = other;
}

ConfigParser& ConfigParser::operator=(ConfigParser const& other) {
	if (this == &other)
		return *this;
	env			 = other.env;
	default_path = other.default_path;
	_defaultPathSet();
	return *this;
}

IConfig* ConfigParser::parse(std::istream& in) {
	Tokenizer					  t		 = Tokenizer();
	std::vector<Tokenizer::Token> toks	 = t.tokenize(in);
	Config*						  config = new Config();
	try {
		while (!toks.empty() && toks.front().type == Tokenizer::WORD && toks.front().value == "server") {
			IServerConfig* srv = parseServer(toks);
			try {
				config->addServer(srv);
			} catch (...) {
				delete srv;
				throw;
			}
		}
		if (!toks.empty())
			throw ConfigError("Unexpected token `" + toks.front().value + "`");
	} catch (ConfigError const& e) {
		if (!toks.empty())
			std::cerr << "Line " << toks.front().line << " : ";
		std::cerr << e.what() << std::endl;
		delete config;
		return NULL;
	}
	return config;
}

IConfig* ConfigParser::parse(const std::string& path) {
	std::ifstream file(path.c_str());
	if (!file.good())
		throw ConfigError("Configuration `" + path + "` can't be read or doesn't exist.");
	return parse(file);
}

IConfig* ConfigParser::parseDefault() {
	return parse(default_path);
}

// ─── Server block ────────────────────────────────────────────────────────────

IServerConfig* ConfigParser::parseServer(std::vector<Tokenizer::Token>& toks) {
	ServerConfigBuilder builder;
	consume_token("server", toks);
	expect_token(Tokenizer::LBRACE, toks);
	while (!toks.empty() && toks.front().type != Tokenizer::RBRACE) {
		if (first_eq("location", toks))
			parseLocationBlock(builder, toks);
		else
			parseServerItem(builder, toks);
	}
	expect_token(Tokenizer::RBRACE, toks);
	return builder.build();
}

void ConfigParser::parseServerItem(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	if (first_eq("listen", toks))
		parseListenDirective(builder, toks);
	else if (first_eq("server_name", toks))
		parseServerNameDirective(builder, toks);
	else if (first_eq("root", toks))
		parseRootDirective(builder, toks);
	else if (first_eq("index", toks))
		parseIndexDirective(builder, toks);
	else if (first_eq("error_page", toks))
		parseErrorPageDirective(builder, toks);
	else if (first_eq("client_max_body_size", toks))
		parseClientMaxBodySizeDirective(builder, toks);
	else if (first_eq("timeout", toks))
		parseTimeOut(builder, toks);
	else if (first_eq("autoindex", toks))
		parseAutoIndexDirective(builder, toks);
	else if (toks.empty())
		throw ConfigError("Unexpected end of file");
	else
		throw ConfigError("Unexpected directive `" + toks.front().value + "`");
	consume_token(Tokenizer::SEMICOLON, toks);
}

void ConfigParser::parseListenDirective(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("listen", toks);
	builder.addListenAddress(parseListenValue(consumeWordToken("listen", toks)));
}

void ConfigParser::parseServerNameDirective(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("server_name", toks);
	while (!toks.empty() && toks.front().type == Tokenizer::WORD) {
		builder.addServerName(toks.front().value);
		toks.erase(toks.begin());
	}
}

void ConfigParser::parseRootDirective(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("root", toks);
	builder.setRootDir(consumeWordToken("root", toks));
}

void ConfigParser::parseAutoIndexDirective(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("autoindex", toks);
	if (first_eq("on", toks)) {
		consume_token("on", toks);
		builder.autoIndexOn();
		return;
	} else if (first_eq("off", toks)) {
		consume_token("off", toks);
		builder.autoIndexOff();
		return;
	}
	throw ConfigError("incomplete autoindex directive");
}

void ConfigParser::parseIndexDirective(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("index", toks);
	while (!toks.empty() && toks.front().type == Tokenizer::WORD) {
		builder.addIndex(toks.front().value);
		toks.erase(toks.begin());
	}
}

void ConfigParser::parseErrorPageDirective(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("error_page", toks);
	builder.addErrorPage(parseErrorPageValue(toks));
}

void ConfigParser::parseClientMaxBodySizeDirective(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("client_max_body_size", toks);
	builder.setClientMaxBodySize(parseSize(consumeWordToken("client_max_body_size", toks)));
}

// ─── Location block ──────────────────────────────────────────────────────────

void ConfigParser::parseLocationBlock(ServerConfigBuilder& srv_builder, std::vector<Tokenizer::Token>& toks) {
	LocationConfigBuilder builder;
	consume_token("location", toks);

	ILocationConfig::MatchType match_type = ILocationConfig::MATCH_PREFIX;
	if (!toks.empty() && toks.front().type == Tokenizer::WORD) {
		const std::string& mod = toks.front().value;
		if (mod == "=") {
			match_type = ILocationConfig::MATCH_EXACT;
			toks.erase(toks.begin());
		} else if (mod == "^~") {
			match_type = ILocationConfig::MATCH_PREFIX_PRIORITY;
			toks.erase(toks.begin());
		} else if (mod == "~" || mod == "~*") {
			throw ConfigError("location: regex modifier `" + mod + "` is not supported");
		}
	}
	builder.setMatchType(match_type);
	builder.setPath(consumeWordToken("location path", toks));
	expect_token(Tokenizer::LBRACE, toks);
	while (!toks.empty() && toks.front().type != Tokenizer::RBRACE)
		parseLocationItem(builder, toks);
	expect_token(Tokenizer::RBRACE, toks);
	ILocationConfig* loc = builder.build();
	try {
		srv_builder.addLocation(loc);
	} catch (...) {
		delete loc;
		throw;
	}
}

void ConfigParser::parseLocationItem(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	if (first_eq("allow_methods", toks))
		parseAllowMethodsDirective(builder, toks);
	else if (first_eq("autoindex", toks))
		parseAutoIndexDirective(builder, toks);
	else if (first_eq("return", toks))
		parseReturnDirective(builder, toks);
	else if (first_eq("upload_store", toks))
		parseUploadStoreDirective(builder, toks);
	else if (first_eq("cgi_pass", toks))
		parseCgiPassDirective(builder, toks);
	else if (first_eq("fastcgi_pass", toks))
		parseFastCgiPassDirective(builder, toks);
	else if (first_eq("fastcgi_param", toks))
		parseFastCgiParamDirective(builder, toks);
	else if (first_eq("root", toks))
		parseRootDirective(builder, toks);
	else if (first_eq("index", toks))
		parseIndexDirective(builder, toks);
	else if (first_eq("error_page", toks))
		parseErrorPageDirective(builder, toks);
	else if (first_eq("client_max_body_size", toks))
		parseClientMaxBodySizeDirective(builder, toks);
	else if (toks.empty())
		throw ConfigError("Unexpected end of file");
	else
		throw ConfigError("Unexpected directive `" + toks.front().value + "`");
	consume_token(Tokenizer::SEMICOLON, toks);
}

void ConfigParser::parseRootDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("root", toks);
	builder.setRoot(consumeWordToken("root", toks));
}

void ConfigParser::parseIndexDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("index", toks);
	while (!toks.empty() && toks.front().type == Tokenizer::WORD) {
		builder.addIndexFile(toks.front().value);
		toks.erase(toks.begin());
	}
}

void ConfigParser::parseErrorPageDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("error_page", toks);
	builder.addErrorPage(parseErrorPageValue(toks));
}

void ConfigParser::parseClientMaxBodySizeDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("client_max_body_size", toks);
	builder.setClientMaxBodySize(parseSize(consumeWordToken("client_max_body_size", toks)));
}

void ConfigParser::parseAllowMethodsDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("allow_methods", toks);
	bool found = false;
	while (!toks.empty() && toks.front().type == Tokenizer::WORD) {
		const std::string& m = toks.front().value;
		if (m == "GET")
			builder.addAllowedMethod(ILocationConfig::GET);
		else if (m == "POST")
			builder.addAllowedMethod(ILocationConfig::POST);
		else if (m == "DELETE")
			builder.addAllowedMethod(ILocationConfig::DELETE);
		else if (m == "PUT")
			builder.addAllowedMethod(ILocationConfig::PUT);
		else
			throw ConfigError("allow_methods: unknown method `" + m + "`");
		toks.erase(toks.begin());
		found = true;
	}
	if (!found)
		throw ConfigError("allow_methods: at least one method required");
}

void ConfigParser::parseAutoIndexDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("autoindex", toks);
	std::string val = consumeWordToken("autoindex", toks);
	if (val == "on")
		builder.setAutoIndex(true);
	else if (val == "off")
		builder.setAutoIndex(false);
	else
		throw ConfigError("autoindex: expected `on` or `off`, got `" + val + "`");
}

void ConfigParser::parseReturnDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("return", toks);
	std::string code_str = consumeWordToken("return code", toks);
	if (!isAllDigits(code_str))
		throw ConfigError("return: expected integer status code, got `" + code_str + "`");
	ILocationConfig::ReturnConfig ret;
	ret.code = static_cast<int>(std::strtol(code_str.c_str(), NULL, 10));
	if (!toks.empty() && toks.front().type == Tokenizer::WORD) {
		ret.url.set(toks.front().value);
		toks.erase(toks.begin());
	}
	builder.setReturnConfig(ret);
}

void ConfigParser::parseUploadStoreDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("upload_store", toks);
	builder.setUploadStore(consumeWordToken("upload_store", toks));
}

void ConfigParser::parseCgiPassDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("cgi_pass", toks);
	ILocationConfig::CgiPass pass;
	pass.extension	 = consumeWordToken("cgi_pass extension", toks);
	pass.interpreter = consumeWordToken("cgi_pass interpreter", toks);
	builder.addCgiPass(pass);
}

void ConfigParser::parseFastCgiPassDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("fastcgi_pass", toks);
	builder.setFastCgiPass(consumeWordToken("fastcgi_pass", toks));
}

void ConfigParser::parseFastCgiParamDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("fastcgi_param", toks);
	std::string key	  = consumeWordToken("fastcgi_param key", toks);
	std::string value = consumeWordToken("fastcgi_param value", toks);
	builder.addFastCgiParam(key, value);
}

void ConfigParser::parseTimeOut(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks) {
	consume_token("timeout", toks);
	size_t		value;
	std::string tok(consumeWordToken("timeout", toks));
	if (!isAllDigits(tok))
		throw ConfigError("invalid timeout value `" + tok + "`");
	std::istringstream iss(tok);
	iss >> value;
	builder.setTimeOut(value);
}
// ─── Value helpers ───────────────────────────────────────────────────────────

IServerConfig::ErrorPage ConfigParser::parseErrorPageValue(std::vector<Tokenizer::Token>& toks) {
	IServerConfig::ErrorPage page;
	while (!toks.empty() && toks.front().type == Tokenizer::WORD && isAllDigits(toks.front().value)) {
		page.codes.push_back(static_cast<unsigned int>(std::strtoul(toks.front().value.c_str(), NULL, 10)));
		toks.erase(toks.begin());
	}
	if (page.codes.empty())
		throw ConfigError("error_page: at least one status code required");
	page.path = consumeWordToken("error_page path", toks);
	return page;
}

std::string ConfigParser::consumeWordToken(const std::string& ctx, std::vector<Tokenizer::Token>& toks) {
	if (toks.empty() || toks.front().type != Tokenizer::WORD)
		throw ConfigError(ctx + ": expected a value");
	std::string val = toks.front().value;
	toks.erase(toks.begin());
	return val;
}

bool ConfigParser::isAllDigits(const std::string& val) const {
	if (val.empty())
		return false;
	for (std::size_t i = 0; i < val.size(); ++i)
		if (!std::isdigit(static_cast<unsigned char>(val[i])))
			return false;
	return true;
}

std::size_t ConfigParser::parseSize(const std::string& val) const {
	if (val.empty())
		throw ConfigError("empty size value");
	std::size_t mult = 1;
	std::string num	 = val;
	char		last = val[val.size() - 1];
	if (last == 'K' || last == 'k') {
		mult = 1024UL;
		num	 = val.substr(0, val.size() - 1);
	} else if (last == 'M' || last == 'm') {
		mult = 1024UL * 1024;
		num	 = val.substr(0, val.size() - 1);
	} else if (last == 'G' || last == 'g') {
		mult = 1024UL * 1024 * 1024;
		num	 = val.substr(0, val.size() - 1);
	}
	if (!isAllDigits(num))
		throw ConfigError("invalid size value: `" + val + "`");
	return static_cast<std::size_t>(std::strtoul(num.c_str(), NULL, 10)) * mult;
}

IServerConfig::ListenAddress ConfigParser::parseListenValue(const std::string& val) const {
	IServerConfig::ListenAddress addr;
	std::size_t					 colon = val.find(':');
	if (colon != std::string::npos) {
		addr.host = val.substr(0, colon);
		addr.port = static_cast<unsigned int>(std::strtoul(val.substr(colon + 1).c_str(), NULL, 10));
	} else {
		addr.host = "0.0.0.0";
		addr.port = static_cast<unsigned int>(std::strtoul(val.c_str(), NULL, 10));
	}
	if (addr.port == 0)
		throw ConfigError("listen: invalid port in `" + val + "`");
	return addr;
}

// ─── Token stream utilities ──────────────────────────────────────────────────

std::string tok_to_string(Tokenizer::TokenType tok_type) {
	switch (tok_type) {
	case Tokenizer::LBRACE:
		return "{";
	case Tokenizer::RBRACE:
		return "}";
	case Tokenizer::END:
		return "end of file";
	case Tokenizer::SEMICOLON:
		return ";";
	default:
		return "a directive or value";
	}
}

void ConfigParser::expect_token(Tokenizer::TokenType tok_type, std::vector<Tokenizer::Token>& toks) {
	if (toks.empty())
		throw ConfigError("Unexpected end of file, expected `" + tok_to_string(tok_type) + "`");
	if (toks.front().type == tok_type) {
		toks.erase(toks.begin());
		return;
	}
	throw ConfigError("Unexpected `" + toks.front().value + "`, expected `" + tok_to_string(tok_type) + "`");
}

void ConfigParser::consume_token(std::string const val, std::vector<Tokenizer::Token>& toks) {
	if (toks.empty())
		throw ConfigError("Unexpected end of file, expected `" + val + "`");
	if (toks.front().value == val && toks.front().type == Tokenizer::WORD) {
		toks.erase(toks.begin());
		return;
	}
	throw ConfigError("Unexpected `" + toks.front().value + "`, expected `" + val + "`");
}

void ConfigParser::consume_token(Tokenizer::TokenType tok_type, std::vector<Tokenizer::Token>& toks) {
	if (toks.empty())
		throw ConfigError("Unexpected end of file, expected `" + tok_to_string(tok_type) + "`");
	if (toks.front().type == tok_type) {
		toks.erase(toks.begin());
		return;
	}
	throw ConfigError("Unexpected `" + toks.front().value + "`, expected `" + tok_to_string(tok_type) + "`");
}

bool ConfigParser::first_eq(Tokenizer::TokenType tok_type, std::vector<Tokenizer::Token>& toks) {
	if (toks.empty())
		return false;
	return toks.front().type == tok_type;
}

bool ConfigParser::first_eq(std::string val, std::vector<Tokenizer::Token>& toks) {
	if (toks.empty())
		return false;
	return toks.front().value == val;
}
