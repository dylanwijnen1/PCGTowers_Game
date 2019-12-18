#pragma once

#include <Game/WeightedGrammarSystem.h>
#include <EASTL/vector.h>
#include <EASTL/stack.h>

class Enemy;

/// <summary>
/// Generate waves using a Grammar System.
/// </summary>
/// <grammar>
/// 
/// Rules : 
///		
///		S => G G G
///		S => G G
///		S => G
/// 
/// 	G => e e e e e
///		G => e e e e
///		G => e e e
///		G => e e
/// 
///		G => [TG]
/// 
///		T => t
///		T => s
///		T => g
/// 
/// Terminating Symbols:
/// 
///		[ => Push Enemy Type
///		] => Pop Enemy Type
/// 
///		e => Enemy
///		t => Tank Type
///		s => Speed Type
///		g => Generic Type
///		r => Random Type
/// 
/// </grammar>
class WaveGenerator
{
	using Spawners = eastl::vector<class Spawner>;

	WeightedGrammarSystem m_waveGrammarSystem;

	char m_enemyType;

	dragon::Random m_random;

public:

	virtual void InitDefaults();

	/// <summary>
	/// Generates the waves using the rules that are setup.
	/// </summary>
	/// <param name="spawners"></param>
	/// <param name="seed"></param>
	/// <param name="currentWave"></param>
	virtual void GenerateWaves(Spawners& spawners, unsigned int seed, unsigned int currentWave);

	// Add a rule to the system.
	void AddRule(char symbol, const WeightedGrammarSystem::Rule& rule) { m_waveGrammarSystem.AddRule(symbol, rule); }

private:

	virtual void ProcessNode(WeightedGrammarSystem::RuleNode* pNode, Spawner& pSpawner);

	virtual Enemy* CreateEnemy(char enemyType);

	virtual void CreateTank(Enemy* pEnemy);
	virtual void CreateSpeedy(Enemy* pEnemy);
	virtual void CreateGeneric(Enemy* pEnemy);
	virtual void CreateRandom(Enemy* pEnemy);
};