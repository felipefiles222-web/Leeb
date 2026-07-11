#pragma once
#include "ZombiePropertySheet.h"
#include "Reflection/Reflection.h"

class ZombieTombRaiserProps : public ZombiePropertySheet
{
public:
	// Main customizable grave type property
	SexyString GraveType = "gravestone_egypt";
	
	// Additional properties for extensibility
	SexyString ProjectileType = "TombraiserProjectile";
	float GraveSpawnOffsetX = 0.0f;
	float GraveSpawnOffsetY = 0.0f;
	int GraveHealth = 100;

	static Reflection::CRefManualSymbolBuilder::BuildSymbolsFunc oTombRaiserPropsBuildSymbols;
	static void modInit();

	static void* construct()
	{
		auto* props = new ZombieTombRaiserProps();

		typedef void* (*ctorWithThisPtr)(void*);
		ctorWithThisPtr baseCtor = (ctorWithThisPtr)getActualOffset(0x163A068);
		baseCtor(props);

		*reinterpret_cast<int*>(uintptr_t(props)) = getActualOffset(0x23E1128);
		return props;
	}

	static void buildSymbols(Reflection::CRefManualSymbolBuilder* builder, Reflection::RClass* rclass)
	{
		oTombRaiserPropsBuildSymbols(builder, rclass);
		REGISTER_STRING_PROPERTY(builder, rclass, ZombieTombRaiserProps, GraveType);
		REGISTER_STRING_PROPERTY(builder, rclass, ZombieTombRaiserProps, ProjectileType);
		REGISTER_STANDARD_PROPERTY(builder, rclass, ZombieTombRaiserProps, GraveSpawnOffsetX);
		REGISTER_STANDARD_PROPERTY(builder, rclass, ZombieTombRaiserProps, GraveSpawnOffsetY);
		REGISTER_STANDARD_PROPERTY(builder, rclass, ZombieTombRaiserProps, GraveHealth);
	}
};
