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
#include "tribe/model.hpp"

#include <netinet/in.h>
#include <arpa/inet.h>

#include <cstring>
#include <iomanip>

namespace tribe {


std::ostream& operator<< (std::ostream& ostr, Address const& address) {

	switch (address.addr.ss_family) {
	case AF_INET: {
		char maxBuffer[INET_ADDRSTRLEN];
		sockaddr_in const* so_addr = reinterpret_cast<sockaddr_in const*>(&address.addr);
		inet_ntop(address.addr.ss_family, &(so_addr->sin_addr), maxBuffer, INET_ADDRSTRLEN);

		ostr.put('\'');
		ostr.write(maxBuffer, strnlen(maxBuffer, INET_ADDRSTRLEN));
		ostr.put(':');
		ostr << ntohs(so_addr->sin_port);;
		ostr.put('\'');

	}break;
	case AF_INET6: {
		char maxBuffer[INET6_ADDRSTRLEN];
		sockaddr_in6 const* so_addr = reinterpret_cast<sockaddr_in6 const*>(&address.addr);
		inet_ntop(address.addr.ss_family, &(so_addr->sin6_addr), maxBuffer, INET6_ADDRSTRLEN);

		ostr.put('\'');
		ostr.write(maxBuffer, strnlen(maxBuffer, INET6_ADDRSTRLEN));
		ostr.put(':');
		ostr << ntohs(so_addr->sin6_port);;
		ostr.put('\'');
	} break;
	default: ostr << "<Unknown Address type '" << address.addr.ss_family << "'>";
		break;
	}

	return ostr;
}


std::ostream& operator<< (std::ostream& ostr, NodeID const& id) {
	return ostr << std::hex << id.value << std::dec;
}


std::ostream& operator<< (std::ostream& ostr, NodeInfo const& node) {
	return ostr << "{id: '" << node.id
				<< "', " << "gen: "  << node.gen
				<< "}";
}

std::ostream& operator<< (std::ostream& ostr, Peer const& peer) {
	return ostr << "{ generation: " << peer.generation
				<< ", " << "address: " << peer.address
				<< ", " << "ttl: " << peer.liveness.ttl
				<< ", " << "alive: " << std::fixed << std::setprecision(2) << peer.liveness.probabitily
				<< "}";
}

}  // namespace tribe
