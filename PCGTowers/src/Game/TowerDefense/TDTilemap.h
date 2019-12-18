#pragma once

#include <Dragon/Game/Tilemap/Tilemap.h>

struct TDTileData
{
	float m_noise;
	float m_temperature; // Fahrenheit
	float m_moistureLevel;
	float m_height;

	dragon::Vector2f m_temp;
};

using TDTilemap = dragon::DataTilemap<TDTileData>;