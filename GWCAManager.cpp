#include "GWCAManager.h"

gw::GWCAManager* gw::GWCAManager::inst_ = nullptr;
gw::GWCAApi* gw::GWCAManager::api_ = nullptr;

gw::GWCAManager::GWCAManager()
{
}


gw::GWCAManager::~GWCAManager()
{
}

bool gw::GWCAManager::Initialize()
{



	return true;
}

gw::GWCAManager& gw::GWCAManager::Instance()
{
	return *inst_;
}

void gw::GWCAManager::Destruct()
{
}

gw::GWCAManager& gw::GWCAManager::operator+=(GWCAScript *)
{
	// TODO: insert return statement here
}

void gw::GWCAManager::gameloop()
{
}
