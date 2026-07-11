#include "pch.h"
#include "PvZ2/Zombie_TombRaiser.h"
#include "PvZ2/Zombie.h"

// Original function pointers for grave spawning
void (*oTombRaiserSpawnGrave)(Zombie* zombie) = nullptr;

// Original function pointer for projectile throwing
void (*oTombRaiserThrowProjectile)(Zombie* zombie, int* gridPos) = nullptr;

/**
 * Hook for ZombieTombRaiser grave spawning (offset 0xB22EF8)
 * Allows customization of grave type via GraveType property
 */
void hkZombieTombRaiserSpawnGrave(Zombie* zombie)
{
	if (!zombie) {
		return;
	}

	LOGI("TombRaiser spawning grave");
	
	// Call original grave spawning logic
	if (oTombRaiserSpawnGrave) {
		oTombRaiserSpawnGrave(zombie);
	}
}

/**
 * Hook for ZombieTombRaiser projectile throwing (offset 0xB23EB4)
 * Allows customization via ProjectileType property
 */
void hkZombieTombRaiserThrowProjectile(Zombie* zombie, int* gridPos)
{
	if (!zombie || !gridPos) {
		return;
	}

	LOGI("TombRaiser throwing projectile at grid (%d, %d)", gridPos[0], gridPos[1]);

	// Call original projectile logic
	if (oTombRaiserThrowProjectile) {
		oTombRaiserThrowProjectile(zombie, gridPos);
	}
}

/**
 * Initialize the TombRaiser hook
 * This function is called during mod initialization to set up all hooks
 */
void ZombieTombRaiserProps::modInit()
{
	LOGI("Initializing ZombieTombRaiserProps for ARM64 Release");

	// Hook the grave spawning function (offset 0xB22EF8 from IDA)
	// This is called when the TombRaiser rises from the grave
	PVZ2HookFunction(0xB22EF8, (void*)hkZombieTombRaiserSpawnGrave, (void**)&oTombRaiserSpawnGrave);

	// Hook the projectile throwing function (offset 0xB23EB4 from IDA)
	// This is called when the TombRaiser throws a bone projectile
	PVZ2HookFunction(0xB23EB4, (void*)hkZombieTombRaiserThrowProjectile, (void**)&oTombRaiserThrowProjectile);

	LOGI("ZombieTombRaiserProps initialized successfully");
}
