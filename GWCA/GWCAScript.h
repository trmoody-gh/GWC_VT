#pragma once

namespace GWCA {
	class ApiMgr;

	class Script {
	public:

		virtual ~Script() {}

		virtual bool __fastcall OnInit() { return true; }
		virtual void __fastcall OnMainloop() { return; }
		virtual void __fastcall OnDestruct() { return; }

		bool initialized() const { return isInitialized_; }
		void set_initialized(bool initialized) { isInitialized_ = initialized; }
	private:
		bool isInitialized_ = false;
	};
}