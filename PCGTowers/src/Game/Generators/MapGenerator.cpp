#include "MapGenerator.h"

#include <Config.h>

#include <EASTL/set.h>
#include <EASTL/unordered_set.h>
#include <EASTL/unordered_map.h>

#include <Dragon/Graphics/RenderTarget.h>
#include <SFML/Graphics.hpp>
#include <Platform/SFML/SfmlHelpers.h>

/// <summary>
/// Terrain, 
/// Dense Terrain(Pathing noise ? )
/// Path Tile,
/// Base Tile,
/// Impassable Terrain(Impassable terrain unavigable),
/// Impassable Navigable,
/// PathTileImpassable
/// </summary>

/*
	kUnknown = 0,
	kTundra = 0x93A7ACFF,
	kTaiga = 0x5B8F52FF,
	kWoodland = 0xB37C06FF,
	kGrassland = 0x927E30FF,
	kSeasonalForest = 0x2C89A0FF,
	kRainForestTemperate = 0x0A546DFF,
	kRainForestTropical = 0x075330FF,
	kSavannah = 0x96A527FF,
	kDesert = 0xC87137FF,
*/
const eastl::unordered_map<BiomeType, MapGenerator::BiomeInfo> MapGenerator::s_kBiomeInfo = 
{
	{ BiomeType::kTundra, { 1 } },
	{ BiomeType::kTaiga, { 1 } },
	{ BiomeType::kWoodland, { 0 } },
	{ BiomeType::kGrassland, { 0 } },
	{ BiomeType::kSeasonalForest, { 0 } },
	{ BiomeType::kRainForestTemperate, { 0 } },
	{ BiomeType::kRainForestTropical, { 0 } },
	{ BiomeType::kSavannah, { 2 } },
	{ BiomeType::kDesert, { 2 } },
};

void MapGenerator::Generate(TDTilemap& tilemap, unsigned int seed)
{
	// Seed the randomizer and perlin noise.
	m_perlinNoise.Seed(seed);
	m_random.Seed(seed);

	dragon::Vector2u size = tilemap.GetSize();

	BiomeType biome = GetBiomeType(m_temperature, m_precipitation);

	auto calculateBiomeDensity = [](float temp, float precip) -> float
	{
		temp /= g_kMaxTemperature;
		precip /= g_kMaxPrecipitation;

		float temperatureVal = std::sin(temp * 3.14f) * temp;
		float precipitationVal = std::sin(precip * 3.14f / 2.0f);
		return precipitationVal * 1.5f * temperatureVal;
	};

	float biomeDensity = calculateBiomeDensity(m_temperature, m_precipitation);

	// Generate noise for tilemap.
	for (unsigned int x = 0; x < size.x; ++x)
	{
		for (unsigned int y = 0; y < size.y; ++y)
		{
			auto& tileData = tilemap.GetTileData(x, y);
			tileData.m_isTurretPlaceable = true; // Reset Placeable status.

			// Height Noise [0.0f, 1.0f]
			float noise = m_perlinNoise.AverageNoise((float)x / (float)size.x, (float)y / (float)size.y, m_zoom, m_octaves, m_persistance, seed);

			// Temperature Noise
			float tileTemperature = m_perlinNoise.AverageNoise((float)x / (float)size.x, (float)y / (float)size.y, m_zoom * 2.0f, 1, 0.5f, seed);
			tileTemperature *= m_temperature;
			tileData.m_temperature = tileTemperature;

			// Precipitation Noise
			float tileMoisture = m_perlinNoise.AverageNoise((float)x / (float)size.x, (float)y / (float)size.y, 4.0f, 4, 0.4f, seed);
			float tilePrecipitation = tileMoisture;
			tilePrecipitation *= m_precipitation;
			tileData.m_moistureLevel = tilePrecipitation;

			if (m_random.RandomUniform() < biomeDensity * noise)
			{
				tilemap.SetTile(x, y, GetBiomeTile(biome, MapTile::kDense));
				tileData.m_isTurretPlaceable = false;
			}
			else
			{
				float totalMoisture = std::abs(std::sin(tilePrecipitation * 3.14f / 2.0f) * tileMoisture);
				if (totalMoisture > .3f && totalMoisture < .5f)
					tilemap.SetTile(x, y, GetBiomeTile(biome, MapTile::kMoist));
				else if (totalMoisture > .7f)
				{
					tilemap.SetTile(x, y, GetBiomeTile(biome, MapTile::kVeryMoist));
					tileData.m_isTurretPlaceable = false;
				}
				else
					tilemap.SetTile(x, y, GetBiomeTile(biome, MapTile::kPlain));
			}

			// Calculate noise for pathing.
			noise = dragon::math::SmootherStep(noise);
			noise = dragon::math::SmootherStep(noise);
			noise = dragon::math::SmootherStep(noise);

			tileData.m_noise = noise;
		}
	}

	GrowRivers(tilemap);
}

