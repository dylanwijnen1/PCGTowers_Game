#include "World.h"

#include <Config.h>

#include <Game/TowerDefense/Enemy.h>
#include <Game/TowerDefense/Spawner.h>
#include <Game/Rounds/Round.h>

#include <Dragon/Application/Application.h>
#include <Dragon/Graphics/RenderTarget.h>


World::~World()
{
	m_pCurrentRound->Pause();
	ClearEnemies();

	delete m_pDefaultWaveGenerator;
	delete m_pCurrentRound;
}

void World::Init()
{
	m_tilemap.Init({ g_kMapSize, g_kMapSize }, { g_kTileSize, g_kTileSize });
	m_tilemap.LoadTileset("tileset.png");

	m_mapGenerator.Init();

	m_pDefaultWaveGenerator = new WaveGenerator();
	m_pDefaultWaveGenerator->InitDefaults();

#if _DEBUG
	m_noiseTilemap.Init({ g_kMapSize, g_kMapSize }, { g_kTileSize, g_kTileSize });
	m_noiseTilemap.LoadTileset("gradient-tiles.png");
#endif
}

void World::Reset()
{
	if (m_pCurrentRound)
	{
		ClearEnemies();
		delete m_pCurrentRound;
	}


	// Set current level to first level in graph.
	//auto roundInfo = m_rounds.GetVertices().front(); // Get The first node.

	// Give player initial gold and initial turret.
	//m_player.m_inventory.emplace_back(m_turretGenerator.Generate(m_difficulty, m_seed));

	// TODO: Remove Temp Round
	RoundInfo info;
	info.m_temperature = m_random.RandomRange(g_kMinTemperature, g_kMaxTemperature);
	info.m_precipitation = m_random.RandomRange(g_kMinPrecipitation, g_kMaxPrecipitation);
	info.m_roundSeed = m_random.Random<unsigned int>();

	GenerateRound(info);
}

void World::GenerateWorld(unsigned int seed)
{
	// Allows for seed to be 0.
	unsigned int worldSeed = dragon::SquirrelNoise::Get1DNoise(1, seed);

	m_random.Seed(worldSeed);

	// Get the max depth for this Game's RoundGraph based on difficulty.
	size_t difficultyDepth = g_kDepthOnDifficulty[(size_t)m_difficulty].GetRandom(m_random);

	// Generate rounds
	//m_rounds = m_roundsGenerator.Generate(difficultyDepth, worldSeed);

	// Start from beginning.
	Reset();

	DLOG("Generated World: %u, Difficulty: %i", seed, m_difficulty);
}

void World::Render(dragon::RenderTarget& target)
{
	target.Draw(m_tilemap);

	for (Enemy* pEnemy : m_enemies)
	{
		pEnemy->Render(target);
	}

#if DRAGON_DEBUG
	// Draw noise tilemap if enabled.
	if(m_drawNoise)
		target.Draw(m_noiseTilemap);
#endif

	if(m_pCurrentRound)
		m_pCurrentRound->Render(target);

	//DrawPlayerInterface(target);
}

void World::Update(float dt)
{
	// Update Round
	if (m_pCurrentRound)
		m_pCurrentRound->Update(dt);

	// Add enemies.
	for (Enemy* pEnemy : m_enemiesToAdd)
	{
		m_enemies.emplace_back(pEnemy);
	}
	m_enemiesToAdd.clear();

	// Update Enemies
	for (Enemy* pEnemy : m_enemies)
	{
		pEnemy->Update(dt);
	}

	// Update Turrets on the field
	// Find Enemies nearby the turret. (Within range)

	// Delete enemies that have died.
	for (auto it = m_enemies.begin(); it != m_enemies.end();)
	{
		Enemy* pEnemy = *it;
		
		bool isDead = pEnemy->GetHealth() <= 0.0f;
		if (isDead)
		{
			delete pEnemy;
			it = m_enemies.erase(it);
		}
		else
		{
			++it;
		}

	}
}

void World::GenerateRound(const RoundInfo& info)
{
	dragon::Random roundRandom(info.m_roundSeed);

	// Create the round object.
	Round::RoundData roundData;
	roundData.m_seed = info.m_roundSeed;
	roundData.m_precipitation = info.m_precipitation;
	roundData.m_temperature = info.m_temperature;

	m_mapGenerator.SetTemperature(info.m_temperature);
	m_mapGenerator.SetPrecipitation(info.m_precipitation);

	m_pCurrentRound = new Round(roundData, this);
	m_pCurrentRound->SetDifficulty(m_difficulty);

	size_t spawnerCount = g_kSpawnerCountOnDifficulty[(size_t)m_difficulty].GetRandom(roundRandom);
	DLOG("Generating round with %u spawners.", spawnerCount);

	// Generate the map.
	MapGenerator::PossiblePositions positionsFound;
	positionsFound.reserve(g_kMaxTries);

	m_mapGenerator.Generate(m_tilemap, info.m_roundSeed);

	// Find a position to place the base at.
	m_mapGenerator.FindBestBasePosition(m_tilemap, positionsFound);
	assert(positionsFound.size() > 0);

	size_t randomIndex = roundRandom.RandomIndex(positionsFound.size());
	dragon::Vector2 basePosition = positionsFound[randomIndex];

	// Clear positions found array.
	positionsFound.clear();

	m_mapGenerator.FindEnemySpawnerLocations(m_tilemap, basePosition, positionsFound);
	assert(positionsFound.size() > 0);

	for (size_t i = 0; i < spawnerCount; ++i)
	{
		size_t randomIndex = roundRandom.RandomIndex(positionsFound.size());
		dragon::Vector2 spawnerPos = positionsFound[randomIndex];
		positionsFound.erase(positionsFound.begin() + randomIndex); // Remove so it can't be re-used.

		// Let the map generator carve a path to the base.
		Path spawnerPath = m_mapGenerator.CarvePath(m_tilemap, spawnerPos, basePosition);

		m_pCurrentRound->EmplaceSpawner(this, spawnerPos, eastl::move(spawnerPath));
	}

	m_mapGenerator.SetBaseTile(m_tilemap, basePosition);

	m_pCurrentRound->NextWave();
}

void World::NextRound(size_t option)
{
	// End the current round if not ended yet.
	if(!m_pCurrentRound->HasFinished())
		m_pCurrentRound->EndRound();

	ClearEnemies();

	//auto nextRounds = m_rounds.GetOutgoingEdges(m_currentRoundHandle);
}

void World::StartRound()
{
	if (m_pCurrentRound)
	{
		m_pCurrentRound->Resume();
	}
}

void World::PauseRound()
{
	if (m_pCurrentRound)
	{
		m_pCurrentRound->Pause();
	}
}

void World::ClearEnemies()
{
	for (Enemy* pEnemy : m_enemiesToAdd)
		delete pEnemy;
	m_enemiesToAdd.clear();

	for (Enemy* pEnemy : m_enemies)
		delete pEnemy;
	m_enemies.clear();
}
