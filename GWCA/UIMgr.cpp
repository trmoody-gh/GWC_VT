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
	return GetStringAsync(stringInfo).get();
}

std::future<wchar_t*> GWCA::UIMgr::GetStringAsync(void* stringInfo) {
	auto task = std::packaged_task<std::future<wchar_t*>()>([=]() {
		auto promisePtr = new std::promise<wchar_t*>;
		auto future = promisePtr->get_future();

		Callback_t callback = [](std::promise<wchar_t*>* arg, wchar_t* string) {
			arg->set_value(string);
			delete arg;
		};

		getString_(stringInfo, callback, promisePtr);

		return future;
	});

	auto future = task.get_future();

	GameThreadMgr::Instance().EnqueueTask(std::move(task));

	return future.get();
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

void GWCA::UIMgr::MoveBackward(bool on) {
	GameThreadMgr::Instance().Enqueue(
		performActionIntermediary,
		0xAC,
		on ? 0x18 : 0x1A);
}