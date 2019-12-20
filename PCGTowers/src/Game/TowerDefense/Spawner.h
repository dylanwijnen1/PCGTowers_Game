#pragma once


#include <Dragon/Generic/Math.h>

#include <EASTL/vector.h>
#include <EASTL/queue.h>

namespace dragon
{
	class RenderTarget;
}

class Spawner
{
public:
	using SpawnQueue = eastl::queue<class Enemy*>;

private:

	/// <summary>
	/// The world this spawner belongs to.
	/// </summary>
	class World* m_pWorld;

	/// <summary>
	/// The position this spawner is located at.
	/// </summary>
	dragon::Vector2 m_position;

	/// <summary>
	/// Time between each group 
	/// (Wave Time / GroupCount)
	/// </summary>
	float m_timeBetweenGroups;

	/// <summary>
	/// Time between each enemy in the group.
	/// Effectively spawn rate.
	/// (GroupTime / EnemyCount);
	/// </summary>
	float m_timeBetweenEnemiesForGroup; // Effectively SpawnRate.

	/// <summary>
	/// Time till next group starts spawning.
	/// </summary>
	float m_currentGroupTime;

	/// <summary>
	/// Time till next enemy spawns.
	/// </summary>
	float m_currentEnemyTime;

	using Groups = eastl::queue<SpawnQueue>;
	Groups m_groups;

	using Path = eastl::vector<dragon::Vector2f>;
	Path m_pathToGoal;

public:

	Spawner() = default;

	Spawner(World* pWorld, dragon::Vector2 position, Path&& path)
		: m_pWorld(pWorld)
		, m_position(position) 
		, m_pathToGoal(eastl::move(path))
		, m_timeBetweenGroups(0.0f)
		, m_timeBetweenEnemiesForGroup(0.0f)
		, m_currentGroupTime(0.0f)
		, m_currentEnemyTime(0.0f)
	{}

	~Spawner();

	/// <summary>
	/// Must be called before starting to spawn to recalculate timing data.
	/// </summary>
	void UpdateWaveTiming(float waveTime);

	void EmplaceEnemyGroup(SpawnQueue&& queue) { m_groups.emplace_back(eastl::move(queue)); }
	void ClearEnemyGroups();

	void EmplacePath(Path&& path) { m_pathToGoal = eastl::move(path); }

	void Update(float dt, class Round* pRound);

	void Render(dragon::RenderTarget& target);
};