#pragma once

#include <EASTL/unordered_map.h>
#include <Game/WeightedGrammarSystem.h>

struct TurretGeneratorTuning
{
	float m_effectChance;
	float m_specialChance;
};

/// <summary>
/// 
/// </summary>
/// <grammar>
/// 
/// Rules : 
///		
///		Temperature Effect (Increase/Decrease)
///		Special (When maxed, Gain this bonus special) Must be mapped for tower types. SpecialMultiMap<TowerType, { SpecialFunctor, chance }>
///		Effect (Affects tiles and towers on them to do something interesting)
/// 
/// Terminating Symbols:
/// 
/// 
/// 
/// </grammar>
class TurretGenerator
{
	WeightedGrammarSystem m_turretGenerator;

public:
	bool Init();
};