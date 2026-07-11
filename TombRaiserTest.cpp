#include "PvZ2/Zombie_TombRaiser.h"
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <unistd.h>
#include "memUtils.h"
#include "MummyMemoryFix.h"
#include "SexyTypes.h"
#include "Logging.h"

__attribute__((constructor))
void libPVZ2ExpansionMod_main()
{
	LOGI("Initializing TombRaiser");
	
	ZombieTombRaiserProps::modInit();
	
	LOGI("Finished initializing");
}
