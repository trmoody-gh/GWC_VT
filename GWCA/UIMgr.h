#pragma once

#include <future>
#include "GWCAManager.h"
#include "Hooker.h"

namespace GWCA {

	class UIMgr : public GWCAManager<UIMgr> {
		friend class GWCAManager<UIMgr>;

	public:
		wchar_t* GetString(void* stringInfo);
		std::future<wchar_t*> GetStringAsync(void* stringInfo);
		void DisplayAllies(bool display);
		void DisplayEnemies(bool display);
	private:
		UIMgr();

		typedef void(__fastcall *Callback_t)(wchar_t* string, std::promise<wchar_t*>* arg);
		typedef void(__fastcall *GetString_t)(void* stringInfo, Callback_t callback, std::promise<wchar_t*>* arg);
		typedef void(__fastcall *PerformAction_t)(DWORD actionBaseStuff, DWORD* actionPtr, DWORD flag, DWORD* actionPtr2, DWORD unk);

		GetString_t getString_;
		PerformAction_t performAction_;

		static void performActionIntermediary(DWORD action, DWORD flag);
	};

}