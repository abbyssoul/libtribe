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
#ifndef TRIBE_PROTOCOL_ENCODER_HPP
#define TRIBE_PROTOCOL_ENCODER_HPP

#include "tribe/protocol/gossip.hpp"

#include <solace/byteWriter.hpp>


namespace tribe {


struct Encoder {

	constexpr Encoder(Solace::ByteWriter& dest) noexcept
		: _dest{dest}
	{}

	Encoder(Encoder const&) = delete;
	Encoder& operator= (Encoder const&) = delete;

	Solace::ByteWriter& writer() noexcept { return _dest; }

private:

	Solace::ByteWriter& _dest;

};

Encoder& operator<< (Encoder& encoder, Solace::uint8 value);
Encoder& operator<< (Encoder& encoder, Solace::uint16 value);
Encoder& operator<< (Encoder& encoder, Solace::uint32 value);
Encoder& operator<< (Encoder& encoder, Solace::uint64 value);
Encoder& operator<< (Encoder& encoder, Solace::StringView str);
Encoder& operator<< (Encoder& encoder, Solace::MemoryView str);

Encoder& operator<< (Encoder& encoder, NodeID id);
Encoder& operator<< (Encoder& encoder, Address const& addr);
Encoder& operator<< (Encoder& encoder, NodeInfo const& node);


}  // namespace tribe
#endif  // TRIBE_PROTOCOL_ENCODER_HPP
