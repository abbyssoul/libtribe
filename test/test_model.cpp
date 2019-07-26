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
 *	@file test/test_model.cpp
 *	@brief		Test suit for tribe::model
 ******************************************************************************/
#include "tribe/model.hpp"    // Class being tested.

#include <gtest/gtest.h>


using namespace tribe;


TEST(Model, initialModel) {
	auto model = PeersModel{};

	ASSERT_TRUE(model.seeds.empty());
	ASSERT_TRUE(model.members.empty());
}


TEST(Model, addSeed) {
	auto model = update(PeersModel{}, AddSeed{anyAddress(0), 1});
	ASSERT_EQ(1, model.seeds.size());
	ASSERT_TRUE(model.members.empty());
}


TEST(Model, ForgetSeed) {
	auto maybeTestAddress = tryParseAddress("10.1.2.3:7654");
	ASSERT_TRUE(maybeTestAddress.isOk());
	auto const address = *maybeTestAddress;

	auto model = update(PeersModel{}, AddSeed{address, 1});
	ASSERT_EQ(1, model.seeds.size());

	auto model2 = update(model, ForgetSeed{address});
	ASSERT_EQ(0, model2.seeds.size());
}


TEST(Model, ForgetSeed_nonExistent) {
	auto maybeTestAddress = tryParseAddress("10.1.2.3:7654");
	ASSERT_TRUE(maybeTestAddress.isOk());
	auto const address = *maybeTestAddress;

	auto model = update(PeersModel{}, AddSeed{address, 1});
	ASSERT_EQ(1, model.seeds.size());

	auto model2 = update(model, ForgetSeed{anyAddress(0)});
	ASSERT_EQ(1, model2.seeds.size());
}



TEST(Model, AddPeer) {
	auto model = update(PeersModel{}, AddPeer{anyAddress(0), {{1}, 0}, 1});
	ASSERT_EQ(1, model.members.size());
}

TEST(Model, ForgetPeer) {
	auto maybeTestAddress = tryParseAddress("10.1.2.3:7654");
	ASSERT_TRUE(maybeTestAddress.isOk());
	auto const address = *maybeTestAddress;

	auto model = update(PeersModel{}, AddPeer{address, {{1}, 0}, 1});
	ASSERT_EQ(1, model.members.size());

	auto model2 = update(model, ForgetPeer{1});
	ASSERT_EQ(0, model2.members.size());
}

TEST(Model, ForgetPeer_nonExistent) {
	auto maybeTestAddress = tryParseAddress("10.1.2.3:7654");
	ASSERT_TRUE(maybeTestAddress.isOk());
	auto const address = *maybeTestAddress;

	auto model = update(PeersModel{}, AddPeer{address, {{1}, 0}, 1});
	ASSERT_EQ(1, model.members.size());

	auto model2 = update(model, ForgetPeer{32});
	ASSERT_EQ(1, model2.members.size());
}


TEST(Model, UpdatePeerAddress) {
	auto maybeTestAddress = tryParseAddress("10.1.2.3:7654");
	ASSERT_TRUE(maybeTestAddress.isOk());
	auto const address = *maybeTestAddress;

	auto model = update(PeersModel{}, AddPeer{anyAddress(321), {{1}, 0}, 1});
	ASSERT_EQ(1, model.members.size());
	ASSERT_EQ(std::find_if(model.members.begin(), model.members.end(),
						   [&address](std::pair<NodeID, Peer> const& p){ return p.second.address == address; }),
			model.members.end());

	auto model2 = update(model, UpdatePeerAddress{{1}, address});
	ASSERT_EQ(1, model2.members.size());
	ASSERT_NE(std::find_if(model2.members.begin(), model2.members.end(),
						   [&address](std::pair<NodeID, Peer> const& p){ return p.second.address == address; }),
			model2.members.end());
}


