#include "WaveGenerator.h"

#include <Config.h>

#include <Game/TowerDefense/Spawner.h>
#include <Game/TowerDefense/Enemy.h>

#include <cassert>

void WaveGenerator::InitDefaults()
{
	m_waveGrammarSystem.AddRule('S', { "TGGG",	.25f });
	m_waveGrammarSystem.AddRule('S', { "TGG",	.5f });
	m_waveGrammarSystem.AddRule('S', { "TG",		.25f });

	m_waveGrammarSystem.AddRule('G', { "eeeee",	.15f });
	m_waveGrammarSystem.AddRule('G', { "eeee",	.4f });
	m_waveGrammarSystem.AddRule('G', { "eee",	.3f });
	m_waveGrammarSystem.AddRule('G', { "ee",	.15f });

	// Switches type of enemy
	m_waveGrammarSystem.AddRule('G', { "TG",	.5f });
	m_waveGrammarSystem.AddRule('G', { "TGG",	.1f });

	m_waveGrammarSystem.AddRule('T', { "t",	.3f });
	m_waveGrammarSystem.AddRule('T', { "s",	.3f });
	m_waveGrammarSystem.AddRule('T', { "g",	.3f });
	m_waveGrammarSystem.AddRule('T', { "r",	.1f });

}

void WaveGenerator::GenerateWaves(Spawners& spawners, unsigned int seed, unsigned int currentWave)
{
	unsigned int waveSeed = seed + (currentWave * 2361103u);

	m_random.Seed(waveSeed);
	m_waveGrammarSystem.SetSeed(waveSeed);

	for (auto& spawner : spawners)
	{
		auto pRoot = m_waveGrammarSystem.RunGrammar('S');

		// Process Nodes
		ProcessNode(pRoot, spawner);
	}
}

void WaveGenerator::ProcessNode(WeightedGrammarSystem::RuleNode* pNode, Spawner& pSpawner)
{
	// Group of Enemies
	if (pNode->m_symbol == 'G')
	{
		eastl::queue<Enemy*> enemyGroup;

		for (auto pChild : pNode->m_children)
		{
			Enemy* pEnemy = CreateEnemy(m_enemyType);
			enemyGroup.emplace_back(pEnemy);
		}

		pSpawner.EmplaceEnemyGroup(eastl::move(enemyGroup));
	}
	else if (pNode->m_symbol == 'T')
	{
		assert(pNode->m_children.size() > 0);

		m_enemyType = pNode->m_children[0]->m_symbol;
	}
	else
	{
		// Continue processing.
		for (auto pChild : pNode->m_children)
			ProcessNode(pChild, pSpawner);
	}
}

Enemy* WaveGenerator::CreateEnemy(char enemyType)
{
	Enemy* pEnemy = new Enemy();

	switch (enemyType)
	{
	case 't':
		// Tank
		CreateTank(pEnemy);
		break;
	case 's':
		// Speedy
		CreateSpeedy(pEnemy);
		break;
		// Generic
	case 'g':
		CreateGeneric(pEnemy);
		break;
	case 'r':
		CreateRandom(pEnemy);
		break;
		// Randomized Attributes.
	}

	return pEnemy;
}

void WaveGenerator::CreateTank(Enemy* pEnemy)
{
	static constexpr dragon::Range<float> kTankHealthRange(100.0f, 200.0f);

	Enemy::Stats stats;
	stats.m_maxHealth = kTankHealthRange.GetRandom(m_random);
	stats.m_speed = g_kTileSize / 2.0f; // 4x Slower than generic.
	stats.m_damage = 2.0f; // 2x Damage than generic.

	pEnemy->SetStats(stats);
	pEnemy->SetShape(Enemy::Shape::kSquare);
	pEnemy->SetColor(dragon::Colors::Cyan);
}

void WaveGenerator::CreateSpeedy(Enemy* pEnemy)
{
	static constexpr dragon::Range<float> kSpeedRange(g_kTileSize, g_kTileSize * 2.0f);

	Enemy::Stats stats;
	stats.m_maxHealth = 20.0f; // Very Low health.
	stats.m_speed = kSpeedRange.GetRandom(m_random);
	stats.m_damage = 0.5f; // Low damage.

	pEnemy->SetStats(stats);
	pEnemy->SetShape(Enemy::Shape::kTriangle);
	pEnemy->SetColor(dragon::Colors::LightYellow);
}

void WaveGenerator::CreateGeneric(Enemy* pEnemy)
{
	static constexpr dragon::Range<float> kHealthRange(60.0f, 100.0f);

	Enemy::Stats stats;
	stats.m_maxHealth = kHealthRange.GetRandom(m_random);
	stats.m_speed = g_kTileSize;
	stats.m_damage = 1.0f;

	pEnemy->SetStats(stats);
	pEnemy->SetShape(Enemy::Shape::kCircle);
	pEnemy->SetColor(dragon::Colors::Red);
}

void WaveGenerator::CreateRandom(Enemy* pEnemy)
{
	static constexpr dragon::Range<float> kHealthRange(10.0f, 160.0f);
	static constexpr dragon::Range<float> kSpeedRange(g_kTileSize / 4.0f, g_kTileSize * 1.5f);
	static constexpr dragon::Range<float> kDamageRange(0.5f, 3.0f);

	Enemy::Stats stats;
	stats.m_maxHealth = kHealthRange.GetRandom(m_random);
	stats.m_speed = kSpeedRange.GetRandom(m_random);
	stats.m_damage = kDamageRange.GetRandom(m_random);

	pEnemy->SetStats(stats);
	pEnemy->SetShape((Enemy::Shape)m_random.RandomRange<size_t>((size_t)Enemy::Shape::kCircle, (size_t)Enemy::Shape::kTriangle));

	// Get a reasonable color using HSV
	dragon::Color randomColor = dragon::Color::FromHSV(m_random.RandomRange(0.0f, 360.0f), .5f, 1.0f);
	pEnemy->SetColor(randomColor);
}
