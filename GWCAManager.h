#pragma once

#include "GWCAScript.h"
#include <vector>

namespace gw {

	class GWCAManager
	{
	private:
		GWCAManager() = delete;
		GWCAManager(GWCAManager&) = delete;
		~GWCAManager();


		std::vector<GWCAScript*> scripts_;
	public:

		static bool Initialize();
		static GWCAManager& Instance();
		static void Destruct();

		GWCAManager& operator+=(GWCAScript*);

	private:
		static GWCAManager* inst_;
		static GWCAApi* api_;

		static void __fastcall gameloop();

	};

}