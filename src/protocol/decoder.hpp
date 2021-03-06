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
#ifndef TRIBE_PROTOCOL_DECODER_HPP
#define TRIBE_PROTOCOL_DECODER_HPP

#include "tribe/nodeInfo.hpp"

#include <solace/byteReader.hpp>


namespace tribe {


struct Decoder {

	constexpr Decoder(Solace::ByteReader& src) noexcept
		: _src(src)
	{}

	Decoder(Decoder const&) = delete;
	Decoder& operator= (Decoder const&) = delete;

	Solace::Result<void, Solace::Error> read(Solace::uint8* dest);
	Solace::Result<void, Solace::Error> read(Solace::uint16* dest);
	Solace::Result<void, Solace::Error> read(Solace::uint32* dest);
	Solace::Result<void, Solace::Error> read(Solace::uint64* dest);
	Solace::Result<void, Solace::Error> read(Solace::StringView* dest);
	Solace::Result<void, Solace::Error> read(Solace::MemoryView* dest);

	Solace::Result<void, Solace::Error> read(NodeID* id) { return read(&id->value); }

	Solace::Result<void, Solace::Error> read(Address* addr);

	Solace::Result<void, Solace::Error> read(NodeInfo* node) {
		return read(&node->id)
				.then([&]() { return read(&node->gen); });
	}

private:
	Solace::ByteReader& _src;
};

}  // namespace tribe
#endif  // TRIBE_PROTOCOL_DECODER_HPP
