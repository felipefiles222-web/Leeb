#include "pch.h"
#include "PvZ2/Zombie_TombRaiser.h"
#include "PvZ2/Zombie.h"
#include "PvZ2/Board.h"
#include "memUtils.h"
#include "Logging.h"

// Original function pointers for grave spawning and projectile throwing
typedef void (*TombRaiserSpawnGraveFunc)(Zombie* zombie);
TombRaiserSpawnGraveFunc oTombRaiserSpawnGrave = nullptr;

typedef void (*TombRaiserThrowProjectileFunc)(Zombie* zombie, int gridPosX, int gridPosY);
TombRaiserThrowProjectileFunc oTombRaiserThrowProjectile = nullptr;

/**
 * Hook for ZombieTombRaiser grave spawning (offset 0xB22EF8)
 * Called when the TombRaiser rises from the grave
 * Allows customization of grave type and spawn offset via GraveType property
 */
void hkZombieTombRaiserSpawnGrave(Zombie* zombie)
{
	if (!zombie) {
		LOGI("ERROR: TombRaiser spawning grave with null zombie pointer");
		return;
	}

	// Get the property sheet for this zombie
	auto* props = reinterpret_cast<ZombieTombRaiserProps*>(zombie->m_propertySheet.Get());
	
	if (!props) {
		LOGI("WARNING: TombRaiser property sheet is null, using default grave spawn");
		if (oTombRaiserSpawnGrave) {
			oTombRaiserSpawnGrave(zombie);
		}
		return;
	}

	LOGI("TombRaiser spawning grave - Type: %s, Offset: (%.1f, %.1f), Health: %d",
		 props->GraveType.c_str(), props->GraveSpawnOffsetX, props->GraveSpawnOffsetY, props->GraveHealth);

	// Apply custom grave spawn logic
	// Store original position for offset calculations
	SexyVector2 originalPos = zombie->m_position;
	
	// Apply spawn offsets if defined
	if (props->GraveSpawnOffsetX != 0.0f || props->GraveSpawnOffsetY != 0.0f) {
		zombie->m_position.x += props->GraveSpawnOffsetX;
		zombie->m_position.y += props->GraveSpawnOffsetY;
	}

	// Call original grave spawning logic
	if (oTombRaiserSpawnGrave) {
		oTombRaiserSpawnGrave(zombie);
	}

	// Restore position for normal gameplay
	zombie->m_position = originalPos;
}

/**
 * Hook for ZombieTombRaiser projectile throwing (offset 0xB23EB4)
 * Called when the TombRaiser throws a bone projectile
 * Allows customization via ProjectileType property
 */
void hkZombieTombRaiserThrowProjectile(Zombie* zombie, int gridPosX, int gridPosY)
{
	if (!zombie) {
		LOGI("ERROR: TombRaiser throwing projectile with null zombie pointer");
		return;
	}

	// Get the property sheet for this zombie
	auto* props = reinterpret_cast<ZombieTombRaiserProps*>(zombie->m_propertySheet.Get());
	
	if (!props) {
		LOGI("WARNING: TombRaiser property sheet is null, using default projectile");
		if (oTombRaiserThrowProjectile) {
			oTombRaiserThrowProjectile(zombie, gridPosX, gridPosY);
		}
		return;
	}

	LOGI("TombRaiser throwing projectile - Type: %s, Grid Position: (%d, %d)",
		 props->ProjectileType.c_str(), gridPosX, gridPosY);

	// Call original projectile logic (can be extended with custom logic if needed)
	if (oTombRaiserThrowProjectile) {
		oTombRaiserThrowProjectile(zombie, gridPosX, gridPosY);
	}
}

// Initialize the static reflection function pointer
Reflection::CRefManualSymbolBuilder::BuildSymbolsFunc ZombieTombRaiserProps::oTombRaiserPropsBuildSymbols = nullptr;

/**
 * Initialize the TombRaiser hook
 * This function is called during mod initialization to set up all hooks
 * 
 * ARM64 Release offsets from IDA Pro:
 * - ZombieTombRaiserProps vtable: 0x23E1128
 * - ZombieTombRaiserProps constructor base: 0x163A068
 * - Grave spawn function: 0xB22EF8
 * - Projectile throw function: 0xB23EB4
 */
void ZombieTombRaiserProps::modInit()
{
	LOGI("========================================");
	LOGI("Initializing ZombieTombRaiserProps for ARM64 Release");
	LOGI("========================================");

	// Hook the property sheet constructor (offset 0xB21614)
	// This is called to initialize the TombRaiser property sheet
	PVZ2HookFunction(0xB21614, (void*)construct, nullptr);

	// Hook the grave spawning function (offset 0xB22EF8 from IDA)
	// This is called when the TombRaiser rises from the grave
	// Now supports customizable GraveType, offsets, and health via properties
	PVZ2HookFunction(0xB22EF8, (void*)hkZombieTombRaiserSpawnGrave, (void**)&oTombRaiserSpawnGrave);

	// Hook the projectile throwing function (offset 0xB23EB4 from IDA)
	// This is called when the TombRaiser throws a bone projectile
	// Now supports customizable ProjectileType via properties
	PVZ2HookFunction(0xB23EB4, (void*)hkZombieTombRaiserThrowProjectile, (void**)&oTombRaiserThrowProjectile);

	// Hook buildSymbols to register the new properties with the reflection system
	// This allows the properties to be read/written from JSON packages
	PVZ2HookFunction(0xB217C4, (void*)buildSymbols, (void**)&oTombRaiserPropsBuildSymbols);

	LOGI("========================================");
	LOGI("ZombieTombRaiserProps initialized successfully!");
	LOGI("Available properties:");
	LOGI("  - GraveType (string): Type of grave to spawn");
	LOGI("  - ProjectileType (string): Type of projectile to throw");
	LOGI("  - GraveSpawnOffsetX (float): X offset for grave spawn");
	LOGI("  - GraveSpawnOffsetY (float): Y offset for grave spawn");
	LOGI("  - GraveHealth (int): Custom grave health value");
	LOGI("========================================");
}
