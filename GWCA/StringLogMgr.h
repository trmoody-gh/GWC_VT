#pragma once

#include "GWCAManager.h"
#include "Hooker.h"

namespace GWCA {

	class StringLogMgr : public GWCAManager<StringLogMgr> {
		friend class GWCAManager<StringLogMgr>;

	public:
		std::wstring GetAgentName(DWORD agentID);
	private:
		StringLogMgr();
		void RestoreHooks() override;

		Hook hook_string_log;
		Hook hook_string_filter1;
		Hook hook_string_filter2;
		Hook hook_load_finished;
		static BYTE* string_log_ret;
		static BYTE* string_filter1_ret;
		static BYTE* string_filter2_ret;
		static BYTE* load_finished_ret;

		static void detourStringLog();
		static void detourStringFilter1();
		static void detourStringFilter2();
		static void detourLoadFinished();

		static DWORD NextStringType;
		static constexpr DWORD StringLogSize = 0x200;

		static wchar_t StringLog[0x100][StringLogSize];
	};

}