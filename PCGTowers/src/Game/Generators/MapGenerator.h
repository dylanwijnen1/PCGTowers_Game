#pragma once

#include <Game/Biome.h>
#include <Game/TowerDefense/TDTilemap.h>
#include <Game/Path.h>

#include <Dragon/Generic/Random/Range.h>
#include <Dragon/Generic/Random/PerlinNoise.h>
#include <Dragon/Generic/Random.h>

#include <EASTL/array.h>
#include <EASTL/unordered_map.h>

#include <SFML/Graphics/Image.hpp>

/// <summary>
/// Generates tile information onto the tilemap.
/// </summary>
class MapGenerator
{
	dragon::PerlinNoise m_perlinNoise;
	dragon::Random m_random;

	// Temperature Info
	float m_temperature;
	float m_precipitation;

	// Noise Info
	float m_zoom;
	float m_persistance;
	int m_octaves;

	sf::Image m_biomeLookup;
	
public:

	/// <summary>
	/// Theme Pick Formula:
	/// (themeIndex * kCount) + tile;
	/// </summary>
	enum struct MapTile
	{
		kPlain,
		kMoist,
		kPath,
		kBase,
		kDense,
		kVeryMoist,
		kPathVeryMoist,
		kCount
	};

	struct BiomeInfo
	{
		size_t themeIndex;
	};

	static const eastl::unordered_map<BiomeType, BiomeInfo> s_kBiomeInfo;

	using PossiblePositions = eastl::vector<dragon::Vector2>;

	MapGenerator()
		: m_zoom(10.0f)
		, m_persistance(0.5f)
		, m_octaves(2)

		, m_temperature(10.0f)
		, m_precipitation(100.0f)
	{}

	bool Init();

	void Generate(TDTilemap& tilemap, unsigned int seed);

	void SetPrecipitation(float precip) { m_precipitation = precip; }
	void SetTemperature(float temp) { m_temperature = temp; }

	void SetBaseTile(TDTilemap& tilemap, dragon::Vector2 position);

	BiomeType GetBiomeType(float temp, float precip);

	dragon::TileID GetBiomeTile(BiomeType biomeType, MapTile tile);

	/// <summary>
	/// Finds random positions on the map to place the base.
	/// Scoring is higher in the center of the map.
	/// </summary>
	/// <param name="map"></param>
	virtual void FindBestBasePosition(const TDTilemap& tilemap, PossiblePositions& positions);

	/// <summary>
	/// Finds best possible positions for enemy spawners.
	/// Scoring is higher farther away of given position.
	/// </summary>
	/// <param name="map"></param>
	/// <param name="position"></param>
	/// <param name="positions"></param>
	virtual void FindEnemySpawnerLocations(const TDTilemap& tilemap, dragon::Vector2 position, PossiblePositions& positions);

	/// <summary>
	/// Carves a path into the map and returns the Path centers.
	/// </summary>
	/// <param name="path">Centroid oriented Path</param>
	virtual Path CarvePath(TDTilemap& tilemap, dragon::Vector2 from, dragon::Vector2 to);

protected:

	using Neighborhood = eastl::array<int, 4>;

	/// <summary>
	/// A* search a path between [from] and [to] in the tilemap using noise weight of the tiles.
	/// </summary>
	/// <param name="tilemap"></param>
	/// <param name="from"></param>
	/// <param name="to"></param>
	/// <returns></returns>
	virtual TilePath GeneratePath(const TDTilemap& tilemap, int from, int to);

	void GetNeighboringTiles(const TDTilemap& tilemap, int tileIndex, Neighborhood& neighborhood);

	float GetDistanceToTile(const TDTilemap& tilemap, int from, int to);

	/// <summary>
	/// Determines which pathing tile this tile should become.
	/// </summary>
	/// <param name="biomeType">Determines the biome tileset.</param>
	/// <param name="tileIndex">Determines which tile to return.</param>
	/// <returns></returns>
	dragon::TileID GetPathTile(BiomeType biomeType, dragon::TileID tileIndex);

	void GrowRivers(TDTilemap& tilemap);
	void GrowRiversIteration(TDTilemap& tilemap);
	void ApplyGrowRiversRule(const dragon::Tilemap& map, dragon::TileID riverTile, size_t start, size_t end, dragon::TileID* pNewState);
};