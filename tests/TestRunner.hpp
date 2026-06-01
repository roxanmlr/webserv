/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TestRunner.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:46:47 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:46:49 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef TEST_RUNNER_HPP
#define TEST_RUNNER_HPP
#include "ITestCase.hpp"
#include <vector>

class TestRunner {
private:
	std::vector<ITestCase*> tests;

public:
	void add(ITestCase* test);
	int	 runAll();
};
#endif
