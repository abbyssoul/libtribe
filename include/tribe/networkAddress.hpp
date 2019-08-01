/*
*  Copyright 2019 Ivan Ryabov
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*/
#pragma once
#ifndef TRIBE_NETWORKADDRESS_HPP
#define TRIBE_NETWORKADDRESS_HPP

#include <solace/result.hpp>
#include <solace/error.hpp>

#include <functional>

#include <sys/socket.h>
// #include <netinet/in.h>


namespace tribe {

/// Network address
struct Address {
	size_t				size;
	sockaddr_storage	addr;

	/*
	sockaddr	addr;
	union data_union {
	  sockaddr base;
	  sockaddr_in v4;
	  sockaddr_in6 v6;
	} data_;
*/

	constexpr Address() noexcept
		: size{0}
		, addr{0, {0}, 0}
	{
	}

	Address(size_t addrSize, sockaddr_storage const& soAddr) noexcept;
};

bool operator== (Address const& lhs, Address const& rhs) noexcept;
bool operator!= (Address const& lhs, Address const& rhs) noexcept;

size_t hashAddress(Address const& addr) noexcept;

Address anyAddress(Solace::uint16 port) noexcept;
Solace::Result<Address, Solace::Error> tryParseAddress(Solace::StringView value);

}  // namespace tribe


namespace std {
template <>
struct hash<tribe::Address> {
	size_t operator()(tribe::Address const& value) const noexcept {
		return hashAddress(value);
	}
};
}

#endif  // TRIBE_NETWORKADDRESS_HPP
