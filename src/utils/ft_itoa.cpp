/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 16:05:29 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/09 16:05:32 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

std::string ft_itoa(int n){
	if (!n)
		return "0";
	std::string s;
	bool negative = n < 0;
	long l = (n >= 0)? n : -n;
	while (l){
		s.insert(s.begin(),(char)(l % 10 + '0'));
		l /= 10;
	}
	if (negative)
		s.insert(s.begin(), '-');
	return s;
}
