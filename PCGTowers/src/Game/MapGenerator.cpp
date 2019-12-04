#include "MapGenerator.h"

#include <Dragon/Game/Tilemap/Tilemap.h>

void MapGenerator::Generate(dragon::Tilemap& map)
{
	dragon::Vector2u size = map.GetSize();
	for (size_t x = 0; x < size.x; ++x)
	{
		for (size_t y = 0; y < size.y; ++y)
		{
			float noise = m_perlinNoise.NormalizedNoise((float)x / (float)size.x * 10.0f, (float)y / (float)size.y * 10.0f);
			dragon::TileID tileId = (dragon::TileID)(noise * 255u);
			map.SetTile(x, y, tileId);
		}
	}
}

void MapGenerator::Seed(unsigned int seed)
{
	m_perlinNoise.Seed(seed);
}
