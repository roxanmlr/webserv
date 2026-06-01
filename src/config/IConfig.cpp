/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IConfig.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:16 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:48:19 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IConfig.hpp"

std::ostream& operator<<(std::ostream& out, IConfig const& config) {
	return config.write(out);
}
std::istream& operator>>(std::istream& in, IConfig& config) {
	return config.read(in);
}
