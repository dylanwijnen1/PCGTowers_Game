#include "Spawner.h"

#include <Config.h>

#include <Game/TowerDefense/Enemy.h>
#include <Game/TowerDefense/World.h>

#include <Dragon/Graphics/RenderTarget.h>
#include <Platform/SFML/SfmlHelpers.h>
#include <SFML/Graphics.hpp>

Spawner::~Spawner()
{
	ClearEnemyGroups();
}

void Spawner::UpdateWaveTiming(float waveTime)
{
	m_timeBetweenGroups = waveTime / m_groups.size();

	// Reset timers
	m_currentGroupTime = m_timeBetweenGroups; // Must be set to group time to start the first group spawn.
	m_timeBetweenEnemiesForGroup = m_timeBetweenGroups / m_groups.front().size();

	m_currentEnemyTime = 0.f; // Immediatly start.
}

void Spawner::ClearEnemyGroups()
{
	while (!m_groups.empty())
	{
		auto& group = m_groups.front();

		while (!group.empty())
		{
			// Delete enemies.
			Enemy* pEnemy = group.front();
			delete pEnemy;
			group.pop();
		}

		m_groups.pop();
	}
}

void Spawner::Update(float dt)
{
	// Enemy Group
	auto& currentGroup = m_groups.front();

	m_currentEnemyTime -= dt;
	if (m_currentEnemyTime < 0.0f)
	{
		m_currentEnemyTime = m_timeBetweenEnemiesForGroup;

		// Spawn the next enemy in the queue.
		if (!currentGroup.empty())
		{
			Enemy* pEnemy = currentGroup.front();
			currentGroup.pop();

			pEnemy->SetPath(&m_pathToGoal); // Set the path of the enemy.
			m_pWorld->AddEnemy(pEnemy);
		}
	}

	// Continue to next group if all enemies have spawned in from the current group.
	m_currentGroupTime -= dt;
	if (m_currentGroupTime < 0.0f && currentGroup.empty())
	{
		// If there are still groups left continue.
		if (!m_groups.empty())
		{
			m_groups.pop();

			// Reset Timer
			m_currentGroupTime = m_timeBetweenGroups;

			// Calculate new timing data for the next group.
			m_timeBetweenEnemiesForGroup = m_timeBetweenGroups / m_groups.front().size();
		}
	}
}

void Spawner::Render(dragon::RenderTarget& target)
{
#if _DEBUG
	sf::RenderTarget* pTarget = target.GetNativeTarget<sf::RenderTarget*>();

	eastl::vector<sf::Vertex> vertices;
	vertices.reserve(m_pathToGoal.size());
	for (auto pos : m_pathToGoal)
	{
		vertices.emplace_back(sf::Convert(pos), sf::Color::Red);
	}

	pTarget->draw(vertices.data(), vertices.size(), sf::PrimitiveType::LineStrip);
#endif
}
