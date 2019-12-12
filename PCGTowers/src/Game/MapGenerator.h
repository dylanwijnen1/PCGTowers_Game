#pragma once

#include <Config.h>
#include <Game/TDTilemap.h>
#include <Game/Biome.h>

#include <Dragon/Generic/Random/PerlinNoise.h>

#include <EASTL/array.h>
#include <EASTL/vector.h>

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Vertex.hpp>

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

	eastl::vector<sf::Vertex> m_path;
	
public:

	~MapGenerator();

	bool Init();

	void Generate(TDTilemap& tilemap);

	void Seed(unsigned int seed);

	void RenderPath(dragon::RenderTarget& target);

	void SetBiomeGenerator(BiomeType type, BiomeGenerator* pGenerator);

private:

	BiomeType GetBiome(float temp, float precip);
	
	eastl::vector<size_t> CreatePath(size_t begin, size_t goal, TDTilemap& tilemap);
	float GetDistanceToTile(size_t from, size_t to, TDTilemap& tilemap);

	using Neighborhood = eastl::array<size_t, 4>;
	void GetNeighboringTiles(size_t tileIndex, Neighborhood& neighbors, TDTilemap& tilemap);

	// Generator functions.
};