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
#include "encoder.hpp"

#include <netinet/in.h>
#include <arpa/inet.h>

using namespace Solace;

namespace tribe {

Encoder& operator<< (Encoder& encoder, Solace::uint8 value) {
	encoder.writer().writeLE(value);
	return encoder;
}

Encoder& operator<< (Encoder& encoder, Solace::uint16 value) {
	encoder.writer().writeLE(value);
	return encoder;
}

Encoder& operator<< (Encoder& encoder, Solace::uint32 value)  {
	encoder.writer().writeLE(value);
	return encoder;
}

Encoder& operator<< (Encoder& encoder, Solace::uint64 value)  {
	encoder.writer().writeLE(value);
	return encoder;
}

Encoder& operator<< (Encoder& encoder, Solace::StringView data) {
	encoder << data.size();
	encoder.writer().write(data.view());

	return encoder;
}

Encoder& operator<< (Encoder& encoder, Solace::MemoryView data) {
	encoder << data.size();
	encoder.writer().write(data);

	return encoder;
}

Encoder&
operator<< (Encoder& out, NodeID id) {
	return out << id.value;
}


void writeAddress(ByteWriter& writer, sockaddr_in const& addr) {
	writer.write(addr.sin_port);
	writer.write(addr.sin_addr.s_addr);
}


void writeAddress(ByteWriter& writer, sockaddr_in6 const& addr) {
	writer.write(addr.sin6_port);
	auto view = wrapMemory(addr.sin6_addr.__in6_u.__u6_addr8);
	writer.write(view);
}


Encoder&
operator<< (Encoder& out, Address const& address) {
	auto& writer = out.writer();
	out << address.addr.ss_family;

	switch (address.addr.ss_family) {
	case AF_INET: {
		writeAddress(writer, *reinterpret_cast<sockaddr_in const*>(&address.addr));
	} break;
	case AF_INET6: {
		writeAddress(writer, *reinterpret_cast<sockaddr_in6 const*>(&address.addr));
	} break;
	default:
		// TODO(abbyssoul): Error handling required
		break;
	}

	return out;
}


Encoder&
operator<< (Encoder& out, NodeInfo const& node) {
	return out << node.id
			   << node.gen;
}


}  // namespace tribe