bool MapGenerator::Init()
{
	return m_biomeLookup.loadFromFile("biome_data.png");
}

void MapGenerator::SetBaseTile(TDTilemap& tilemap, dragon::Vector2 position)
{
	BiomeType biome = GetBiomeType(m_temperature, m_precipitation);
	tilemap.SetTile(position.x, position.y, GetBiomeTile(biome, MapTile::kBase));
}

BiomeType MapGenerator::GetBiomeType(float temp, float precip)
{

	float x = temp / (float)(g_kMaxTemperature - g_kMinTemperature);
	float y = precip / (float)(g_kMaxPrecipitation - g_kMinPrecipitation);

	sf::Vector2u size = m_biomeLookup.getSize();

	unsigned int xLookup = (unsigned int)(x * size.x);
	unsigned int yLookup = (unsigned int)(y * size.y);

	sf::Color pixelColor = m_biomeLookup.getPixel(xLookup, (size.y - 1) - yLookup);

	return static_cast<BiomeType>(pixelColor.toInteger());
}

void MapGenerator::FindBestBasePosition(const TDTilemap& tilemap, PossiblePositions& positions)
{
	size_t count = 0;

	dragon::Vector2u mapSize = tilemap.GetSize();

	// Draws an M on desmos.
	auto coordWeight = [](float in) -> float
	{
		return (std::abs(std::sin(2.0f * 3.14f * in)) + 0.5f - 0.5f * std::abs(std::cos(3.14f * in))) / 1.164f;
	};

	// If we've reached max amount of tries, and we have atleast found one position.
	while (count < g_kMaxTries || positions.size() == 0)
	{
		int x = m_random.RandomRange<int>(0, (int)mapSize.x);
		int y = m_random.RandomRange<int>(0, (int)mapSize.y);

		float xWeight = coordWeight((float)x / (mapSize.x - 1));
		float yWeight = coordWeight((float)y / (mapSize.y - 1));
		float tileWeight = (xWeight + yWeight) / 2.0f;

		tileWeight *= 0.8f; // 80% influence;

		// Makes sure that we always find a base.
		tileWeight = (count > g_kMaxTries ? 1.0f : tileWeight);

		if (m_random.RandomUniform() > 1.0f - tileWeight)
		{
			positions.emplace_back(x, y);
		}

		++count;
	}
}

void MapGenerator::FindEnemySpawnerLocations(const TDTilemap& tilemap, dragon::Vector2 position, PossiblePositions& positions)
{
	dragon::Vector2u mapSize = tilemap.GetSize();

	/// <summary>
	/// Calculates the weight for the given tile.
	/// </summary>
	auto tileWeight = [position](size_t x, size_t y) -> float
	{
		dragon::Vector2 direction = position - dragon::Vector2((int)x, (int)y);
		int distance = std::abs(direction.Length());
		
		float distanceValue = (float)dragon::math::Max(0, distance - g_kMinDistanceOfSpawner);
		distanceValue /= (float)((int)g_kMapSize - g_kMinDistanceOfSpawner);

		return dragon::math::Sin(distanceValue * 2.3f);
	};

	// For every tile find a possible location.
	for(size_t x = 0; x < mapSize.x; ++x)
	{
		for (size_t y = 0; y < mapSize.y; ++y)
		{

			float weight = tileWeight(x, y);

			if (m_random.RandomUniform() > 1.0f - weight)
			{
				positions.emplace_back(x, y);
			}

		}
	}
}

Path MapGenerator::CarvePath(TDTilemap& tilemap, dragon::Vector2 from, dragon::Vector2 to)
{
	int fromIndex = tilemap.IndexFromPosition(from);
	int toIndex = tilemap.IndexFromPosition(to);

	TilePath tilePath = GeneratePath(tilemap, fromIndex, toIndex);

	BiomeType biome = GetBiomeType(m_temperature, m_precipitation);

	Path path;
	path.reserve(tilePath.size());

	for (int tileIndex : tilePath)
	{
		tilemap.GetTileDataAtIndex(tileIndex).m_isTurretPlaceable = false;

		dragon::TileID tilePathId = GetPathTile(biome, tilemap.GetTileAtIndex(tileIndex));
		tilemap.SetTileAtIndex(tileIndex, tilePathId);

		// Find centroid of tile.
		dragon::Vector2 tilePos = tilemap.PositionFromIndex(tileIndex);
		dragon::Vector2f centroidPos =
		{
			(float)tilePos.x * g_kTileSize + (g_kTileSize / 2.0f),
			(float)tilePos.y * g_kTileSize + (g_kTileSize / 2.0f)
		};

		path.emplace_back(centroidPos);
	}

	return path;
}

