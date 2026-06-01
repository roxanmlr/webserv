/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:22 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:48:23 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP
#include <cctype>
#include <istream>
#include <string>
#include <vector>

class Tokenizer {
public:
	enum TokenType { WORD, LBRACE, RBRACE, SEMICOLON, END };
	struct Token {
		enum TokenType type;
		std::string	   value;
		std::size_t	   line;
		Token(TokenType t, const std::string& v, size_t l) : type(t), value(v), line(l) {
		}
	};

private:
	std::vector<Token> tokens;
	void			   flushWord(std::string& buf, std::size_t line);
	TokenType		   symbolType(char c);

public:
	Tokenizer();
	~Tokenizer();
	Tokenizer(Tokenizer const& other);
	Tokenizer&				  operator=(Tokenizer const&);
	std::vector<Token> const& tokenize(std::istream& in);
	std::vector<Token> const& getTokens();
};
#endif
