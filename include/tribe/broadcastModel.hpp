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
#ifndef TRIBE_BROADCASTMODEL_HPP
#define TRIBE_BROADCASTMODEL_HPP

#include "networkAddress.hpp"
#include "nodeInfo.hpp"

#include <unordered_map>
//#include <unordered_set>
#include <variant>


namespace tribe {
struct BroadcastGroup;
}

namespace std {
	template <> struct hash<tribe::BroadcastGroup> {
		size_t operator()(tribe::BroadcastGroup const& value) const noexcept;
	};
}



namespace tribe {

/// Broadcast group model
struct BroadcastGroup {
//	Address						address;				/// Network broadcast adddress eg. {"238.255.0.1", 5670};

	Solace::uint16				hops{1};				/// Network hops limit.
	Solace::int32				ttl{3};					/// TTL of the broadcast. Broadcast stops when this counter is 0
	Solace::uint32				timeMs{2*1000};			/// Time in ms between each braodcast
};


bool operator== (BroadcastGroup const& lhs, BroadcastGroup const& rhs) noexcept;

/// Broadcast model
struct BroadcastModel {
	std::unordered_map<Address, BroadcastGroup>	groups;
//	std::unordered_set<BroadcastGroup>	groups;
};



// UDP Broadcast management

/// Join UDP broadcast address
struct JoinBroadcastGroup	{ Address	groupAddress; };

/// Leave UDP broadcast address
struct LeaveBroadcastGroup	{ Address	groupAddress; };

/// Enqueue a number of broadcast rounds
struct Broadcast			{ Address	groupAddress; Solace::uint32 rounds{1}; };

/// Update number of broadcast round requested +/- number
struct UpdateBroadcastRounds{ Address	groupAddress; Solace::int32 rounds{1}; };


/// Action to update broadcast model
using BroadcastAction = std::variant<JoinBroadcastGroup,
									LeaveBroadcastGroup,
									Broadcast,
									UpdateBroadcastRounds
							>;

/// Update broadcast model by applying an action. Note: pure function.
BroadcastModel
update(BroadcastModel const& state, BroadcastAction&& action);

}  // namespace tribe

//namespace std {
//	size_t hash<tribe::BroadcastGroup>::operator()(tribe::BroadcastGroup const& value) const noexcept {
//		return hash<tribe::Address>()(value.address);
//	}
//}  // namespace std

#endif  // TRIBE_BROADCASTMODEL_HPP
