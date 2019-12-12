#pragma once

#include <Game/TDTilemap.h>
#include <EASTL/array.h>

#include <SFML/Graphics/Vertex.hpp>

namespace dragon
{
	class RenderTarget;
}

class PathFinder
{
	using Path = eastl::vector<sf::Vertex>;
	Path m_path;

	size_t m_start;
	size_t m_end;

	const TDTilemap* m_pMap;

public:

	void SetTilemap(const TDTilemap* pMap) { m_pMap = pMap; }

	void SetStartTile(size_t start) { m_start = start; FindPath(); }
	void SetEndTile(size_t end) { m_end = end; FindPath(); }
	void Recalculate() { FindPath(); }

	void DrawPath(dragon::RenderTarget& target);

private:

	void FindPath();

	eastl::vector<size_t> CreatePath(size_t begin, size_t goal, const TDTilemap& tilemap);
	float GetDistanceToTile(size_t from, size_t to, const TDTilemap& tilemap);

	using Neighborhood = eastl::array<size_t, 4>;
	void GetNeighboringTiles(size_t tileIndex, Neighborhood& neighbors, const TDTilemap& tilemap);
};