TilePath MapGenerator::GeneratePath(const TDTilemap& tilemap, int from, int to)
{
	// Note: EASTL has no constexpr infinity...
	constexpr float kInf = std::numeric_limits<float>::infinity();

	struct Node
	{
		size_t tileIndex;
		float global;
		float local;
		Node* pPrevious;

		Node()
			: Node(dragon::kInvalidTile)
		{}

		Node(size_t index)
			: tileIndex(index)
			, global(kInf)
			, local(kInf)
			, pPrevious(nullptr)
		{}

		Node(int index, float glob, float loc, Node* pPrev)
			: tileIndex(index)
			, global(glob)
			, local(loc)
			, pPrevious(pPrev)
		{}
	};

	TilePath path;

	using NodeMap = eastl::unordered_map<int, Node>;
	NodeMap nodes;

	auto hueristicSort = [&nodes](int left, int right) -> bool
	{
		return nodes[left].global < nodes[right].global;
	};

	using OpenSet = eastl::set<int, decltype(hueristicSort)>;
	OpenSet openSet(hueristicSort);

	using ClosedSet = eastl::unordered_set<int>;
	ClosedSet closedSet;

	Neighborhood neighbors;

	// Create first node.
	openSet.emplace(from);
	nodes.emplace(from, Node(from, GetDistanceToTile(tilemap, from, to), 0.0f, nullptr));

	while (!openSet.empty())
	{
		// Pop And Get Front.
		auto it = openSet.begin();
		int current = *it;
		openSet.erase(it);

		if (current == to)
		{
			Node* pNode = &nodes[current];
			while (pNode->pPrevious != nullptr)
			{
				path.emplace_back(pNode->tileIndex);
				pNode = pNode->pPrevious;
			}
		}

		// Find current node path score.
		Node* pCurrentNode = &nodes[current];

		GetNeighboringTiles(tilemap, current, neighbors);
		for (int neighbor : neighbors)
		{
			// Skip if neighbor is invalid.
			if (neighbor == dragon::kInvalidTile)
				continue;

			float edgeWeight = 2.0f - tilemap.GetTileDataAtIndex(neighbor).m_noise;
			edgeWeight *= 100.0f;

			float newPathScore = pCurrentNode->local + edgeWeight;

			// Find neighborScore if exists.
			auto neighborIt = nodes.find(neighbor);

			// Create if it doesn't
			if (neighborIt == nodes.end())
			{
				neighborIt = nodes.emplace(neighbor, Node(neighbor)).first;
			}

			Node* pNeighbor = &neighborIt->second;

			if (newPathScore < pNeighbor->local)
			{
				//cameFrom[neighbor] : = current
				//gScore[neighbor] : = tentative_gScore
				//fScore[neighbor] : = gScore[neighbor] + h(neighbor)
				pNeighbor->pPrevious = pCurrentNode;
				pNeighbor->local = newPathScore;
				pNeighbor->global = newPathScore + (GetDistanceToTile(tilemap, neighbor, to) * 10.0f);

				if (closedSet.find(neighbor) == closedSet.end())
				{
					// Add to closed set, We've now visited this neighbor.
					closedSet.emplace(neighbor);
					openSet.emplace(neighbor);
				}
			}
		}
	}

	eastl::reverse(path.begin(), path.end());

	return path;
}

void MapGenerator::GetNeighboringTiles(const TDTilemap& tilemap, int tileIndex, Neighborhood& neighborhood)
{
	dragon::Vector2 tilePos = tilemap.PositionFromIndex(tileIndex);

	neighborhood[0] = tilemap.WithinBounds(tilePos.x - 1, tilePos.y) ? tilemap.IndexFromPosition(tilePos.x - 1, tilePos.y) : dragon::kInvalidTile;
	neighborhood[1] = tilemap.WithinBounds(tilePos.x + 1, tilePos.y) ? tilemap.IndexFromPosition(tilePos.x + 1, tilePos.y) : dragon::kInvalidTile;
	neighborhood[2] = tilemap.WithinBounds(tilePos.x, tilePos.y - 1) ? tilemap.IndexFromPosition(tilePos.x, tilePos.y - 1) : dragon::kInvalidTile;
	neighborhood[3] = tilemap.WithinBounds(tilePos.x, tilePos.y + 1) ? tilemap.IndexFromPosition(tilePos.x, tilePos.y + 1) : dragon::kInvalidTile;
}

