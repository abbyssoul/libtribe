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
#include "tribe/protocol/messageParser.hpp"

#include "decoder.hpp"


using namespace tribe;
using namespace Solace;


namespace /* anonymous */ {


Result<Message, MessageParser::Error>
parseJoinRequest(ByteReader& reader) {
	ConnectRequest msg;

	Decoder decoder{reader};
	auto result = decoder.read(&msg.nodeInfo)
			.then([&](){ return decoder.read(&msg.token); })
			.then([&](){ return decoder.read(&msg.auth);  });

	if (!result) {
		return Err(MessageParser::Error{});
	}

	return Ok(std::move(msg));
}


Result<Message, MessageParser::Error>
parseJoinAck(ByteReader& reader) {
	ConnectResponseAck msg;

	Decoder decoder{reader};
	auto result = decoder.read(&msg.self);
  // .then([&](){ return decoder.read(&msg.snapshot);  });

	if (!result) {
		return Err(MessageParser::Error{});
	}

	return Ok(std::move(msg));
}

Result<Message, MessageParser::Error>
parseJoinRedirect(ByteReader& reader) {

	Address otherNode;
	uint64 reasonDomain{};
	uint64 reasonCode{};
	StringView reasonMessage{};

	Decoder decoder{reader};
	auto result = decoder.read(&otherNode)
			.then([&](){ return decoder.read(&reasonDomain);	})
			.then([&](){ return decoder.read(&reasonCode);		})
			.then([&](){ return decoder.read(&reasonMessage);	});

	if (!result) {
		return Err(MessageParser::Error{});
	}

	ConnectResponseRedirect msg {
				std::move(otherNode),
				Error(static_cast<AtomValue>(reasonDomain), reasonCode, StringLiteral{})
	};

	return Ok(std::move(msg));
}


Result<Message, MessageParser::Error>
parseConnectRejected(ByteReader& reader) {
	uint64 reasonDomain{};
	uint64 reasonCode{};
	StringView reasonMessage{};

	Decoder decoder{reader};
	auto result = decoder.read(&reasonDomain)
						.then([&](){ return decoder.read(&reasonCode);		})
						.then([&](){ return decoder.read(&reasonMessage);	});

	if (!result) {
		return Err(MessageParser::Error{});
	}

	ConnectResponseRejected msg {
				Error(static_cast<AtomValue>(reasonDomain), reasonCode, StringLiteral{})
	};

	return Ok(std::move(msg));
}
Result<Message, MessageParser::Error>
parseBroadcastMessage(ByteReader& reader) {
	Decoder decoder{reader};
	BroadcastMessage msg;

	auto result = decoder.read(&msg.node);

	if (result) {
		return Ok(std::move(msg));
	}

	return Err(MessageParser::Error{});
}


Result<Message, MessageParser::Error>
parsePingMessage(ByteReader& reader) {
	PingMessage msg;

	Decoder decoder{reader};
	auto result = decoder.read(&msg.origin)
						 .then([&](){ return decoder.read(&msg.target); })
						 .then([&](){ return decoder.read(&msg.ttl); });

	if (!result) {
		return Err(MessageParser::Error{});
	}

	return Ok(std::move(msg));
}

Result<Message, MessageParser::Error>
parsePongMessage(ByteReader& reader) {
	PongMessage msg;

	Decoder decoder{reader};
	auto result = decoder.read(&msg.origin)
						 .then([&](){ return decoder.read(&msg.nodeDetails); })
						 .then([&](){ return decoder.read(&msg.ttl); });
	if (!result) {
		return Err(MessageParser::Error{});
	}

	return Ok(std::move(msg));
}

}  // anonymous namespace


Result<Gossip::MessageHeader, MessageParser::Error>
MessageParser::parseMessageHeader(ByteReader& buffer) const {
	const auto mandatoryHeaderSize = Gossip::headerSize();
	const auto dataAvailliable = buffer.remaining();

	// Check that we have enough data to read mandatory message header
	if (dataAvailliable < mandatoryHeaderSize) {
		return Err(MessageParser::Error{});
	}

	Gossip::MessageHeader header;
//    buffer.readLE(header.msgSize);

//    // Sanity checks:
//    // It is a serious error if server responded with the message of a size bigger than negotiated one.
//    if (header.msgSize < headerSize()) {
//        return Err(MessageParser::Error{});
//    }

//    if (header.msgSize > maxMessageSize()) {
//        return Err(MessageParser::Error{});
//    }

	// Read message type:
	byte messageBytecode;
	buffer.readLE(messageBytecode);
	// don't want any funny messages.
	header.type = static_cast<Gossip::MessageType>(messageBytecode);
//    if (header.type < MessageType::_beginSupportedMessageCode ||
//        header.type >= MessageType::_endSupportedMessageCode) {
//        return Err(MessageParser::Error{});
//    }

	// Read message tag. Tags are provided by the client and can not be checked by the message parser.
	// Unless we are provided with the expected tag...
//    buffer.readLE(header.tag);

	return Ok(header);
}


Result<Message, MessageParser::Error>
MessageParser::parse(ByteReader& reader) const {
	auto maybeHeader = parseMessageHeader(reader);
	if (!maybeHeader) {
		return Err(maybeHeader.moveError());
	}

	auto& header = maybeHeader.unwrap();
	switch (header.type) {
	case Gossip::MessageType::JoinReq:			return parseJoinRequest(reader);
	case Gossip::MessageType::JoinAck:			return parseJoinAck(reader);
	case Gossip::MessageType::JoinRedirect:		return parseJoinRedirect(reader);
	case Gossip::MessageType::JoinNak:			return parseConnectRejected(reader);

	case Gossip::MessageType::PingDirect:		return parsePingMessage(reader);
	case Gossip::MessageType::PongDirect:		return parsePongMessage(reader);

	case Gossip::MessageType::Broadcast:		return parseBroadcastMessage(reader);
	}

	return Err(MessageParser::Error{});
}
