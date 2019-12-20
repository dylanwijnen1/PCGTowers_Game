#pragma once

#include <Dragon/Game/Tilemap/Tilemap.h>

struct TDTileData
{
	float m_noise;			// Noise data of this tile.
	float m_temperature;	// Temperature of this tile in Fahrenheit
	float m_moistureLevel;	// Moisture level of this tile in CM^3

	bool m_isTurretPlaceable; // If a turret can be placed on this tile.

	TDTileData()
		: m_noise(0.0f)
		, m_temperature(0.0f)
		, m_moistureLevel(0.0f)
		, m_isTurretPlaceable(true)
	{}
};

using TDTilemap = dragon::DataTilemap<TDTileData>;