#pragma once
#include "PvZ2/Zombie.h"

class ZombieTombRaiserProps
{
public:
	// Main customizable grave type property
	int GraveType = 0; // 0 = default gravestone_egypt
	
	// Additional properties for extensibility
	int ProjectileType = 0; // 0 = default TombraiserProjectile
	float GraveSpawnOffsetX = 0.0f;
	float GraveSpawnOffsetY = 0.0f;
	int GraveHealth = 100;

	static void modInit();
};
