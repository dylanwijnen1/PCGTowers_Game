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
			float noise = m_perlinNoise.AverageNoise((float)x / (float)size.x, (float)y / (float)size.y, 10.0f, 2, 0.5f, time(0));
			tilemap.GetTileData(x, y).m_noise = noise;
		}
	}

	// Generate Path
	auto path = CreatePath(0, tilemap.IndexFromPosition(size.x - 1, size.y - 1), tilemap);
	m_path.clear();
	for (size_t index : path)
	{
		dragon::Vector2 pos = tilemap.PositionFromIndex(index);
		dragon::Vector2f worldPos = tilemap.MapToWorldCoordinates(pos);

		m_path.emplace_back(sf::Convert(worldPos), sf::Convert(dragon::Colors::Red));
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

void MapGenerator::RenderPath(dragon::RenderTarget& target)
{
	sf::RenderTarget* pSfTarget = target.GetNativeTarget<sf::RenderTarget*>();
	pSfTarget->draw(m_path.data(), m_path.size(), sf::PrimitiveType::LineStrip);
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

eastl::vector<size_t> MapGenerator::CreatePath(size_t begin, size_t goal, TDTilemap& tilemap)
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

		Node(size_t index, float glob, float loc, Node* pPrev)
			: tileIndex(index)
			, global(glob)
			, local(loc)
			, pPrevious(pPrev)
		{}
	};

	eastl::vector<size_t> path;

	using NodeMap = eastl::unordered_map<size_t, Node*>;
	NodeMap nodes;

	auto hueristicSort = [&nodes](size_t left, size_t right) -> bool
	{
		return nodes[left]->global < nodes[right]->global;
	};

	using OpenSet = eastl::set<size_t, decltype(hueristicSort)>;
	OpenSet openSet(hueristicSort);

	using ClosedSet = eastl::unordered_set<size_t>;
	ClosedSet closedSet;

	Neighborhood neighbors;

	// Create first node.
	openSet.emplace(begin);
	nodes.emplace(begin, new Node( begin, GetDistanceToTile(begin, goal, tilemap), 0.0f, nullptr ));

	while (!openSet.empty())
	{
		// Pop And Get Front.
		auto it = openSet.begin();
		size_t current = *it;
		openSet.erase(it);
		
		if (current == goal)
		{
			Node* pNode = nodes[current];
			while (pNode->pPrevious != nullptr)
			{
				path.emplace_back(pNode->tileIndex);
				pNode = pNode->pPrevious;
			}
		}

		// Find current node path score.
		Node* pCurrentNode = nodes[current];

		GetNeighboringTiles(current, neighbors, tilemap);
		for (size_t neighbor : neighbors)
		{
			// Skip if neighbor is invalid.
			if (neighbor == dragon::kInvalidTile)
				continue;

			float edgeWeight = tilemap.GetTileDataAtIndex(neighbor).m_noise * 100.0f;
			float newPathScore = pCurrentNode->local + edgeWeight;

			// Find neighborScore if exists.
			auto neighborIt = nodes.find(neighbor);

			// Create if it doesn't
			if (neighborIt == nodes.end())
			{
				neighborIt = nodes.emplace(neighbor, new Node(neighbor)).first;
			}

			Node* pNeighbor = neighborIt->second;

			if (newPathScore < pNeighbor->local)
			{
				//cameFrom[neighbor] : = current
				//gScore[neighbor] : = tentative_gScore
				//fScore[neighbor] : = gScore[neighbor] + h(neighbor)
				pNeighbor->pPrevious = pCurrentNode;
				pNeighbor->local = newPathScore;
				pNeighbor->global = newPathScore + GetDistanceToTile(neighbor, goal, tilemap);

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

float MapGenerator::GetDistanceToTile(size_t from, size_t to, TDTilemap& tilemap)
{
	dragon::Vector2 posFrom = tilemap.PositionFromIndex(from);
	dragon::Vector2 posTo = tilemap.PositionFromIndex(to);

	return (float)dragon::Vector2::DistanceSquared(posFrom, posTo);
}

void MapGenerator::GetNeighboringTiles(size_t tileIndex, MapGenerator::Neighborhood& neighborhood, TDTilemap& tilemap)
{
	dragon::Vector2 tilePos = tilemap.PositionFromIndex(tileIndex);

	neighborhood[0] = tilemap.WithinBounds(tilePos.x - 1, tilePos.y) ? tilemap.IndexFromPosition(tilePos.x - 1, tilePos.y) : dragon::kInvalidTile;
	neighborhood[1] = tilemap.WithinBounds(tilePos.x + 1, tilePos.y) ? tilemap.IndexFromPosition(tilePos.x + 1, tilePos.y) : dragon::kInvalidTile;
	neighborhood[2] = tilemap.WithinBounds(tilePos.x, tilePos.y - 1) ? tilemap.IndexFromPosition(tilePos.x, tilePos.y - 1) : dragon::kInvalidTile;
	neighborhood[3] = tilemap.WithinBounds(tilePos.x, tilePos.y + 1) ? tilemap.IndexFromPosition(tilePos.x, tilePos.y + 1) : dragon::kInvalidTile;
}