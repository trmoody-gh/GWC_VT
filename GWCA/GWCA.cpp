#include "GWCA.h"

#include "MemoryMgr.h"

#include "GWCAModule.h"
#include "PatternScanner.h"
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
		call GWCA::ApiMgr::mainLoop
		pop edx
		pop ecx
		pop eax
		jmp trampoline
	}
}

bool GWCA::ApiMgr::Initialize() {
	if (MemoryMgr::Scan()) {
		mgr_instance = new ApiMgr();

		trampoline = (void*)hk_gameloop.Detour(MemoryMgr::GameLoopLocation, (BYTE*)mainloopIntermediary, 6);

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
	return *inst;
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
