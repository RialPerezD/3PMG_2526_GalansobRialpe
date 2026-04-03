#pragma once
#include <cstdint>
#include <map>
#include <vector>
#include "Ecs.hpp"
#include "ObjItem.hpp"

namespace MTRD {
	class SimplePacketReciver {
	public:
		SimplePacketReciver();
		SimplePacketReciver(
			std::vector<MTRD::ObjItem>* objItemListPtr,
			ECSManager* ecsPtr,
			size_t localPlayerEntity
			);
		void OnReceivePacket(uint32_t senderID, const void* data, size_t size);

		std::map<uint32_t, size_t> remoteEntities;
		std::vector<MTRD::ObjItem>* objItemListPtr;
		ECSManager* ecsPtr;
		size_t localPlayerEntity;
	};
}