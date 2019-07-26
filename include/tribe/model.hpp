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
#ifndef TRIBE_MODEL_HPP
#define TRIBE_MODEL_HPP

#include "nodeInfo.hpp"

#include <solace/range_view.hpp>

#include <map>
#include <variant>
#include <functional>  // std::function - to handle side-effects


namespace tribe {

/**
 * Value of seed peer
 * This is a peer we have not connected to yet but should try
 */
struct SeedPeer {
	Address         address;    //!< Network address of the node.
	Solace::uint16	ttl;		//!< Number of connection attempt before we give up trying to connect to this node.

	constexpr SeedPeer(Address addr, Solace::uint16 baseTtl) noexcept
		: address{std::move(addr)}
		, ttl{baseTtl}
	{}
};



/**
 * Value representing local view of another group member/cluseter participants.
 * Each group member keeps track of a limited number of other nodes - its peers.
 * For each peer the node estimates the probability that that peer is still 'alive'
 */
struct Peer {
	static const Solace::float32 kCertainlyAlive;
	static const Solace::float32 kMaybeNotAlive;
	static const Solace::float32 kCertainlyNotAlive;

	enum class State {
		Alive,
		Suspected,
		Dead
	};

	struct Liveness {
		Solace::uint16      ttl;			//!< Number of ticks before we should start to worry if node is alive
		Solace::float32		probabitily;  	//!< The probability that the node is alive
		State				state;
	};

	Solace::uint32		generation;			//!< Generation of the node / individual node 'token'
	Address				address;			//!< Network address to reach this peer
	Liveness			liveness;			//!< Estimated state of the peer
	Solace::uint64		comSequance{0};     //!< Communication sequance number
	Solace::uint32		capacity{0};		//!< Estimated capacity of the peer
	Solace::uint32		peerCount{0};		//!< Estimated number of peers

	constexpr Peer(Solace::uint32 gen, Address rsvpAddress, Solace::uint16 baseTtl, Solace::float32 aliveness) noexcept
		: generation{gen}
		, address{std::move(rsvpAddress)}
		, liveness{baseTtl, aliveness, State::Alive}
		, comSequance{0}
		, capacity{0}
		, peerCount{0}
	{}
};


/// Group membership parameters
struct MembershipSettings {
	Solace::uint32		peerInfoDecayTimeMs{1300};
	Solace::float32		peerInfoDecayRate{0.3f};
	Solace::uint16		ttl{8};

	bool				allowedToJoin;  	//!< Does this node accept new connections?
	bool				allowedRedirect;  	//!< Can this node redirect connection requests to peer when at capacity?
	Solace::uint32		maxPeers{128};		//!< Max number of peer this node tracks
	Solace::uint32		samplingRate{3};  	//!< Max sample size if state info does not fit into a datagram buffer
};


/**
 * Model of cluster membership
 */
struct PeersModel {
	using PeerRangeView = Solace::RangeView<Peer, std::map<NodeID, Peer>::const_iterator>;

	static bool isAlive(Peer const& p) noexcept { return (p.liveness.state == Peer::State::Alive); }
	static bool isSuspected(Peer const& p) noexcept { return (p.liveness.state == Peer::State::Suspected); }
	static bool isDead(Peer const& p) noexcept { return (p.liveness.state == Peer::State::Dead); }
	static bool isExpired(Peer const& p) noexcept { return isDead(p) && (p.liveness.ttl == 0); }


	// return a list of peers that should be checked for liveness
	PeerRangeView suspectedPeers() const noexcept	{ return {members.begin(), members.end(), isSuspected}; }
	PeerRangeView deadPeers() const noexcept		{ return {members.begin(), members.end(), isDead}; }
	PeerRangeView expiredPeers() const noexcept		{ return {members.begin(), members.end(), isExpired}; }

	Solace::Optional<Address>
	findRedirectAddress() const;

	NodeInfo				node;
	MembershipSettings		params;

	std::vector<SeedPeer>				seeds;
	std::map<NodeID, Peer>	members;
};


//---------------------------------------------------------------------------
// Peer management
//---------------------------------------------------------------------------

/// Add a single seed to the list of seeds
struct AddSeed				{ Address address; Solace::uint16 ttl; };

/// Drop given seed
struct ForgetSeed			{ Address address; };

/// Add a single new peer into a peer table
struct AddPeer				{ Address address; NodeInfo	nodeInfo; Solace::uint16 ttl; };
/// Drop given peer from the peer table
struct ForgetPeer			{ NodeID	peerId; };
/// Update address for a particular peer
struct UpdatePeerAddress	{ NodeID	peerId; Address newAddress; };
/// Update record for a particular peer
struct UpdatePeerGeneration { NodeID	peerId; Solace::uint32  gen; Solace::uint16 ttl; };
/// Change the state of a peer to 'dead'.
struct PronouncePeerDead	{ NodeID	peerId; };
/// Decay infor about peer state as time passes
struct DecayPeerInfo		{ Solace::uint16 ttlDelta; 	Solace::uint32 decayTimeMs; Solace::float32	decayRate; };


/// Action to update peers model
using Action = std::variant<AddSeed, ForgetSeed,
							AddPeer, ForgetPeer,
							UpdatePeerAddress,
							UpdatePeerGeneration,
							PronouncePeerDead,
							DecayPeerInfo
							>;

/// Update Peer model by applying an action.s Note: pure function.
PeersModel
update(PeersModel const& state, Action&& action);

/// Update Peer model by applying an action.s Note: pure function.
PeersModel
update(PeersModel const& state, Action&& action, std::function<void()>);

}  // namespace tribe
#endif  // TRIBE_MODEL_HPP
