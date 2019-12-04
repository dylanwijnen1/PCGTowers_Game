#pragma once

#include <Dragon/Generic/Random/PerlinNoise.h>

namespace dragon
{
	class Tilemap;
}

/// <summary>
/// Factory for generated maps.
/// </summary>
class MapGenerator
{
	dragon::PerlinNoise m_perlinNoise;
	
public:

	void Generate(dragon::Tilemap& map);

	void Seed(unsigned int seed);
};