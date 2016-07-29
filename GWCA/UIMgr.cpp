#include "UIMgr.h"

#include <future>
#include "GameThreadMgr.h"
#include "MemoryMgr.h"
#include "PatternScanner.h"


GWCA::UIMgr::UIMgr() {
	PatternScanner scan("Gw.exe");

	getString_ = (GetString_t)scan.FindPattern("\x8D\x7C\x46\x02\x8B\xCE\x6A\x01", "xxxxxxxx", -0x88);
	performAction_ = (PerformAction_t)MemoryMgr::ActionFunction;
}

wchar_t* GWCA::UIMgr::GetString(void* stringInfo) {
	auto future = GetStringAsync(stringInfo);
	future.wait();
	return future.get();
}

std::future<wchar_t*> GWCA::UIMgr::GetStringAsync(void* stringInfo) {
	std::promise<wchar_t*> promise;
	auto future = promise.get_future();

	GameThreadMgr::Instance().Enqueue([&]() {
		Callback_t callback = [](wchar_t* string, std::promise<wchar_t*>* arg) {
			(*arg).set_value(string);
		};
		getString_(stringInfo, callback, &promise);
	});

	return future;
}

void GWCA::UIMgr::DisplayAllies(bool display) {
	GameThreadMgr::Instance().Enqueue(
		performActionIntermediary,
		0x89,
		display ? 0x18 : 0x1A);
}

void GWCA::UIMgr::DisplayEnemies(bool display) {
	GameThreadMgr::Instance().Enqueue(
		performActionIntermediary,
		0x94,
		display ? 0x18 : 0x1A);
}

void GWCA::UIMgr::performActionIntermediary(DWORD action, DWORD flag) {
	DWORD base = MemoryMgr::ReadPtrChain<DWORD>((DWORD)MemoryMgr::ActionBase, 4, 0x170, 0x20, 0, 0);
	DWORD actionStruct[3] = { action, 0, 0 };

	UIMgr::Instance().performAction_(base, actionStruct, flag, actionStruct, 0);
}