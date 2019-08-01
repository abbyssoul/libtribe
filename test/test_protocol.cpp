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
#include "tribe/protocol/messageWriter.hpp"

#include <gtest/gtest.h>

#include <solace/result.hpp>
#include <solace/posixErrorDomain.hpp>  // test makeError for redirect reason


using namespace tribe;
using namespace Solace;


TEST(TestProtocol, test_parser) {
	auto parser = MessageParser{};

	byte buffer[128] = {0};
	auto reader = ByteReader{wrapMemory(buffer)};

	auto message = parser.parse(reader);
	ASSERT_FALSE(message.isOk());
}

struct TestGossipMessage : public ::testing::Test {

	struct Error{};

//	Result<void, Error> assertCondition(bool isOk) {
//		if (!isOk)
//			FAIL();

//		return Ok();
//	}

	template<typename MessageType>
	Result<MessageType, Error> expectMessage() {
		auto parser = MessageParser{};
		auto reader = ByteReader{messageWriter.writer().viewWritten()};

		auto message = parser.parse(reader);
		if (!message) {
			ADD_FAILURE() << "Failed to parse message from a buffer";
			return Err(Error{});
		}

		auto& msg = *message;
		if (!std::holds_alternative<MessageType>(msg)) {
			ADD_FAILURE() << "Unexpected message type parsed";
			return Err(Error{});
		}

		return Ok(std::get<MessageType>(std::move(msg)));
	}

protected:
	NodeInfo selfNodeInfo{{321}, 19};
	NodeInfo otherNodeInfo{{7177}, 7};

	byte buffer[128] = {0};
	ByteWriter writer{wrapMemory(buffer)};
	MessageWriter messageWriter{writer};
};


TEST_F(TestGossipMessage, ConnectRequest) {
	messageWriter.join(otherNodeInfo);

	EXPECT_TRUE(expectMessage<ConnectRequest>()
			.then([this](ConnectRequest const& request) {
				EXPECT_EQ(request.nodeInfo.id, otherNodeInfo.id);
				EXPECT_EQ(request.nodeInfo.gen, otherNodeInfo.gen);

				EXPECT_TRUE(request.token.empty());
				EXPECT_TRUE(request.auth.empty());
			}).isOk());
}



TEST_F(TestGossipMessage, ConnectResponseAck) {
	messageWriter.joinAck(selfNodeInfo);

	EXPECT_TRUE(expectMessage<ConnectResponseAck>()
			.then([this](ConnectResponseAck const& request) {
				EXPECT_EQ(request.self.id, selfNodeInfo.id);
				EXPECT_EQ(request.self.gen, selfNodeInfo.gen);
			}).isOk());
}


TEST_F(TestGossipMessage, ConnectResponseRedirect) {
	auto maybeAltAddress = tryParseAddress("10.1.1.3:12483");
	ASSERT_TRUE(maybeAltAddress.isOk());
	auto altAddress = *maybeAltAddress;

	auto reason = makeError(BasicError::Overflow, "server full");
	messageWriter.joinRedirect(reason, altAddress);

	EXPECT_TRUE(expectMessage<ConnectResponseRedirect>()
			.then([&](ConnectResponseRedirect const& request) {
				EXPECT_EQ(request.otherNode, altAddress);
				EXPECT_EQ(request.reason, reason);
			}).isOk());
}
