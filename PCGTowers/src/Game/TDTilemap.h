#pragma once

#include <Dragon/Game/Tilemap/Tilemap.h>

struct TDTileData
{
	float m_noise;
	float m_temperature; // Fahrenheit
	float m_moistureLevel;
	float m_height;
};

using TDTilemap = dragon::DataTilemap<TDTileData>;