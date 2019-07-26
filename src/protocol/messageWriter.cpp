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
#include "tribe/protocol/messageWriter.hpp"

#include "encoder.hpp"


using namespace tribe;
using namespace Solace;



Solace::ByteWriter&
MessageWriter::build() {
	return _writer;
}


Encoder&
writeHeader(Encoder& encoder, Gossip::MessageType messageType) {
// size_type const msgTotalSize = headerSize() + payload;
// encoder << msgTotalSize;
	return encoder << static_cast<byte>(messageType);
}

MessageWriter&
MessageWriter::join(NodeInfo const& self) {
	return join(self, MemoryView{}, MemoryView{});
}


MessageWriter&
MessageWriter::join(NodeInfo const& self, MemoryView token, MemoryView auth) {
	Encoder encode(_writer);

	writeHeader(encode, Gossip::MessageType::JoinReq)
			<< self
			<< token
			<< auth;

	return (*this);
}

MessageWriter&
MessageWriter::joinAck(NodeInfo const& self) {
	Encoder encode(_writer);

	writeHeader(encode, Gossip::MessageType::JoinAck)
			<< self;

	return (*this);
}


MessageWriter&
MessageWriter::joinRedirect(Error reason, Address const& redirectAddress) {
	Encoder encode(_writer);

	writeHeader(encode, Gossip::MessageType::JoinRedirect)
			<< redirectAddress
			<< static_cast<uint64>(reason.domain())
			<< static_cast<uint64>(reason.value())
			<< reason.tag();

	return (*this);
}

MessageWriter&
MessageWriter::joinNack(Error reason) {
	Encoder encode(_writer);

	writeHeader(encode, Gossip::MessageType::JoinNak)
			<< static_cast<uint64>(reason.domain())
			<< static_cast<uint64>(reason.value())
			<< reason.tag();

	return (*this);
}


MessageWriter&
MessageWriter::advertise(NodeInfo const& node) {
	Encoder encode(_writer);

	writeHeader(encode, Gossip::MessageType::Broadcast)
			<< node;

	return (*this);
}


MessageWriter&
MessageWriter::ping(NodeID requestorId, NodeID targetId) {
	Encoder encode{_writer};
	writeHeader(encode, Gossip::MessageType::PingDirect)
			<< requestorId
			<< targetId;

	return (*this);
}

MessageWriter&
MessageWriter::pong(NodeID requestorId, const NodeInfo& targetInfo) {
	Encoder encode{_writer};
	writeHeader(encode, Gossip::MessageType::PongDirect)
			<< requestorId
			<< targetInfo;

	return (*this);
}
