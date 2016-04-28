#pragma once

namespace GWCA {
	class ApiMgr;

	class Script {
	public:

		// Will be called when the DLL is unloaded, or the GWCAManager is destructed by other means
		virtual ~Script() {}

		// Called on the first gameloop iteration, once true is returned, this function will not be called again, return false if you need to retry initialization
		// Use this to install StoCMgr hooks, or hooks of your own if you know what you are doing.
		virtual bool OnInit() { return true; }

		// Called every gameloop iteration, DO NOT USE WHILE 1, code one full iteration into the function as is.
		virtual void OnMainloop() { return; }

		// Called when ApiMgr::Destruct() is called
		// Use this to restore any custom hooks you make, stocmgr hooks will be handled for you.
		virtual void OnDestruct() { return; }

		bool initialized() const { return isInitialized_; }
		void set_initialized(bool initialized) { isInitialized_ = initialized; }
	private:
		bool isInitialized_ = false;
	};
}