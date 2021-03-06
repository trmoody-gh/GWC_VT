#pragma once

#include <Windows.h>

#include <GWCA\GameContainers\gw_array.h>

namespace GW {
	struct Quest {
		DWORD questid;
		int logstate;
		void* unk1[3];
		DWORD mapfrom;
		GamePos marker;
		DWORD unk2;
		DWORD mapto;
		void* unk3[2];
	};

	using QuestLog = gw_array<Quest>;
}