float MapGenerator::GetDistanceToTile(const TDTilemap& tilemap, int from, int to)
{
	dragon::Vector2 posFrom = tilemap.PositionFromIndex(from);
	dragon::Vector2 posTo = tilemap.PositionFromIndex(to);

	return (float)dragon::Vector2::DistanceSquared(posFrom, posTo);
}

dragon::TileID MapGenerator::GetPathTile(BiomeType biomeType, dragon::TileID tileId)
{
	// Get base type of tile.
	MapTile tileType = (MapTile)(tileId % (size_t)MapTile::kCount);

	// Determine what type of path the tile must become.
	if (tileType == MapTile::kVeryMoist)
	{
		tileType = MapTile::kPathVeryMoist;
	}
	else
	{
		tileType = MapTile::kPath;
	}

	return GetBiomeTile(biomeType, tileType);
}

void MapGenerator::GrowRivers(TDTilemap& tilemap)
{

	for (size_t i = 0; i < 3; ++i)
	{
		GrowRiversIteration(tilemap);
	}

}

void MapGenerator::GrowRiversIteration(TDTilemap& tilemap)
{
	const size_t kThreadCount = std::thread::hardware_concurrency();
	const dragon::Vector2u kMapSize = tilemap.GetSize();
	const size_t kTileCount = (size_t)kMapSize.x * (size_t)kMapSize.y;

	BiomeType biome = GetBiomeType(m_temperature, m_precipitation);
	dragon::TileID riverTile = GetBiomeTile(biome, MapTile::kVeryMoist);

	// Spawn threads
	std::thread* pThreads = new std::thread[kThreadCount];

	// Write buffer.
	dragon::TileID* pNewState = new dragon::TileID[kTileCount]{ 0 };

	// Initiate Jobs for the threads
	size_t stride = kTileCount / kThreadCount;
	size_t startIndex = 0;
	size_t endIndex = stride;

	for (size_t i = 0; i < kThreadCount - 1; ++i)
	{
		pThreads[i] = std::thread(&MapGenerator::ApplyGrowRiversRule, this, std::cref(tilemap), riverTile, startIndex, endIndex, pNewState);

		startIndex += stride;
		endIndex += stride;
	}
	ApplyGrowRiversRule(tilemap, riverTile, startIndex, kTileCount, pNewState);

	// Wait for threads to finish.
	for (size_t i = 0; i < kThreadCount - 1; ++i)
	{
		if (pThreads[i].joinable())
			pThreads[i].join();
	}

	// Finish off by setting all the states on the map.
	for (size_t i = 0; i < kTileCount; ++i)
	{
		tilemap.SetTileAtIndex(i, pNewState[i]);
		
		// Recheck if the tile is now unplaceable.
		if (pNewState[i] == riverTile)
		{
			// Can no longer place turrets at this tile.
			tilemap.GetTileDataAtIndex(i).m_isTurretPlaceable = false;
		}
	}

	// Cleanup
	delete[] pThreads;
	delete[] pNewState;
}

void MapGenerator::ApplyGrowRiversRule(const dragon::Tilemap& tilemap, dragon::TileID riverTile, size_t start, size_t end, dragon::TileID* pNewState)
{
	auto mooreNeighborhood = [&tilemap](size_t index, dragon::TileID id) -> size_t
	{
		dragon::Vector2 pos = tilemap.PositionFromIndex((int)index);

		size_t count = 0;
		if (tilemap.GetTile(pos.x - 1, pos.y - 1) == id) ++count;
		if (tilemap.GetTile(pos.x, pos.y - 1) == id) ++count;
		if (tilemap.GetTile(pos.x + 1, pos.y - 1) == id) ++count;

		if (tilemap.GetTile(pos.x - 1, pos.y) == id) ++count;
		if (tilemap.GetTile(pos.x + 1, pos.y) == id) ++count;

		if (tilemap.GetTile(pos.x - 1, pos.y + 1) == id) ++count;
		if (tilemap.GetTile(pos.x, pos.y + 1) == id) ++count;
		if (tilemap.GetTile(pos.x + 1, pos.y + 1) == id) ++count;
		return count;
	};

	for (size_t i = start; i < end; ++i)
	{
		// More than 0 tiles around then grow. 
		if (mooreNeighborhood(i, riverTile) > 1)
		{
			pNewState[i] = riverTile;
		}
		else
		{
			pNewState[i] = tilemap.GetTileAtIndex(i);
		}
	}
}

dragon::TileID MapGenerator::GetBiomeTile(BiomeType biomeType, MapTile tile)
{
	size_t theme = 0;

	if (auto it = s_kBiomeInfo.find(biomeType); it != s_kBiomeInfo.end())
	{
		theme = it->second.themeIndex;
	}

	return (dragon::TileID)((size_t)MapTile::kCount * theme) + (size_t)tile;
}