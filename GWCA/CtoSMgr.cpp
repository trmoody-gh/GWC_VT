#include "CtoSMgr.h"

#include "MemoryMgr.h"

GWCA::CtoSMgr::SendCtoGSPacket_t GWCA::CtoSMgr::gs_send_function_ = NULL;

GWCA::CtoSMgr::CtoSMgr() {
	gs_send_function_ = (SendCtoGSPacket_t)MemoryMgr::CtoGSSendFunction;
}

void GWCA::CtoSMgr::SendPacket(DWORD size, ...) {
	DWORD* pak = new DWORD[size / 4];;

	va_list vl;

	va_start(vl, size);
	for (DWORD i = 0; i < size / 4; i++)
	{
		pak[i] = va_arg(vl, DWORD);
	}
	va_end(vl);

	gs_send_function_(MemoryMgr::GetGSObject(), size, pak);
}
