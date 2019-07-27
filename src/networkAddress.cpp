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
#include "tribe/networkAddress.hpp"

#include <solace/posixErrorDomain.hpp>

#include <algorithm>  // std::min
#include <cstring>
#include <variant>

#include <netinet/in.h>
#include <arpa/inet.h>


using namespace Solace;


namespace tribe {


bool operator== (Address const& lhs, Address const& rhs) noexcept {
	return (lhs.size == rhs.size) && (std::memcmp(&lhs.addr, &rhs.addr, lhs.size) == 0);

}

bool operator!= (Address const& lhs, Address const& rhs) noexcept {
	return (lhs.size != rhs.size) || (std::memcmp(&lhs.addr, &rhs.addr, lhs.size) != 0);
}


size_t hashAddress(Address const& address) noexcept {
	switch (address.addr.ss_family) {
	case AF_INET: {
		sockaddr_in const* so_addr = reinterpret_cast<sockaddr_in const*>(&address.addr);
		return std::hash<in_addr_t>{}(so_addr->sin_addr.s_addr);
	}
	case AF_INET6: {
		sockaddr_in6 const* so_addr = reinterpret_cast<sockaddr_in6 const*>(&address.addr);

		// FIXME: Need a proper hash function for ip6 AND port
		size_t hashValue = std::hash<uint32_t>{}(so_addr->sin6_port);
		for (auto value : so_addr->sin6_addr.__in6_u.__u6_addr8) {
			hashValue ^= ((hashValue << 5) + value + (hashValue >> 2));
		}

		return hashValue;
	}
	default:
		// TODO(abbyssoul): Error handling required
		break;
	}

	return std::hash<size_t>{}(address.size);
}


Result<in6_addr, int>
tryParseIP6(StringView str) {
	if (str.empty())
		return Err(0);

	if (str.size() >= INET6_ADDRSTRLEN)
		return Err(1);

	char presentationString[INET6_ADDRSTRLEN] = {0};	 // FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:255.255.255.255
	memset(presentationString, 0, sizeof(presentationString));
	strncpy(presentationString, str.data(), str.size());

	in6_addr result;
	if (inet_pton(AF_INET6, presentationString, &result) <= 0) {
		return Err(1);
	}

	return Ok(result);
}

Result<in_addr, int>
tryParseIP4(StringView str) {
	if (str.empty())
		return Err(0);

	if (str.size() >= INET_ADDRSTRLEN)
		return Err(1);

	char presentationString[INET_ADDRSTRLEN] = {0};	 // 255.255.255.255
	memset(presentationString, 0, sizeof(presentationString));
	strncpy(presentationString, str.data(), str.size());

	in_addr result;
	if (inet_pton(AF_INET, presentationString, &result) <= 0) {
		return Err(1);
	}

	return Ok(result);
}

using IPAddress = std::variant<in_addr, in6_addr>;

Result<IPAddress, Error>
parseIPAddress(StringView str) {
	if (auto r1 = tryParseIP4(str))
		return Ok(r1.moveResult());

	if (auto r2 = tryParseIP6(str))
		return Ok(r2.moveResult());

	return Err(makeError(BasicError::InvalidInput, "parseIPAddress"));
}



Address
anyAddress(uint16 port) noexcept {
	sockaddr_in any;
	any.sin_family = AF_INET;
	any.sin_port = htons(port);
	any.sin_addr.s_addr = INADDR_ANY;

	return Address {
		sizeof(sockaddr_in),
		*reinterpret_cast<sockaddr_storage*>(&any)
	};
}

Address
asAddress(IPAddress const& address, uint16 port) {

	struct IPHandler {

		Address operator() (in_addr const& addr) noexcept {
			sockaddr_in so_addr;
			so_addr.sin_family = AF_INET;
			so_addr.sin_port = htons(port);
			so_addr.sin_addr = addr;

			return Address {
				sizeof(so_addr),
				*reinterpret_cast<sockaddr_storage*>(&so_addr)
			};
		}

		Address operator() (in6_addr const& addr) noexcept {
			sockaddr_in6 so_addr;
			so_addr.sin6_family = AF_INET6;
			so_addr.sin6_port = htons(port);
			so_addr.sin6_addr = addr;

			return Address {
				sizeof(so_addr),
				*reinterpret_cast<sockaddr_storage*>(&so_addr)
			};
		}

		uint16 port;
	};

	return std::visit(IPHandler{port}, address);
}

Result<Address, Error>
tryParseAddress(StringView src) {
	StringView input = src.trim();
	StringView portValue;
	StringView ipString;

	if (input.startsWith('[')) {  // IPv6 address
		input.split("]:", [&portValue, &ipString](StringView split, StringView::size_type i, StringView::size_type count) {
			if (count == 1)  {
				ipString = split;  // No split - single stirng starting with '[' ???
				return;
			}

			if (i + 1 == count) {  // Last element
				portValue = split;
			} else {
				ipString = split.substring(1);  // Skip initial '['
			}
		});
	} else {
		input.split(":", [&portValue, &ipString](StringView split, StringView::size_type i, StringView::size_type count) {
			if (count == 1) {
				ipString = split;  // No splitting - string does not contain ':'
				return;
			}

			if (i + 1 == count) {
				portValue = split;  // Last element
			} else {
				ipString = split;
			}
		});
	}

	if (portValue.empty()) {
		return Err(makeError(BasicError::InvalidInput, "tryParseAddress"));
	}

	auto address = parseIPAddress(ipString);
	if (!address) {
		return Err(address.moveError());
	}

	// FIXME: Use uint16 parser to catch uint16 overflow
	auto const maybePort = (portValue.empty())
			? 0
			: std::strtoul(portValue.data(), nullptr, 10);

	if (maybePort > std::numeric_limits<uint16>::max()) {
		return Err(makeError(BasicError::Overflow, "tryParseAddress:port"));
	}

	auto const port = narrow_cast<uint16>(maybePort);
	return Ok(asAddress(*address, port));
}

}  // namespace tribe
