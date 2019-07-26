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
#include "encoder.hpp"


using namespace Solace;

namespace tribe {

Gossip::size_type
Encoder::protocolSize(uint8 const& value) noexcept {
	return sizeof(value);
}

Gossip::size_type
Encoder::protocolSize(uint16 const& value) noexcept {
	return sizeof(value);
}

Gossip::size_type
Encoder::protocolSize(uint32 const& value) noexcept {
	return sizeof(value);
}

Gossip::size_type
Encoder::protocolSize(uint64 const& value) noexcept {
	return sizeof(value);
}

Gossip::size_type
Encoder::protocolSize(StringView const& str) noexcept {
	return sizeof(Gossip::size_type) +         // Space for string var size
			str.size();             // Space for the actual string bytes
}


Gossip::size_type
Encoder::protocolSize(NodeID id) noexcept {
	return protocolSize(id.value);
}


Gossip::size_type
Encoder::protocolSize(Address const& address) noexcept {
	return 0;  // FIXME: Implementation required
}

Gossip::size_type
Encoder::protocolSize(NodeInfo const& node) noexcept {
	return protocolSize(node.id) +	 // Proposed fid for authentication mechanism
			protocolSize(node.gen);  // Root name where we want to attach to
}


Encoder&
operator<< (Encoder& out, NodeID id) {
	return out << id.value;
}


Encoder&
operator<< (Encoder& out, Address const& addr) {
	return out;   // FIXME: Implementation required
}


Encoder&
operator<< (Encoder& out, NodeInfo const& node) {
	return out << node.id
			   << node.gen;
}


}  // namespace tribe
