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
#ifndef TRIBE_PROTOCOL_GOSSIP_HPP
#define TRIBE_PROTOCOL_GOSSIP_HPP

#include "../nodeInfo.hpp"

#include <solace/stringView.hpp>
#include <solace/string.hpp>
#include <solace/byteReader.hpp>
#include <solace/byteWriter.hpp>

#include <solace/result.hpp>
#include <solace/error.hpp>

#include <variant>


namespace tribe {

/// Requrest to connect to a recepient and join it's group
struct ConnectRequest {
	NodeInfo		nodeInfo;
	Solace::MemoryView	token;
	Solace::MemoryView	auth;
};

/// Response acknowleding a node joining the group of the sender
struct ConnectResponseAck {
	NodeInfo		self;  	// The peer you have joined
	// TODO(abbyssoul): Encoded base state a view of the group this peer is sharing with requestor
};

/// Response when peer cannot accept requestor into a group but someone else in the group may
struct ConnectResponseRedirect {
	Address			otherNode;
	Solace::Error	reason;
};

/// Response when peer rejects connection request in a polite manner
struct ConnectResponseRejected {
	Solace::Error		reason;
};

/// "Are you there?"
struct PingMessage {
	NodeID			origin;
	NodeID			target;
	Solace::uint8		ttl;
};

/// "Hey! I am alive" Message
struct PongMessage {
	NodeID			origin;
	NodeInfo		nodeDetails;
	Solace::uint8		ttl;
};

/// Broadcast message to elicit peer intoduction
struct BroadcastMessage {
	NodeInfo		node;
};

using Message = std::variant<ConnectRequest,
							ConnectResponseAck,
							ConnectResponseRedirect,
							ConnectResponseRejected,

							PingMessage, PongMessage,
							BroadcastMessage>;


/**
 * Netowork Protocol utils
 */
struct Gossip {

	/// Network protocol uses fixed width uint16 to represent size of data in bytes
	using size_type = Solace::uint16;

	/// Message types
	enum class MessageType : Solace::byte {
		JoinReq = 63,
		JoinAck,
		JoinRedirect,
		JoinNak,

		PingDirect,
		PongDirect,

		Broadcast = 250
	};

	/// Mandatory fixed size message header
	struct MessageHeader {
		MessageType type;
	};

	/// Get the size of a buffer in bytes required to store message header
	constexpr static size_type headerSize() noexcept {
		return sizeof(MessageHeader::type);
	}


	Gossip() = delete;
	Gossip(Gossip const& ) = delete;
	Gossip& operator= (Gossip const& ) = delete;
};


}  // namespace tribe
#endif  // TRIBE_PROTOCOL_GOSSIP_HPP
