#include "GWCA.h"

#include "MemoryMgr.h"
#include "CtoSMgr.h"
#include "AgentMgr.h"
#include "PartyMgr.h"
#include "ItemMgr.h"
#include "SkillbarMgr.h"
#include "EffectMgr.h"
#include "MapMgr.h"
#include "ChatMgr.h"
#include "MerchantMgr.h"
#include "GuildMgr.h"
#include "FriendListMgr.h"
#include "StoCMgr.h"
#include "CameraMgr.h"
#include "GWCAModule.h"

#include "Hooker.h"

enum e_DestructState {
	CONTINUE,
	DESTRUCT,
	DESTRUCTED
};

std::vector<GWCA::BaseModule*> GWCA::ApiMgr::managers;
std::vector<GWCA::Script*> GWCA::ApiMgr::scripts;
GWCA::ApiMgr* GWCA::ApiMgr::mgr_instance;

GWCA::Hook hk_gameloop;
void* trampoline;

e_DestructState destruct = CONTINUE;


// Main Loop Logic

void __stdcall GWCA::ApiMgr::mainLoop() {

	if (destruct == DESTRUCT) {
		for (std::vector<GWCA::Script*>::iterator it = scripts.begin(); it < scripts.cend(); ++it)
		{
			GWCA::Script* current_script = *it;
			current_script->OnDestruct();
		}
		hk_gameloop.Retour();
		destruct = DESTRUCTED;
		return;
	}

	// For each script in scripts vector
	for (std::vector<GWCA::Script*>::iterator it = scripts.begin(); it < scripts.cend(); ++it)
	{
		GWCA::Script* current_script = *it;
		if (!current_script->initialized()) {
			current_script->OnInit();
			current_script->set_initialized(true);
			continue; // Let init take up a time slot of the current thread iteration
		}
		current_script->OnMainloop();
	}
}

void __declspec(naked) mainloopIntermediary()
{
	__asm {
		push eax
		push ecx
		push edx
		call mainLoop
		pop edx
		pop ecx
		pop eax
		jmp trampoline
	}
}

bool GWCA::ApiMgr::Initialize() {
	if (MemoryMgr::Scan()) {
		mgr_instance = new ApiMgr();

		PatternScanner scan(0x401000, 0x49a000);
		void* gameloop_addr = (void*)scan.FindPattern("\x90\x55\x8B\xEC\x83\xEC\x08\x53\x56\x8B\xD9\x57\x89\x55\xF8", "xxxxxxxxxxxxxxx", 0);
		if (!gameloop_addr) return false;

		trampoline = (void*)hk_gameloop.Detour((BYTE*)gameloop_addr, (BYTE*)mainloopIntermediary, 6);

		return true;
	} else {
		return false;
	}
}

void GWCA::ApiMgr::Destruct() {
	for (BaseModule* manager : managers) {
		manager->OnDestruct();
	}
	destruct = DESTRUCT;

	// Memory will be deallocated when the dll is unloaded from the process,
	// dont spend time doing what the operating system is going to do anyways.

	//for (BaseModule* manager : managers) {
	//		delete manager;
	//	}
}

GWCA::ApiMgr& GWCA::ApiMgr::Instance()
{
	static ApiMgr* inst = new ApiMgr();
}

void GWCA::ApiMgr::AddScript(Script * script)
{
	scripts.push_back(script);
}

GWCA::ApiMgr& GWCA::ApiMgr::operator+=(Script * script)
{
	AddScript(script);
	return *this;
}