TEST(Model, UpdatePeerAddress_nonExistent) {
	auto maybeTestAddress = tryParseAddress("10.1.2.3:7654");
	ASSERT_TRUE(maybeTestAddress.isOk());
	auto const address = *maybeTestAddress;

	auto model = update(PeersModel{}, AddPeer{anyAddress(321), {{1}, 0}, 1});
	ASSERT_EQ(1, model.members.size());
	ASSERT_EQ(std::find_if(model.members.begin(), model.members.end(),
						   [&address](std::pair<NodeID, Peer> const& p){ return p.second.address == address; }),
			model.members.end());

	auto model2 = update(model, UpdatePeerAddress{{7127}, address});
	ASSERT_EQ(1, model2.members.size());
	ASSERT_EQ(std::find_if(model2.members.begin(), model2.members.end(),
						   [&address](std::pair<NodeID, Peer> const& p){ return p.second.address == address; }),
			model2.members.end());
}

TEST(Model, UpdatePeerGeneration) {
	auto model = update(PeersModel{}, AddPeer{anyAddress(321), {{1}, 0}, 1});
	ASSERT_EQ(1, model.members.size());

	auto it = model.members.find({1});
	ASSERT_NE(it, model.members.end());
	ASSERT_EQ(it->second.generation, 0);


	auto model2 = update(model, UpdatePeerGeneration{{1}, 8, 1});
	ASSERT_EQ(1, model2.members.size());

	auto it2 = model2.members.find({1});
	ASSERT_NE(it2, model2.members.end());
	ASSERT_EQ(it2->second.generation, 8);
}

TEST(Model, UpdatePeerGeneration_lower) {
	auto model = update(PeersModel{}, AddPeer{anyAddress(321), {{1}, 12}, 1});
	auto model2 = update(model, UpdatePeerGeneration{{1}, 8, 1});
	ASSERT_EQ(1, model2.members.size());

	auto it = model2.members.find({1});
	ASSERT_NE(it, model2.members.end());
	ASSERT_NE(it->second.generation, 8);
	ASSERT_EQ(it->second.generation, 12);
}


TEST(Model, UpdatePeerGeneration_nonExistent) {
	auto model = update(PeersModel{}, AddPeer{anyAddress(321), {{1}, 0}, 1});
	auto model2 = update(model, UpdatePeerGeneration{{72}, 4, 1});
	ASSERT_EQ(1, model2.members.size());

	ASSERT_EQ(model2.members.find({72}), model2.members.end());

	auto it = model2.members.find({1});
	ASSERT_NE(it, model2.members.end());
	ASSERT_EQ(it->second.generation, 0);
}



TEST(Model, PronouncePeerDead) {
	auto initialModel = update(PeersModel{}, AddPeer{anyAddress(321), {{1}, 0}, 1});

	{  // Check initial state of the peer
		auto it = initialModel.members.find({1});
		ASSERT_NE(it, initialModel.members.end());
		ASSERT_EQ(it->second.liveness.state, Peer::State::Alive);
	}


	auto model = update(initialModel, PronouncePeerDead{{1}});
	ASSERT_EQ(1, model.members.size());

	{
		auto it = model.members.find({1});
		ASSERT_NE(it, model.members.end());
		ASSERT_EQ(it->second.liveness.state, Peer::State::Dead);
	}
}

TEST(Model, PronouncePeerDead_nonExistent) {
	auto initialModel = update(PeersModel{}, AddPeer{anyAddress(321), {{1}, 0}, 1});
	auto model = update(initialModel, PronouncePeerDead{{32}});
	ASSERT_EQ(1, model.members.size());
	ASSERT_EQ(model.members.find({32}), model.members.end());

	{
		auto it = model.members.find({1});
		ASSERT_NE(it, model.members.end());
		ASSERT_EQ(it->second.liveness.state, Peer::State::Alive);
	}
}


TEST(Model, DecayPeerInfo) {
//	auto initialModel = update(PeersModel{}, AddPeer{anyAddress(321), {{1}, 0}, 1});

//	{  // Check initial state of the peer
//		auto it = initialModel.members.find({1});
//		ASSERT_NE(it, initialModel.members.end());
//		ASSERT_EQ(it->second.liveness.state, Peer::State::Alive);
//	}


//	auto model = update(initialModel, PronouncePeerDead{{1}});
//	ASSERT_EQ(1, model.members.size());

//	{
//		auto it = model.members.find({1});
//		ASSERT_NE(it, model.members.end());
//		ASSERT_EQ(it->second.liveness.state, Peer::State::Dead);
//	}
}
