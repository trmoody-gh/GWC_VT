#include "GWCAModule.h"

#include "GWCA.h"

GWCA::BaseModule::BaseModule() {
	GWCA::ApiMgr::managers.push_back(this);
}
