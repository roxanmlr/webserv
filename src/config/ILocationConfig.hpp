/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ILocationConfig.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:47:46 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/25 21:05:11 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef ILOCATION_CONFIG_HPP
#define ILOCATION_CONFIG_HPP
#include "../utils/Optional.tpp"
#include "ConfigError.hpp"
#include "IServerConfig.hpp"
#include <map>
#include <string>
#include <vector>

class ILocationConfig {
public:
	struct Redirect {
		int			code;
		std::string target;
	};
	struct ReturnConfig {
		int			code;
		std::string url;
	};
	struct CgiPass {
		std::string extension;
		std::string interpreter;
	};
	enum Method { GET, POST, DELETE, PUT };
	enum MatchType { MATCH_PREFIX, MATCH_EXACT, MATCH_PREFIX_PRIORITY };
	virtual ~ILocationConfig() {
	}
	virtual ILocationConfig*							 clone() const = 0;

	virtual const std::string&							 getPath() const					   = 0;
	virtual MatchType									 getMatchType() const				   = 0;
	virtual bool										 matches(const std::string& uri) const = 0;

	virtual Optional<std::string> const&				 getRoot() const			  = 0;
	virtual Optional<std::size_t> const&				 getClientMaxBodySize() const = 0;

	virtual std::vector<std::string> const&				 getIndexFiles() const	   = 0;
	virtual std::vector<IServerConfig::ErrorPage> const& getErrorPages() const	   = 0;
	virtual std::vector<Method> const&					 getAllowedMethods() const = 0;
	virtual bool										 isAutoIndex() const	   = 0;

	virtual Optional<ReturnConfig> const&				 getReturnConfig() const	 = 0;
	virtual Optional<std::string> const&				 getUploadStore() const		 = 0;
	virtual Optional<std::string> const&				 getFastCgiPass() const		 = 0;
	virtual std::map<std::string, std::string> const&	 getFastCgiParams() const	 = 0;
	virtual std::vector<CgiPass> const&					 getCgiPasses() const		 = 0;
	virtual bool										 needsAuthentication() const = 0;
	virtual Optional<std::string>						 getAuthFilename() const	 = 0;

	virtual bool										 isMethodAllowed(const std::string& method) const = 0;

	virtual bool										 operator==(ILocationConfig const& other) const = 0;
};

std::ostream& operator<<(std::ostream& out, ILocationConfig const& loc);
#endif
