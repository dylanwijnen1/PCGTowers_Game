#pragma once

struct RoundInfo
{
	unsigned int m_roundSeed;
	float m_temperature;
	float m_precipitation;
};

/// <summary>
/// Generates a tree of rounds with different seeds and biomes that are somewhat similar.
/// </summary>
/// <grammar>
/// R => TRRR, 0.6
/// R => TRR, 0.4
/// 
/// T => g, 0.05
/// T => m, 0.2
/// T => n, 0.8
/// </grammar>
class RoundsGenerator
{
	void InitDefaults();

	void Generate(size_t maxDepth, unsigned int seed);
};