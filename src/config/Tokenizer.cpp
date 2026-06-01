/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:47:30 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:47:31 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Tokenizer.hpp"

Tokenizer::Tokenizer() : tokens() {
}
Tokenizer::~Tokenizer() {
}
Tokenizer::Tokenizer(Tokenizer const& other) {
	if (this == &other)
		return;
	*this = other;
}

Tokenizer& Tokenizer::operator=(Tokenizer const& other) {
	if (this == &other)
		return *this;
	this->tokens = other.tokens;
	return *this;
}

void Tokenizer::flushWord(std::string& buf, std::size_t line) {
	if (buf == "")
		return;
	tokens.push_back(Token(WORD, buf, line));
	buf = "";
}

Tokenizer::TokenType Tokenizer::symbolType(char c) {
	switch (c) {
	case '{':
		return LBRACE;
	case '}':
		return RBRACE;
	case ';':
		return SEMICOLON;
	default:
		return WORD;
	}
}
std::vector<Tokenizer::Token> const& Tokenizer::tokenize(std::istream& in) {
	tokens.clear();

	std::string buf;
	std::size_t line = 1;
	char		c;

	while (in.get(c)) {
		if (c == '\n') {
			flushWord(buf, line);
			++line;
		} else if (std::isspace(c)) {
			flushWord(buf, line);
		} else if (c == '#') {
			flushWord(buf, line);
			while (in.get(c) && c != '\n')
				;
			++line;
		} else if (c == '{' || c == '}' || c == ';') {
			flushWord(buf, line);
			tokens.push_back(Token(symbolType(c), std::string(1, c), line));
		} else
			buf += c;
	}
	flushWord(buf, line);
	return tokens;
}

std::vector<Tokenizer::Token> const& Tokenizer::getTokens() {
	return tokens;
}
