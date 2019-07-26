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
	/**
	 * Compute the number of bytes in the buffer required to store a given value.
	 * @param value Value to store in the message.
	 * @return Number of bytes required to represent the value given.
	 */
	static Gossip::size_type protocolSize(Solace::uint8 const& value) noexcept;
	/**
	 * Compute the number of bytes in the buffer required to store a given value.
	 * @param value Value to store in the message.
	 * @return Number of bytes required to represent the value given.
	 */
	static Gossip::size_type protocolSize(Solace::uint16 const& value) noexcept;
	/**
	 * Compute the number of bytes in the buffer required to store a given value.
	 * @param value Value to store in the message.
	 * @return Number of bytes required to represent the value given.
	 */
	static Gossip::size_type protocolSize(Solace::uint32 const& value) noexcept;
	/**
	 * Compute the number of bytes in the buffer required to store a given value.
	 * @param value Value to store in the message.
	 * @return Number of bytes required to represent the value given.
	 */
	static Gossip::size_type protocolSize(Solace::uint64 const& value) noexcept;
	/**
	 * Compute the number of bytes in the buffer required to store a given value.
	 * @param value Value to store in the message.
	 * @return Number of bytes required to represent the value given.
	 */
	static Gossip::size_type protocolSize(Solace::StringView const& value) noexcept;

	static Gossip::size_type protocolSize(NodeID id) noexcept;
	static Gossip::size_type protocolSize(Address const& address) noexcept;
	static Gossip::size_type protocolSize(NodeInfo const& node) noexcept;


	constexpr Encoder(Solace::ByteWriter& dest) noexcept
		: _dest{dest}
	{}

	Encoder(Encoder const&) = delete;
	Encoder& operator= (Encoder const&) = delete;


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
