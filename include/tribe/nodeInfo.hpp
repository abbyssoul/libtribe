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
#ifndef TRIBE_NODE_INFO_HPP
#define TRIBE_NODE_INFO_HPP

#include "networkAddress.hpp"

#include <solace/types.hpp>


namespace tribe {

/**
 * @brief Unique identifier of a node in a group / cluster.
 * Note: The Id may not be unique across multiple clusters.
 */
struct NodeID {
	Solace::uint32 value;
};

struct GroupID {
	Solace::uint32 value;
};


inline bool operator== (NodeID const& lhs, NodeID const& rhs) noexcept {
	return (lhs.value == rhs.value);
}

inline bool operator!= (NodeID const& lhs, NodeID const& rhs) noexcept {
	return (lhs.value != rhs.value);
}

inline bool operator< (NodeID const& lhs, NodeID const& rhs) noexcept {
	return (lhs.value < rhs.value);
}


inline bool operator== (GroupID const& lhs, GroupID const& rhs) noexcept {
	return (lhs.value == rhs.value);
}

inline bool operator!= (GroupID const& lhs, GroupID const& rhs) noexcept {
	return (lhs.value != rhs.value);
}


/**
 * Node is a member of a cluster that provides services to the cluster.
 * NodeInfo represents info about any cluster participant, including self.
 */
struct NodeInfo {
	NodeID          id;         //!< Id of the node in the cluster.
	Solace::uint32  gen;        //!< Generation of the node / individual node 'token'
};


}  // namespace tribe

namespace std {
template <>
struct hash<tribe::NodeID> {
	size_t operator()(tribe::NodeID const& value) const noexcept {
		return std::hash<decltype (tribe::NodeID::value)>{}(value.value);
	}
};
}

#endif  // TRIBE_NODE_INFO_HPP
