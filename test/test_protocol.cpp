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
/*******************************************************************************
 * libTribe Unit Test Suit
 *	@file test/test_protocol.cpp
 *	@brief		Test suit for tribe::model
 ******************************************************************************/
#include "tribe/protocol/messageParser.hpp"    // Class being tested.

#include <gtest/gtest.h>


using namespace tribe;
using namespace Solace;


TEST(TestProtocol, test_parser) {
	auto parser = MessageParser{};

	byte buffer[128] = {0};
	Solace::ByteReader reader{wrapMemory(buffer)};

	auto message = parser.parse(reader);
	ASSERT_FALSE(message.isOk());
}
