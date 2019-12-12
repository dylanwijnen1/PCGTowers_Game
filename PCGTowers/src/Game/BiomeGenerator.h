#pragma once

#include <Game/TDTilemap.h>

class BiomeGenerator
{
public:
	virtual void Generate(TDTilemap& tilemap) = 0;

	virtual bool IsNavigable(dragon::TileID tile) = 0;
};