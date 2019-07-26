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
#ifndef TRIBE_OSTREAM_HPP
#define TRIBE_OSTREAM_HPP

#include "nodeInfo.hpp"
#include "model.hpp"

#include <ostream>

namespace tribe {

std::ostream& operator<< (std::ostream& ostr, Address const& address);
std::ostream& operator<< (std::ostream& ostr, NodeID const& nodeId);
std::ostream& operator<< (std::ostream& ostr, NodeInfo const& node);
std::ostream& operator<< (std::ostream& ostr, Peer const& peer);

}  // namespace tribe
#endif  // TRIBE_OSTREAM_HPP
