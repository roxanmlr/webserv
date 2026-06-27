/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 16:05:51 by lmilando          #+#    #+#             */
/*   Updated: 2026/06/26 09:15:07 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef UTILS_HPP
#define UTILS_HPP
#include <fcntl.h>
#include <string>

std::string ft_itoa(int n);
int			set_nonblocking(int fd);
std::string base64_encode(const std::string& input);
bool		base64_decode(const std::string& input, std::string& output);
#endif
