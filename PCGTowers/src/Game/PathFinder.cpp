#include "PathFinder.h"

#include <EASTL/array.h>
#include <EASTL/vector.h>
#include <EASTL/unordered_map.h>
#include <EASTL/unordered_set.h>
#include <EASTL/set.h>

#include <Dragon/Graphics/RenderTarget.h>

#include <SFML/Graphics/RenderTarget.hpp>

#include <Platform/SFML/SfmlHelpers.h>

void PathFinder::DrawPath(dragon::RenderTarget& target)
{
	auto pSfTarget = target.GetNativeTarget<sf::RenderTarget*>();
	pSfTarget->draw(m_path.data(), m_path.size(), sf::LineStrip);
}

void PathFinder::FindPath()
{
	const TDTilemap& tilemap = *m_pMap;
	m_path.clear();

	auto pathFound = CreatePath(m_start, m_end, tilemap);

	for (size_t tile : pathFound)
	{
		dragon::Vector2 pos = tilemap.PositionFromIndex(tile);
		dragon::Vector2f worldCoords = tilemap.MapToWorldCoordinates(pos);

		m_path.emplace_back(sf::Convert(worldCoords), sf::Color::Red);
	}
}

eastl::vector<size_t> PathFinder::CreatePath(size_t begin, size_t goal, const TDTilemap& tilemap)
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

	using NodeMap = eastl::unordered_map<size_t, Node>;
	NodeMap nodes;

	auto hueristicSort = [&nodes](size_t left, size_t right) -> bool
	{
		return nodes[left].global < nodes[right].global;
	};

	using OpenSet = eastl::set<size_t, decltype(hueristicSort)>;
	OpenSet openSet(hueristicSort);

	using ClosedSet = eastl::unordered_set<size_t>;
	ClosedSet closedSet;

	Neighborhood neighbors;

	// Create first node.
	openSet.emplace(begin);
	nodes.emplace(begin, Node(begin, GetDistanceToTile(begin, goal, tilemap), 0.0f, nullptr));

	while (!openSet.empty())
	{
		// Pop And Get Front.
		auto it = openSet.begin();
		size_t current = *it;
		openSet.erase(it);

		if (current == goal)
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

		GetNeighboringTiles(current, neighbors, tilemap);
		for (size_t neighbor : neighbors)
		{
			// Skip if neighbor is invalid.
			if (neighbor == dragon::kInvalidTile)
				continue;

			float edgeWeight = 1.0f - tilemap.GetTileDataAtIndex(neighbor).m_noise;
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

float PathFinder::GetDistanceToTile(size_t from, size_t to, const TDTilemap& tilemap)
{
	dragon::Vector2 posFrom = tilemap.PositionFromIndex(from);
	dragon::Vector2 posTo = tilemap.PositionFromIndex(to);

	return (float)dragon::Vector2::DistanceSquared(posFrom, posTo);
}

void PathFinder::GetNeighboringTiles(size_t tileIndex, Neighborhood& neighborhood, const TDTilemap& tilemap)
{
	dragon::Vector2 tilePos = tilemap.PositionFromIndex(tileIndex);

	neighborhood[0] = tilemap.WithinBounds(tilePos.x - 1, tilePos.y) ? tilemap.IndexFromPosition(tilePos.x - 1, tilePos.y) : dragon::kInvalidTile;
	neighborhood[1] = tilemap.WithinBounds(tilePos.x + 1, tilePos.y) ? tilemap.IndexFromPosition(tilePos.x + 1, tilePos.y) : dragon::kInvalidTile;
	neighborhood[2] = tilemap.WithinBounds(tilePos.x, tilePos.y - 1) ? tilemap.IndexFromPosition(tilePos.x, tilePos.y - 1) : dragon::kInvalidTile;
	neighborhood[3] = tilemap.WithinBounds(tilePos.x, tilePos.y + 1) ? tilemap.IndexFromPosition(tilePos.x, tilePos.y + 1) : dragon::kInvalidTile;
}