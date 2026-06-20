/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:11 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/20 07:54:59 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP
#include "IConfigParser.hpp"
#include "LocationConfigBuilder.hpp"
#include "ServerConfigBuilder.hpp"
#include "Tokenizer.hpp"
#include <cstdlib>
#include <istream>
#include <sstream>

class ConfigParser : public IConfigParser {
private:
	std::string					 default_path;
	std::string					 env;
	void						 _defaultPathSet();
	IServerConfig*				 parseServer(std::vector<Tokenizer::Token>& toks);
	void						 parseServerItem(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseListenDirective(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseServerNameDirective(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseRootDirective(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseIndexDirective(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseErrorPageDirective(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseClientMaxBodySizeDirective(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseTimeOut(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseAutoIndexDirective(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);

	void						 parseLocationBlock(ServerConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseLocationItem(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseRootDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseIndexDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseErrorPageDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseClientMaxBodySizeDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseAllowMethodsDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseAutoIndexDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseReturnDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseUploadStoreDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseCgiPassDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseFastCgiPassDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);
	void						 parseFastCgiParamDirective(LocationConfigBuilder& builder, std::vector<Tokenizer::Token>& toks);

	// Value helpers
	IServerConfig::ErrorPage	 parseErrorPageValue(std::vector<Tokenizer::Token>& toks);
	std::string					 consumeWordToken(const std::string& ctx, std::vector<Tokenizer::Token>& toks);
	bool						 isAllDigits(const std::string& val) const;
	std::size_t					 parseSize(const std::string& val) const;
	IServerConfig::ListenAddress parseListenValue(const std::string& val) const;

	void						 expect_token(Tokenizer::TokenType tok, std::vector<Tokenizer::Token>& toks);
	void						 consume_token(std::string const val, std::vector<Tokenizer::Token>& toks);
	void						 consume_token(Tokenizer::TokenType tok_type, std::vector<Tokenizer::Token>& toks);
	bool						 first_eq(Tokenizer::TokenType tok_type, std::vector<Tokenizer::Token>& toks);
	bool						 first_eq(std::string val, std::vector<Tokenizer::Token>& toks);

public:
	ConfigParser();
	ConfigParser(std::string env, std::string default_path);
	~ConfigParser();
	ConfigParser(ConfigParser const& other);
	ConfigParser& operator=(ConfigParser const& other);
	IConfig*	  parse(const std::string& path);
	IConfig*	  parse(std::istream& in);
	IConfig*	  parseDefault(void);
};
#endif
