#include <GWCA\Managers\GameThreadMgr.h>

#include <GWCA\GWCA.h>
#include <GWCA\Managers\MemoryMgr.h>

CRITICAL_SECTION GW::GameThreadMgr::criticalsection_;

void __stdcall GW::GameThreadMgr::CallFunctions() {
	if (TryEnterCriticalSection(&criticalsection_)) {
		if (!calls_.empty()) {
			for (const auto& Call : calls_) {
				Call();
			}

			calls_.clear();
		}

		if (!calls_permanent_.empty()) {
			for (const auto& Call : calls_permanent_) {
				Call();
			}
		}
		LeaveCriticalSection(&criticalsection_);
	}
}

void __declspec(naked) GW::GameThreadMgr::gameLoopHook() {
	_asm PUSHAD

	GameThreadMgr::Instance().CallFunctions();

	_asm POPAD
	_asm JMP MemoryMgr::GameLoopReturn
}

void __declspec(naked) GW::GameThreadMgr::renderHook() {
	Sleep(1);
	_asm {
		POP ESI
		POP EBX
		FSTP DWORD PTR DS : [0xA3F998]
		MOV ESP, EBP
		POP EBP
		RETN
	}
}

void GW::GameThreadMgr::ToggleRenderHook() {
	static BYTE restorebuf[5];
	DWORD dwProt;

	render_state_ = !render_state_;

	if (render_state_) {
		memcpy(restorebuf, MemoryMgr::RenderLoopLocation, 5);

		VirtualProtect(MemoryMgr::RenderLoopLocation, 5, PAGE_EXECUTE_READWRITE, &dwProt);
		memset(MemoryMgr::RenderLoopLocation, 0xE9, 1);
		*(DWORD*)(MemoryMgr::RenderLoopLocation + 1) = (DWORD)((BYTE*)renderHook - MemoryMgr::RenderLoopLocation) - 5;
		VirtualProtect(MemoryMgr::RenderLoopLocation, 5, dwProt, &dwProt);
	} else {
		VirtualProtect(MemoryMgr::RenderLoopLocation, 5, PAGE_EXECUTE_READWRITE, &dwProt);
		memcpy(MemoryMgr::RenderLoopLocation, restorebuf, 5);
		VirtualProtect(MemoryMgr::RenderLoopLocation, 5, dwProt, &dwProt);
	}
}

GW::GameThreadMgr::GameThreadMgr() : render_state_(false) {
	MemoryMgr::GameLoopReturn = (BYTE*)hk_game_thread_.Detour(MemoryMgr::GameLoopLocation, (BYTE*)gameLoopHook, 5);
	InitializeCriticalSection(&criticalsection_);
}

void GW::GameThreadMgr::RestoreHooks() {
	if (render_state_) ToggleRenderHook();
	hk_game_thread_.Retour();
}
