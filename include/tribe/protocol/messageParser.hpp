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
#ifndef TRIBE_PROTOCOL_MESSAGEPARSER_HPP
#define TRIBE_PROTOCOL_MESSAGEPARSER_HPP

#include "gossip.hpp"

namespace tribe {

/**
 * Gossip message parser
 */
struct MessageParser {
	struct Error {};

	[[nodiscard]]
	Solace::Result<Gossip::MessageHeader, Error>
	parseMessageHeader(Solace::ByteReader& src) const;

	[[nodiscard]]
	Solace::Result<Message, Error>
	parse(Solace::ByteReader& src) const;
};

}  // namespace tribe
#endif  // TRIBE_PROTOCOL_MESSAGEPARSER_HPP
