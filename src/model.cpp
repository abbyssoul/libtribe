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
#include "tribe/model.hpp"

#include <functional>  // std::remove_if


using namespace Solace;
using namespace tribe;


const float32 Peer::kCertainlyAlive = 0.97f;
const float32 Peer::kMaybeNotAlive = 0.65f;
const float32 Peer::kCertainlyNotAlive = 0.20f;


namespace /* anonymous */ {


// Util function to remove items from collections
template<typename C, typename F>
void
eraseIf(C& container, F&& pre) {
	container.erase(
				std::remove_if(container.begin(), container.end(), std::forward<F>(pre)),
				container.end());
}

auto
expDecay(decltype(Peer::Liveness::probabitily) value, float32 k, float32 dt) noexcept {
	// Exponential decay of aliveness certainty: dP / dt = -kP
	// (p1 - p0) / dt = -kp0
	// p1 = p0 - kp0*dt = p0*(1 - kdt)
	return value * (1.0f - k*dt);
}


auto
decayLiveness(Peer::Liveness value, uint16 dt, float32 decayRate, uint32 decayTimeMs) noexcept {
	// Exponential decay of aliveness certainty: dP / dt = -kP
	value.probabitily = expDecay(value.probabitily, decayRate, dt*(decayTimeMs / 1000.f));

	// Decrement TTL of the nodes
	value.ttl = (value.ttl > 0)
			? (value.ttl - dt)  // FIXME: Underflow?
			: 0;


	// Update state
	switch (value.state) {
	case Peer::State::Alive: {
		if (value.probabitily < Peer::kMaybeNotAlive) {
			value.state = Peer::State::Suspected;
		}
	} break;
	case Peer::State::Suspected: {
		if (value.ttl == 0) {  // Expired suspected state transition to be considered 'dead'
			value.state = Peer::State::Dead;
		}
	} break;
	case Peer::State::Dead: break;  // Nothing to go from here
	}

	return value;
}

}  // anonymous namespace


namespace /* anonymous */ {

PeersModel
addSeed(PeersModel state, AddSeed&& seedAction) {
	state.seeds.try_emplace(std::move(seedAction.address), SeedPeer{seedAction.ttl});

	return state;
}

PeersModel
dropSeed(PeersModel state, Address const& address) {
	state.seeds.erase(address);

	return state;
}


PeersModel
addPeer(PeersModel state, AddPeer&& peerAction) {
	if (state.node.id == peerAction.nodeInfo.id) {  // Don't add `self` into the routing table
		return state;
	}

	state.members.try_emplace(peerAction.nodeInfo.id,
							  peerAction.nodeInfo.gen,
							  std::move(peerAction.address),
							  peerAction.ttl,
							  Peer::kCertainlyAlive);

	return state;
}


PeersModel
dropPeer(PeersModel state, NodeID peerId) {
	state.members.erase(peerId);

	return state;
}


PeersModel
pronouncePeerDead(PeersModel state, NodeID peerId) {
	auto it = state.members.find(peerId);
	if (it != state.members.end()) {
		it->second.liveness.state = Peer::State::Dead;
	}

	return state;
}

PeersModel
updatePeerInfo(PeersModel state, UpdatePeerGeneration&& action) {
	auto it = state.members.find(action.peerId);
	if (it != state.members.end()) {
		auto& member = it->second;
		if (member.generation <= action.gen) {  // Update info iff newer generation
			member.generation = action.gen;

			member.liveness.ttl = action.ttl;
			member.liveness.probabitily = Peer::kCertainlyAlive;
		}
	}

	return state;
}

PeersModel
updatePeerAddress(PeersModel state, UpdatePeerAddress&& action) {
	auto it = state.members.find(action.peerId);
	if (it != state.members.end()) {
		it->second.address = action.newAddress;
	}

	return state;
}


PeersModel
decayPeerInfo(PeersModel state, DecayPeerInfo decayParams) {
	// Dacaying info producess side-effects - peers change states
	for (auto& p : state.members) {
		p.second.liveness = decayLiveness(p.second.liveness,
										  decayParams.ttlDelta,
										  decayParams.decayRate,
										  decayParams.decayTimeMs);
	}

	// Remove expired peers
	for (auto it = state.members.begin(); it != state.members.end(); ) {
		if (PeersModel::isExpired(it->second)) {
			it = state.members.erase(it);
		} else {
			++it;
		}
	}

	return state;
}

}  // anonymous namespace



Optional<Address>
PeersModel::findRedirectAddress() const {
	for (auto const& peer : members) {
		if (isHealthy(peer.second)) {
			return peer.second.address;
		}
	}

	return none;
}


PeersModel
tribe::update(PeersModel const& state, Action&& action) {

	struct ActionHandler {
		PeersModel const& state;

		PeersModel operator() (AddSeed&& request) const { return addSeed(state, std::move(request)); }
		PeersModel operator() (ForgetSeed&& action) const { return dropSeed(state, std::move(action.address)); }

		PeersModel operator() (AddPeer&& request) const { return addPeer(state, std::move(request)); }
		PeersModel operator() (ForgetPeer&& action) const { return dropPeer(state, action.peerId); }

		PeersModel operator() (UpdatePeerGeneration&& action) const { return updatePeerInfo(state, std::move(action)); }
		PeersModel operator() (UpdatePeerAddress&& action) const { return updatePeerAddress(state, std::move(action)); }


		PeersModel operator() (DecayPeerInfo&& action) const { return decayPeerInfo(state, action); }
		PeersModel operator() (PronouncePeerDead&& action) const { return pronouncePeerDead(state, action.peerId); }
	};

	return std::visit(ActionHandler{state}, std::move(action));

}
