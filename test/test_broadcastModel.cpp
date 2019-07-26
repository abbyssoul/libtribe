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
 *	@file test/test_broadcastModel.cpp
 *	@brief		Test suit for tribe::broadcastModel
 ******************************************************************************/
#include "tribe/broadcastModel.hpp"  // Class being tested.

#include <gtest/gtest.h>


using namespace tribe;


TEST(TestBroadcastModel, test_initialModel) {
	auto model = BroadcastModel{};

	ASSERT_TRUE(model.groups.empty());
}


TEST(TestBroadcastModel, test_join_group) {
	auto const group1Address = anyAddress(0);
	auto model = update(BroadcastModel{}, JoinBroadcastGroup{group1Address});
	ASSERT_EQ(1, model.groups.size());
	ASSERT_NE(model.groups.find(group1Address), model.groups.end());

	auto const group2Address = anyAddress(1);
	auto model2 = update(model, JoinBroadcastGroup{group2Address});
	ASSERT_EQ(2, model2.groups.size());
	ASSERT_NE(model2.groups.find(group2Address), model2.groups.end());
}

TEST(TestBroadcastModel, test_LeaveBroadcastGroup) {
	auto maybeTestAddress = tryParseAddress("10.1.2.3:7654");
	ASSERT_TRUE(maybeTestAddress.isOk());
	auto const groupAddress = *maybeTestAddress;

	auto model = update(BroadcastModel{}, JoinBroadcastGroup{groupAddress});
	ASSERT_EQ(1, model.groups.size());
	ASSERT_NE(model.groups.find(groupAddress), model.groups.end());

	auto model2 = update(model, LeaveBroadcastGroup{groupAddress});
	ASSERT_EQ(0, model2.groups.size());
	ASSERT_EQ(model2.groups.find(groupAddress), model2.groups.end());
}


TEST(TestBroadcastModel, test_LeaveBroadcastGroup_nonExistent) {
	auto maybeTestAddress = tryParseAddress("10.1.2.3:7654");
	ASSERT_TRUE(maybeTestAddress.isOk());
	auto const groupAddress = *maybeTestAddress;

	auto model = update(BroadcastModel{}, JoinBroadcastGroup{groupAddress});
	ASSERT_EQ(1, model.groups.size());
	ASSERT_NE(model.groups.find(groupAddress), model.groups.end());

	auto const someAddress = anyAddress(0);
	auto model2 = update(model, LeaveBroadcastGroup{someAddress});
	ASSERT_EQ(1, model2.groups.size());
	ASSERT_EQ(model2.groups.find(someAddress), model2.groups.end());
	ASSERT_NE(model2.groups.find(groupAddress), model2.groups.end());
}
