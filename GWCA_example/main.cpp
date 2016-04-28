#include <Windows.h>

// Include main API
#include "../GWCA/GWCA.h"

// Include used managers
#include "../GWCA/StoCMgr.h"
#include "../GWCA/ChatMgr.h"
#include "../GWCA/SkillbarMgr.h"

// Implementation of zrank packet, check StoCPackets.h for implementation first
struct P147_UpdateGenericValue : public GWCA::StoC_Pak::Packet<P147_UpdateGenericValue> {
	DWORD type;
	DWORD AgentID;
	DWORD value;
};
const DWORD GWCA::StoC_Pak::Packet<P147_UpdateGenericValue>::STATIC_HEADER = 147;

// An example of a gwca script.
// Will turn all "zranks" into z12 clientside.
class z12ForEveryoneScript : public GWCA::Script {

	// Due to the nature of stocmgr hooks, all we need is OnInit, do not implement what you dont need to.
	bool OnInit() override {

		// Add event to stocmgr, can use a lambda function, or a normal ctype function
		GWCA::StoCMgr::Instance().AddGameServerEvent<P147_UpdateGenericValue>(
			[](P147_UpdateGenericValue* pak) -> bool {
			if (pak->type == 27) {
				pak->value = 12;
			}
			return false;
		}

		);

		return true;
	}

};

// Another script definition, will say HAI WERLD in local chat when you press the INSERT key.
// Will use skill 1 on the page down key.
class HaiWerldScript : public GWCA::Script {

	// Mainloop hook, remember to only write ONE FULL ITERATION OF YOUR LOOP. DO NOT WHILE 1
	void OnMainloop() override {

		if (GetAsyncKeyState(VK_INSERT) & 1)
			GWCA::ChatMgr::Instance().SendChat(L"HAI WERLD", L'!');

		if (GetAsyncKeyState(VK_PRIOR) & 1)
			GWCA::SkillbarMgr::Instance().UseSkill(1);

	}

};


// DLL entry point, do not loop/wait in this thread, the loadlibrary function has to return
BOOL WINAPI DllMain(_In_ HMODULE _HDllHandle, _In_ DWORD _Reason, _In_opt_ LPVOID _Reserved) {
	if (_Reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(_HDllHandle);

		// Creating a console for use in this test project.
		AllocConsole();
		FILE* fh;
		SetConsoleTitleA("GWCA++ Test Project");
		freopen_s(&fh, "CONOUT$", "w", stdout);
		
		// Initialize the GWCA api. ALWAYS DO THIS FIRST BEFORE USING ANY OF THE API FEATURES
		if (!GWCA::ApiMgr::Initialize()) {
			printf("Api initialization FAILED");

			// Something went wrong, tell LoadLibrary to unload this dll.
			return FALSE;
		}

		// Add scripts to the api manager.

		GWCA::ApiMgr::Instance() += new z12ForEveryoneScript();
		GWCA::ApiMgr::Instance() += new HaiWerldScript();

	}
	else if (_Reason == DLL_PROCESS_DETACH) {
		
		GWCA::ApiMgr::Destruct();

	}
	// Everything was done sucessfully :) say its okay to leave the dll in memory.
	return TRUE;
}
