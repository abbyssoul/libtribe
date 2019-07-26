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
#ifndef TRIBE_PROTOCOL_MESSAGEBUILDER_HPP
#define TRIBE_PROTOCOL_MESSAGEBUILDER_HPP

#include "gossip.hpp"


namespace tribe {

/**
 * Gossip Message Writer
 * This is an adapter for byte writer that writes gossip messages
 * directly into an output buffer.
 */
struct MessageWriter {

	constexpr MessageWriter(Solace::ByteWriter& dest) noexcept
		: _writer(dest)
	{}

	constexpr Solace::ByteWriter& writer() noexcept { return _writer; }

	Solace::ByteWriter& build();

	MessageWriter& join(NodeInfo const& self);
	MessageWriter& join(NodeInfo const& self, Solace::MemoryView token, Solace::MemoryView auth);

	MessageWriter& joinAck(NodeInfo const& self);
	MessageWriter& joinNack(Solace::Error reason);
	MessageWriter& joinRedirect(Solace::Error reason, Address const& redirectAddress);

	MessageWriter& advertise(NodeInfo const& state);
	MessageWriter& ping(NodeID requestorId, NodeID targetId);
	MessageWriter& pong(NodeID requestorId, NodeInfo const& selfInfo);

private:

	Solace::ByteWriter&     _writer;
};

}  // namespace tribe
#endif  // TRIBE_PROTOCOL_MESSAGEBUILDER_HPP
