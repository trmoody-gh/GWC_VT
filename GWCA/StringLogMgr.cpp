#include "StringLogMgr.h"
#include "PatternScanner.h"

BYTE* GWCA::StringLogMgr::string_log_ret = NULL;
BYTE* GWCA::StringLogMgr::string_filter1_ret = NULL;
BYTE* GWCA::StringLogMgr::string_filter2_ret = NULL;

GWCA::StringLogMgr::StringLogMgr() {
	PatternScanner scan("Gw.exe");

	auto StringLogFunc = (BYTE*)scan.FindPattern("\x89\x3E\x8B\x7D\x10\x89\x5E\x04\x39\x7E\x08", "xxxxxxxxxxx", 0x22);
	string_log_ret = (BYTE*)hook_string_log.Detour(StringLogFunc, (BYTE*)StringLogMgr::detourStringLog, 5);

	auto StringFilter1 = (BYTE*)scan.FindPattern("\x51\x56\x8B\x75\x08\x57\x8B\xF9\x83\x3E\x00", "xxxxxxxxxxx", -0x3);
	string_filter1_ret = (BYTE*)hook_string_filter1.Detour(StringFilter1, (BYTE*)StringLogMgr::detourStringFilter1, 5);

	auto StringFilter2 = (BYTE*)scan.FindPattern("\x51\x53\x56\x57\x8B\xF9\x33\xD2\x8B\x4F\x2C", "xxxxxxxxxxx", -0x3);
	string_filter2_ret = (BYTE*)hook_string_filter2.Detour(StringFilter2, (BYTE*)StringLogMgr::detourStringFilter2, 5);


	auto LoadFinishedFunc = (BYTE*)scan.FindPattern("\x8B\x56\x1C\x8B\xCF\x52\xE8", "xxxxxxx", 0);
	load_finished_ret = (BYTE*)hook_load_finished.Detour(LoadFinishedFunc, (BYTE*)StringLogMgr::detourLoadFinished, 5);
}

void GWCA::StringLogMgr::RestoreHooks() {
	hook_string_log.Retour();
	hook_string_filter1.Retour();
	hook_string_filter2.Retour();
	hook_load_finished.Retour();
}


std::wstring GWCA::StringLogMgr::GetAgentName(DWORD agentID) {
	return StringLog[agentID];
}


DWORD GWCA::StringLogMgr::NextStringType = 0;

void __declspec(naked) GWCA::StringLogMgr::detourStringLog() {
	__asm {
		pushad
		mov eax, GWCA::StringLogMgr::NextStringType
		test eax, eax
		jz label_exit

		cmp eax, 1
		jnz label_filter2
		mov eax, [ebp+0x37C]
		jmp label_range_check

	label_filter2:
		cmp eax, 2
		jnz label_exit
		mov eax, [ebp+0x338]

	label_range_check:
		mov GWCA::StringLogMgr::NextStringType, 0
		cmp eax, 0
		jbe label_exit
		cmp eax, GWCA::StringLogMgr::StringLogSize
		jae label_exit

		shl eax, 8
		add eax, offset GWCA::StringLogMgr::StringLog

		xor ebx, ebx

	label_loop:
		mov dx, [ecx]
		mov [eax], dx
		add ecx, 2
		add eax, 2
		inc ebx
		cmp ebx, 0x80
		jz label_exit
		test dx, dx
		jnz label_loop

	label_exit:
		popad
		jmp GWCA::StringLogMgr::string_log_ret
	}
}

void __declspec(naked) GWCA::StringLogMgr::detourStringFilter1() {
	__asm {
		mov GWCA::StringLogMgr::NextStringType, 1
		jmp GWCA::StringLogMgr::string_filter1_ret
	}
}

void __declspec(naked) GWCA::StringLogMgr::detourStringFilter2() {
	__asm {
		mov GWCA::StringLogMgr::NextStringType, 2
		jmp GWCA::StringLogMgr::string_filter2_ret
	}
}

void __declspec(naked) GWCA::StringLogMgr::detourLoadFinished() {
	__asm {
		pushad

		xor ebx, ebx
		mov eax, offset GWCA::StringLogMgr::StringLog

		label_loop:
		mov [eax], 0
		inc ebx
		add eax, 0x100
		cmp ebx, GWCA::StringLogMgr::StringLogSize
		jnz label_loop

		popad
		jmp GWCA::StringLogMgr::load_finished_ret
	}
}