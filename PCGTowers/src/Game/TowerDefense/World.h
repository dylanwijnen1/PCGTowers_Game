#pragma once

#include <EASTL/vector.h>

#include <Game/Generators/TurretGenerator.h>
#include <Game/Generators/WaveGenerator.h>
#include <Game/Generators/MapGenerator.h>
#include <Game/Generators/RoundsGenerator.h>

#include <Game/GameDifficulty.h>

namespace dragon
{
	class RenderTarget;
}

class World
{


	//
	// Generators
	//

	/// <summary>
	/// Generates the world rounds for the player to play through.
	/// </summary>
	//RoundsGenerator m_roundsGenerator;

	/// <summary>
	/// Generates turrets for the player and during the game.
	/// </summary>
	TurretGenerator m_turretGenerator;

	/// <summary>
	/// The map generator of the world.
	/// TODO: Possibly should be changeable just like the wave generator.
	/// </summary>
	MapGenerator m_mapGenerator;

	/// <summary>
	/// The default wave generator.
	/// </summary>
	WaveGenerator* m_pDefaultWaveGenerator;

	// Game State
	TDTilemap m_tilemap;

#if _DEBUG

	TDTilemap m_noiseTilemap; // Only used in debugging.

#endif

	/// <summary>
	/// World Generator.
	/// </summary>
	dragon::Random m_random;

	struct Player
	{
		float m_gold;
		// eastl::array<Turret*, 5> m_inventory; // Max 5 in the inventory.

		Player()
			: m_gold(0.0f)
		{}

	} m_player; 

	/// <summary>
	/// Turrets that are currently on the game board.
	/// </summary>
	// Turrets m_turrets;

	GameDifficulty m_difficulty;

	//dragon::Handle m_currentRoundHandle;
	class Round* m_pCurrentRound;

	using Enemies = eastl::vector<class Enemy*>;

	/// <summary>
	/// Enemies that are currently on the playing field.
	/// </summary>
	Enemies m_enemies;

	/// <summary>
	/// Enemies that have to be added.
	/// </summary>
	Enemies m_enemiesToAdd;

	// Events:
	// Event<void(Round*)> OnNextRoundSelected;

public:

	World()
		: m_difficulty(GameDifficulty::kNormal)
		, m_pCurrentRound(nullptr)
		, m_pDefaultWaveGenerator(nullptr)
		, m_player()
	{}

	~World();

	/// <summary>
	/// Initialize the world
	/// </summary>
	void Init();

	/// <summary>
	/// Reset the world to start from the beginning.
	/// </summary>
	void Reset();

	/// <summary>
	/// Generates the Game World based on seed and difficulty.
	/// Also resets the game world.
	/// </summary>
	void GenerateWorld(unsigned int seed = (unsigned int)time(0));

	/// <summary>
	/// Sets the world's difficulty level.
	/// </summary>
	/// <param name="difficulty"></param>
	void SetDifficulty(GameDifficulty difficulty) { m_difficulty = difficulty; }

	/// <summary>
	/// Gets the default wave generator for the world.
	/// </summary>
	/// <returns></returns>
	WaveGenerator* GetDefaultWaveGenerator() { return m_pDefaultWaveGenerator; }

	/// <summary>
	/// Sets the default wave generator for the world.
	/// </summary>
	/// <param name="pGenerator"></param>
	void SetDefaultWaveGenerator(WaveGenerator* pGenerator) { m_pDefaultWaveGenerator = pGenerator; }

	// OnEvent(ApplicationEvent& event);
	// ChooseNextRound(dragon::Handle); // Fires OnNextRoundSelected.

	/// <summary>
	/// Renders the State of the World to the screen.
	/// </summary>
	/// <param name="target"></param>
	void Render(dragon::RenderTarget& target);

	void Update(float dt);

private:

	void GenerateRound(const RoundInfo& roundInfo);

#pragma region Round Utilities

private:

	/// <summary>
	/// Generates the next round.
	/// </summary>
	void NextRound(size_t option);

	/// <summary>
	/// Tell the round to start spawning waves of enemies.
	/// </summary>
	void StartRound();
	void PauseRound();

	/// <summary>
	/// Gets rid of all enemies in the world or were yet to be added.
	/// </summary>
	void ClearEnemies();

public:
	/// <summary>
	/// Adds an enemy to the world.
	/// </summary>
	/// <param name="pEnemy"></param>
	void AddEnemy(class Enemy* pEnemy) { m_enemiesToAdd.emplace_back(pEnemy); }

#pragma endregion


};