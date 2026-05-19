#pragma once

#include <MotArda/Ecs.hpp>
#include <MotArda/ObjItem.hpp>

#include <cstdint>
#include <map>
#include <vector>

namespace MTRD {
	class SimplePacketReceiver {
	public:
		SimplePacketReceiver();
		SimplePacketReceiver(
			std::vector<std::shared_ptr<MTRD::ObjItem>>* objItemListPtr,
			ECSManager* ecsPtr,
			size_t localPlayerEntity
			);
		void OnReceivePacket(uint32_t senderID, const void* data, size_t size);

		std::map<uint32_t, size_t> remoteEntities;
		std::vector<std::shared_ptr<MTRD::ObjItem>>* objItemListPtr;
		ECSManager* ecsPtr;
		size_t localPlayerEntity;
	};
}