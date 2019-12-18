#include "Round.h"

#include <Config.h>

#include <Game/TowerDefense/World.h>
#include <Game/TowerDefense/Spawner.h>

void Round::NextWave()
{
	if (m_currentWave + 1 > g_kWavesPerRound)
	{
		// Early exit, No more waves.
		return;
	}
	
	float difficultyTime = g_kWaveTimes[(size_t)m_difficulty];

	// Reset Wave Timer
	m_waveTimer = difficultyTime;

	// Reset Wave Score
	m_waveScore = 0.0f;

	// Increase Wave Count
	++m_currentWave;

	WaveGenerator* pWaveGenerator = m_pWorld->GetDefaultWaveGenerator();
	if (m_roundData.m_pWaveGenerator)
	{
		pWaveGenerator = m_roundData.m_pWaveGenerator;
	}

	// Generate enemies on the spawners.
	pWaveGenerator->GenerateWaves(m_spawners, m_roundData.m_seed, (unsigned int)m_currentWave);

	for (Spawner& spawner : m_spawners)
	{
		spawner.UpdateWaveTiming(difficultyTime);
	}
}

void Round::EndRound()
{
	Pause();
	//onRoundFinished(true, m_roundScore);
}

bool Round::HasFinished() const
{
	return m_currentWave == g_kWavesPerRound;
}

void Round::Update(float dt)
{
	/// Note:
	/// The following will allow the NextWave() function to only trigger once.
	/// This will prevent the round for calling NextWave() continiously after completing all waves.
	if (m_waveTimer > 0.0f)
	{
		m_waveTimer -= dt;
		if (m_waveTimer < 0.0f)
		{
			// Add scoring for this wave.
			m_roundScore += CalculateWaveScore(m_waveTimer);

			// Start next wave.
			NextWave();
		}
	}

	// Update Spawners
	for (Spawner& pSpawner : m_spawners)
	{
		pSpawner.Update(dt);
	}
}

void Round::Render(dragon::RenderTarget& target)
{
	// Update Spawners
	for (Spawner& pSpawner : m_spawners)
	{
		pSpawner.Render(target);
	}
}

float Round::CalculateWaveScore(float time) const
{
	float difficultyTime = g_kWaveTimes[(size_t)m_difficulty];
	float waveScoreMultiplier = (time > 0.0f) ? 1.0f + (m_waveTimer / difficultyTime) : 1.0f;

	return m_waveScore * waveScoreMultiplier;
}
