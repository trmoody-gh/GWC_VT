#include "GWCAModule.h"

#include "GWCA.h"

GWCA::GWCABaseModule::GWCABaseModule() {
	GWCA::Api::managers.push_back(this);
}
