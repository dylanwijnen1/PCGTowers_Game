#pragma once

#include <Game/TowerDefense/TDTilemap.h>
#include <Game/TowerDefense/Spawner.h>
#include <Game/GameDifficulty.h>

#include <EASTL/vector.h>
#include <Dragon/Generic/Math.h>

namespace dragon
{
	class RenderTarget;
}

/// <summary>
/// Round holds the map and state information for the round.
/// </summary>
class Round
{
public:
	
	/// <summary>
	/// Node used in RoundGraph holds the information for a round until ready to be used to create the actual map.
	/// </summary>
	struct RoundData
	{
		//
		// Round Map Data
		//
		float m_precipitation;
		float m_temperature;

		/// <summary>
		/// Seed for the current round.
		/// </summary>
		unsigned int m_seed;

		/// <summary>
		/// The wave generator for this node.
		/// </summary>
		class WaveGenerator* m_pWaveGenerator;

		RoundData()
			: m_pWaveGenerator(nullptr)
			, m_precipitation(0.0f)
			, m_temperature(0.0f)
			, m_seed(0)
		{}
	};

private:

	RoundData m_roundData;

	class World* m_pWorld;

	using Spawners = eastl::vector<Spawner>;
	Spawners m_spawners;

	struct Base
	{
		float m_health;
		dragon::Vector2 m_tilePosition;

		Base()
			: Base(0.0f, { 0,0 })
		{}

		Base(float health, dragon::Vector2 pos)
			: m_health(health)
			, m_tilePosition(pos)
		{}

	} m_base;

	// Round State
	size_t m_currentWave;
	float m_waveTimer; // Based on difficulty.
	GameDifficulty m_difficulty;
	bool m_isPaused;

	float m_roundScore;
	float m_waveScore;

	//
	// Events
	//

	// using RoundFinishedEvent = Event<void(bool, score)>;
	// RoundFinishedEvent onRoundFinished; // TODO: World attaches to event to be notified when the round has finished and generate the next.

public:

	Round(const RoundData& data, World* pWorld)
		: m_roundData(data)
		, m_pWorld(pWorld)

		// State
		, m_currentWave(0)
		, m_waveTimer(0.0f)
		, m_difficulty(GameDifficulty::kNone)
		, m_isPaused(true) // Round starts of paused.
		, m_roundScore(0.0f)
		, m_waveScore(0.0f)
	{}

	template<typename... Args>
	void EmplaceSpawner(Args... args) { m_spawners.emplace_back(eastl::forward<Args>(args)...); }

	void SetDifficulty(GameDifficulty difficulty) { m_difficulty = difficulty; }

	/// <summary>
	/// Starts the next wave.
	/// </summary>
	void NextWave();

	/// <summary>
	/// Pauses the current round.
	/// </summary>
	void Pause() { m_isPaused = true; }

	/// <summary>
	/// Resumes the round.
	/// </summary>
	void Resume() { m_isPaused = false; }
	
	/// <summary>
	/// Ends the round
	/// </summary>
	void EndRound(); 

	bool HasFinished() const;

	void SetBaseHealth(float health) { m_base.m_health = health; }

	void Update(float dt);

	/// <summary>
	/// Render the state of the round to the screen, including the tilemap.
	/// </summary>
	/// <param name="target"></param>
	void Render(dragon::RenderTarget& target);

private:

	/// <summary>
	/// Calculates the score accumulated for this wave.
	/// </summary>
	/// <param name="time"></param>
	/// <returns></returns>
	float CalculateWaveScore(float time) const;

};