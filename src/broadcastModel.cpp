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
#include "tribe/broadcastModel.hpp"

#include <functional>


using namespace Solace;
using namespace tribe;

namespace /* anonymous */ {

BroadcastModel addGroup(BroadcastModel model, JoinBroadcastGroup const& group) {
	model.groups.emplace(group.groupAddress, BroadcastGroup{});

	return model;
}


BroadcastModel dropGroup(BroadcastModel model, LeaveBroadcastGroup const& group) {
	model.groups.erase(group.groupAddress);

//	model.groups.erase(
//				std::remove_if(model.groups.begin(), model.groups.end(), [&group](BroadcastGroup const& g) { return g.address == group.groupAddress;}),
//				model.groups.end());

	return model;
}


BroadcastModel
broadcast(BroadcastModel model, Broadcast const& action) {
	for (auto& group : model.groups) {
		if (group.first == action.groupAddress) {
			if (group.second.ttl > 0) {
				group.second.ttl -= action.rounds;;
//				auto groupNode = model.groups.extract(group);
//				groupNode.value().ttl -= action.rounds;
//				model.groups.insert(std::move(groupNode));
//				break;
			}
		}
	}

//	auto it = std::find_if(model.groups.begin(), model.groups.end(), [&action](BroadcastGroup const& g) { return g.address == action.groupAddress; });
//	if (it == model.groups.end())
//		return model;


	return model;
}


BroadcastModel
updateRounds(BroadcastModel model, UpdateBroadcastRounds const& action) {
	for (auto& group : model.groups) {
		if (group.first == action.groupAddress) {
			group.second.ttl = action.rounds;
//				auto groupNode = model.groups.extract(group);
//				model.groups.insert(std::move(groupNode));
//				break;
		}
	}

	return model;
}

}  // anonymous namespace

BroadcastModel
tribe::update(BroadcastModel const& state, BroadcastAction&& action) {

	struct ActionHandler {
		BroadcastModel const& state;

		BroadcastModel operator() (JoinBroadcastGroup&& action) const { return addGroup(state, std::move(action)); }
		BroadcastModel operator() (LeaveBroadcastGroup&& action) const { return dropGroup(state, std::move(action)); }

		BroadcastModel operator() (Broadcast&& action) const { return broadcast(state, action); }
		BroadcastModel operator() (UpdateBroadcastRounds&& action) const { return updateRounds(state, action); }
	};

	return std::visit(ActionHandler{state}, std::move(action));

}
