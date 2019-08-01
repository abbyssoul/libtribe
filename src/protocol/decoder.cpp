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
#include "decoder.hpp"

#include "tribe/protocol/gossip.hpp"

#include <solace/posixErrorDomain.hpp>

#include <netinet/in.h>
#include <arpa/inet.h>


using namespace Solace;


namespace tribe {

Result<void, Error>
Decoder::read(uint8* dest) {
	return _src.readLE(*dest);
}


Result<void, Error>
Decoder::read(uint16* dest) {
	return _src.readLE(*dest);
}

Result<void, Error>
Decoder::read(uint32* dest) {
	return _src.readLE(*dest);
}

Result<void, Error>
Decoder::read(uint64* dest) {
	return _src.readLE(*dest);
}

Result<void, Error>
Decoder::read(StringView* dest) {
	uint16 dataSize = 0;

	return _src.readLE(dataSize)
			.then([&]() {
				StringView view(_src.viewRemaining().dataAs<const char>(), dataSize);
				return _src.advance(dataSize)
						.then([dest, &view]() {
							*dest = view;
						});
			});
}

Result<void, Error>
Decoder::read(MemoryView* data) {
	Gossip::size_type dataSize = 0;
	// Read size of the following data.
	return read(&dataSize)
			.then([&]() {
				if (dataSize <= _src.remaining()) {
					// Read the data. Note we only take a view into the actual message buffer.
					*data = _src.viewRemaining().slice(0, dataSize);
				}

				return _src.advance(dataSize);
			});
}


void readAddress(ByteReader& reader, sockaddr_in& addr) {
	reader.read(&(addr.sin_port));
	reader.read(&(addr.sin_addr.s_addr));
}


void readAddress(ByteReader& reader, sockaddr_in6& addr) {
	reader.read((&addr.sin6_port));
	auto view = wrapMemory(addr.sin6_addr.__in6_u.__u6_addr8);
	reader.read(view);
}


Result<void, Error>
Decoder::read(Address* addr) {
	auto r = read(&addr->addr.ss_family);
	if (!r) {
		 return Err(r.getError());
	}

	switch (addr->addr.ss_family) {
	case AF_INET: {
		addr->size = sizeof (sockaddr_in);
		readAddress(_src, *reinterpret_cast<sockaddr_in*>(&addr->addr));
	} break;
	case AF_INET6: {
		addr->size = sizeof (sockaddr_in6);
		readAddress(_src, *reinterpret_cast<sockaddr_in6*>(&addr->addr));
	} break;
	default:
		return Err(makeError(BasicError::InvalidInput, "address family"));
	}

	return Ok();
}

}  // namespace tribe
