#pragma once

enum struct GameDifficulty
{
	/// <summary>
	/// Turrets :
	///		- Effect Chance : High
	///		- Special Chance : High
	///		
	///	Enemies :
	///		- 2-5 Spawners per round
	///		- Decreased enemy health.
	///		- Decreased enemy speed.
	/// 
	/// World :
	///		- Depth : [5, 10]
	/// </summary>
	kEasy = 0,

	/// <summary>
	///	Enemies :
	///		- 2-3 Spawners per round
	/// 
	/// World :
	///		- Depth : [8, 14]
	/// </summary>
	kNormal = 1,

	/// <summary>
	/// Turrets :
	///		- Effect Chance : Low
	///		- Special Chance : Low
	///		
	///	Enemies :
	///		- 2-5 Spawners per round
	///		- Increased enemy health.
	///		- Increased enemy speed.
	/// 
	/// World :
	///		- Depth [ 15, 21 ]
	/// </summary>
	kHard = 2,

	kNone,
};