#pragma once

#include <Windows.h>
#include "GWCAModule.h"

namespace GWCA {

	class CtoSMgr : public Module<CtoSMgr> {
		friend class Module<CtoSMgr>;

	public:

		// Send packet that uses only dword parameters, can copypaste most gwa2 sendpackets :D
		void SendPacket(DWORD size, ...);

		// Send a packet with a specific struct alignment, used for more complex packets.
		template <class T>
		void SendPacket(T* packet)
		{
			DWORD size = sizeof(T);
			gs_send_function_(
				MemoryMgr::GetGSObject(), size, (DWORD*)packet);
		}

	private:		
		typedef void(__fastcall *SendCtoGSPacket_t)(DWORD ctogsobj, DWORD size, DWORD* packet);

		static SendCtoGSPacket_t gs_send_function_;

		CtoSMgr();
		void OnDestruct() override {}
	};

}