/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Optional.tpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmilando <lmilando@42.fr>                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/27 23:48:43 by lmilando          #+#    #+#             */
/*   Updated: 2026/05/27 23:48:43 by lmilando         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#ifndef OPTIONAL_TPP
#define OPTIONAL_TPP
#include <stdexcept>

template <typename T> class Optional {
private:
	union u_storage {
		char   buf[sizeof(T)];
		long   _long;
		double _double;
		void*  ptr;
	};
	bool	  hasValue;
	u_storage storage;
	void	  construct(T const& value) {
		 new (storage.buf) T(value);
		 hasValue = true;
	}
	T* ptr() {
		return reinterpret_cast<T*>(storage.buf);
	}
	const T* ptr_const() const {
		return reinterpret_cast<const T*>(storage.buf);
	}

public:
	Optional() : hasValue(false) {
	}
	Optional(Optional const& other) : hasValue(false) {
		if (other.hasValue)
			construct(other.get());
	}
	~Optional() {
		reset();
	}
	Optional& operator=(const Optional& other) {
		if (this == &other)
			return *this;
		reset();
		if (other.hasValue)
			construct(other.get());
		return *this;
	}

	bool empty() const {
		return !hasValue;
	}

	T& get() {
		if (!hasValue)
			throw std::runtime_error("Optional::get on empty Optional");
		return *ptr();
	}
	const T& get() const {
		if (!hasValue)
			throw std::runtime_error("Optional::get on empty Optional");
		return *ptr_const();
	}
	void set(T const& value) {
		reset();
		construct(value);
	}
	void reset() {
		if (!hasValue)
			return;
		ptr()->~T();
		hasValue = false;
	}
};
#endif
