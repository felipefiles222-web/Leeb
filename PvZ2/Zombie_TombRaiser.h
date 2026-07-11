#pragma once
#include "PvZ2/Zombie.h"

class ZombieTombRaiserProps
{
public:
	// Main customizable grave type property
	SexyString GraveType = "gravestone_egypt";
	
	// Additional properties for extensibility
	SexyString ProjectileType = "TombraiserProjectile";
	float GraveSpawnOffsetX = 0.0f;
	float GraveSpawnOffsetY = 0.0f;
	int GraveHealth = 100;

	static void modInit();
};
