#pragma once

// TODO: Probably not the best to be in Config.
//		 Also wasn't the best placement for Range I will be moving this in my engine in the future.
#include <Dragon/Generic/Random/Range.h>

// Globals

static constexpr float g_kTextSize = 21.0f;

static constexpr float g_kTileSize = 16.0f;
static constexpr size_t g_kMapSize = 45;

static constexpr float g_kMinTemperature = -10.0f;
static constexpr float g_kMaxTemperature = 30.0f;
static constexpr float g_kMinPrecipitation = 0.0f;
static constexpr float g_kMaxPrecipitation = 400.0f;

static constexpr size_t g_kWavesPerRound = 5;

static constexpr int g_kMaxTries = 25;
static constexpr int g_kMinDistanceOfSpawner = 16;

/// <summary>
/// Cost of buying a new turret.
/// </summary>
static constexpr float g_kTurretCost = 80.0f;

/// <summary>
/// Get **% back of the turret cost
/// </summary>
static constexpr float g_kTurretReturnValue = 0.6f; 

/// <summary>
/// Cost of an upgrade
/// </summary>
static constexpr float g_kTurretUpgradeCost = 20.0f;

/// <summary>
/// upgradeCost = upgradeLevel * g_kTurretUpgradeCost * multiplier;
/// </summary>
static constexpr float g_kTurretUpgradeCostMultiplier = 1.2f;

//
// Difficulty Settings
//

/// <summary>
/// Wave timers for the each difficulty in order.
/// </summary>
inline static constexpr float g_kWaveTimes[]
{
	60.f, // Easy
	45.f, // Normal
	30.f, // Hard
};

/// <summary>
/// Depth of the RoundGraph for difficulty.
/// </summary>
static constexpr dragon::Range<size_t> g_kDepthOnDifficulty[]
{
	dragon::Range<size_t>(5, 10), // Easy
	dragon::Range<size_t>(8, 14), // Normal
	dragon::Range<size_t>(15, 21), // Hard
};

/// <summary>
/// Range of spawner count on this difficulty.
/// </summary>
static constexpr dragon::Range<size_t> g_kSpawnerCountOnDifficulty[]
{
	dragon::Range<size_t>(1, 2), // Easy
	dragon::Range<size_t>(2, 3), // Normal
	dragon::Range<size_t>(2, 5), // Hard
};

/// <summary>
/// Starting gold for difficulty.
/// </summary>
static constexpr float g_kStarterGold[]
{
	200.0f,		// Easy
	150.0f,		// Normal
	100.0f		// Hard
};