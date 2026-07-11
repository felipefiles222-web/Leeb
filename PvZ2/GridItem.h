#pragma once
#include "BoardEntity.h"
#include "PropertySheetBase.h"
#include "StringRestrictionSet.h"
#include "ObjectTypeDescriptor.h"
#include "Board.h"

class GridItemType;
class GridItemPropertySheet;

class GridItemPropertySheet : public PropertySheetBase
{
public:
	float Hitpoints = 1000.0f;
	float HitRectOffsetX;
	float HitRectOffsetWidth;
	BoardEntityHeight Height = BoardEntityHeight::normal;
	bool CanBeMowed;
	PlantingRestrictionSet PlantingRestrictions;
	GridItemRestrictionSet GridItemsOverride;
};

static_assert(sizeof(GridItemPropertySheet) == 152);
static_assert(offsetof(GridItemPropertySheet, Hitpoints) == 40);
static_assert(offsetof(GridItemPropertySheet, CanBeMowed) == 56);
static_assert(offsetof(GridItemPropertySheet, PlantingRestrictions) == 64);
static_assert(offsetof(GridItemPropertySheet, GridItemsOverride) == 112);

class GridItemType : public ObjectTypeDescriptor
{
public:
	SexyString GridItemClass;
	std::vector<SexyString> ResourceGroups;
	std::vector<SexyString> AudioGroups;
	Sexy::RtWeakPtr<GridItemPropertySheet> Properties;
	char m_pad[8];
};

static_assert(sizeof(GridItemType) == 120);
static_assert(offsetof(GridItemType, GridItemClass) == 32);

class GridItem : public BoardEntity
{
public:
	char m_pad5[144];
	Sexy::RtWeakPtr<GridItemType> m_type;
	float m_health;
	float m_healthMax;
	Sexy::Point m_gridLocation;
	Sexy::RtWeakPtr<GridItemPropertySheet> m_propertySheet;

	// OJO: offset TODO -- necesitamos encontrar el equivalente ARM64 real en IDA/Ghidra
	// (el 0x716690 de Blazey es de su build ARM32). Mientras tanto, FUN_00ba2550 sigue
	// siendo nuestro camino confirmado y funcional para crear GridItems.
	static GridItem* SpawnGridItemAt(const SexyString& gridItemName, int gridPosX, int gridPosY)
	{
		return CallFunc<GridItem*, Board*, SexyString, int, int>(0 /* TODO: offset ARM64 real */, Board::GetBoard(), gridItemName, gridPosX, gridPosY);
	}
};

static_assert(sizeof(GridItem) == 304);
static_assert(offsetof(GridItem, m_pad5) == 128);
static_assert(offsetof(GridItem, m_type) == 272);
static_assert(offsetof(GridItem, m_gridLocation) == 288);
