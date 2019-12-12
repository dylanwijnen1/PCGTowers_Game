#pragma once

#include <Config.h>
#include <Game/TDTilemap.h>
#include <Game/Biome.h>

#include <Dragon/Generic/Random/PerlinNoise.h>

#include <EASTL/array.h>

#include <SFML/Graphics/Image.hpp>

class BiomeGenerator;

namespace dragon
{
	class RenderTarget;
}

/// <summary>
/// Factory for generated maps.
/// 
/// </summary>
class MapGenerator
{
	dragon::PerlinNoise m_perlinNoise;

	float m_temperature;
	float m_precipitation;

	sf::Image m_biomeLookup;

	using BiomeGenerators = eastl::array<BiomeGenerator*, (size_t)BiomeType::kCount>;
	BiomeGenerators m_generators;
	
public:

	float m_zoom = 10.0f;
	float m_persistance = 0.5f;
	int m_octaves = 2;

	~MapGenerator();

	bool Init();

	void Generate(TDTilemap& tilemap);

	void Seed(unsigned int seed);

	void SetBiomeGenerator(BiomeType type, BiomeGenerator* pGenerator);

private:

	BiomeType GetBiome(float temp, float precip);

};