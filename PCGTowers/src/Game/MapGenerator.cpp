#include "MapGenerator.h"

#include <Game/BiomeGenerator.h>
//#include <Game/BiomeGenerators.h>

#include <EASTL/set.h>
#include <EASTL/unordered_set.h>
#include <EASTL/array.h>
#include <EASTL/unordered_map.h>

#include <Dragon/Graphics/RenderTarget.h>
#include <SFML/Graphics.hpp>
#include <Platform/SFML/SfmlHelpers.h>

void MapGenerator::Generate(TDTilemap& tilemap)
{
	dragon::Vector2u size = tilemap.GetSize();

	// Get Biome
	//BiomeType biome = GetBiome(m_temperature, m_precipitation);
	BiomeType biome = BiomeType::kDesert;

	//BiomeGenerator* pGenerator = m_generators[(size_t)biome];
	//pGenerator->Generate(tilemap);

	for (size_t x = 0; x < size.x; ++x)
	{
		for (size_t y = 0; y < size.y; ++y)
		{
			// Set Noise Data
			float noise = m_perlinNoise.AverageNoise((float)x / (float)size.x, (float)y / (float)size.y, m_zoom, m_octaves, m_persistance, time(0));
			noise = dragon::math::SmootherStep(noise);
			noise = dragon::math::SmootherStep(noise);
			noise = dragon::math::SmootherStep(noise);

			tilemap.GetTileData(x, y).m_noise = noise;
		}
	}

}

MapGenerator::~MapGenerator()
{
	for (BiomeGenerator* pGenerator : m_generators)
		delete pGenerator;
}

bool MapGenerator::Init()
{
	// Initialize Default Generators

	return m_biomeLookup.loadFromFile("biome_data.png");
}

void MapGenerator::Seed(unsigned int seed)
{
	m_perlinNoise.Seed(seed);
}

void MapGenerator::SetBiomeGenerator(BiomeType type, BiomeGenerator* pGenerator)
{
	if (m_generators[(size_t)type])
		delete m_generators[(size_t)type];

	m_generators[(size_t)type] = pGenerator;
}

BiomeType MapGenerator::GetBiome(float temp, float precip)
{

	float x = (float)(g_kMaxTemperature - g_kMinPrecipitation) / temp;
	float y = (float)(g_kMaxPrecipitation - g_kMinPrecipitation) / precip;

	sf::Vector2u size = m_biomeLookup.getSize();

	unsigned int xLookup = (unsigned int)(x * size.x);
	unsigned int yLookup = (unsigned int)(y * size.y);

	sf::Color pixelColor = m_biomeLookup.getPixel(xLookup, yLookup);

	return static_cast<BiomeType>(pixelColor.toInteger());
}