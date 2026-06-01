/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ITestCase.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:46:41 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:46:42 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef ITEST_CASE_HPP
#define ITEST_CASE_HPP
#include <string>

class ITestCase {
public:
	virtual ~ITestCase() {
	}
	virtual const std::string& name() const = 0;
	virtual void			   run()		= 0;
};
#endif
