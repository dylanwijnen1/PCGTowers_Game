#pragma once

#include <Game/Generators/WaveGenerator.h>
#include <Game/Generators/MapGenerator.h>

#include <Game/Rounds/Round.h>

#include <Game/GameDifficulty.h>

#include <EASTL/vector.h>
#include <EASTL/array.h>
#include <EASTL/unordered_map.h>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>

namespace dragon
{
	class RenderTarget;
	class ApplicationEvent;

	class MouseButtonPressed;
	class MouseButtonReleased;
	class MouseMoved;
	class KeyReleased;
}

class World
{
	//
	// Generators
	//

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

	/// <summary>
	/// World Random, Seeded by GenerateWorld and Init
	/// </summary>
	dragon::Random m_random;

	/// <summary>
	/// Player Gold.
	/// </summary>
	float m_playerGold;

	/// <summary>
	/// Total Score
	/// </summary>
	float m_score;

	using Turrets = eastl::unordered_map<size_t, class Turret*>;
	Turrets m_turrets;

	/// <summary>
	/// Turrets that are currently on the game board.
	/// </summary>
	// Turrets m_turrets;

	GameDifficulty m_difficulty;

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

	//
	// User Interaction
	//

	class Turret* m_pMovingTurret;
	dragon::Vector2f m_lastMousePosition;

	//
	// User Interface
	//

	sf::Font m_font;

	/// <summary>
	/// Displays turret information.
	/// </summary>
	sf::Text m_turretInfoText;

	/// <summary>
	/// Displays round information : Wave Time, Wave Score
	/// </summary>
	sf::Text m_roundText;

	/// <summary>
	/// Displays Game Info : Total Score, Player Gold
	/// </summary>
	sf::Text m_gameText;

	/// <summary>
	/// Displays Games Key Binding Information and in debug it also shows information about tiles.
	/// </summary>
	sf::Text m_infoText;

	/// <summary>
	/// Displays the text whilst paused.
	/// </summary>
	sf::Text m_pauseText;

public:

	World()
		: m_difficulty(GameDifficulty::kNormal)
		, m_pCurrentRound(nullptr)
		, m_pDefaultWaveGenerator(nullptr)
		, m_pMovingTurret(nullptr)
		, m_playerGold(0.0f)
	{}

	~World();

	/// <summary>
	/// Initialize the world
	/// </summary>
	bool Init();

	/// <summary>
	/// Reset the world to start from the beginning.
	/// </summary>
	void Reset();

	/// <summary>
	/// Generates the Game World with a randomized seed.
	/// Also resets the game world.
	/// </summary>
	void GenerateWorld();

	/// <summary>
	/// Generates the Game World based on seed and difficulty.
	/// Also resets the game world.
	/// </summary>
	void GenerateWorld(unsigned int seed);

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

	void OnEvent(dragon::ApplicationEvent& ev);

	/// <summary>
	/// Renders the State of the World to the screen.
	/// </summary>
	/// <param name="target"></param>
	void Render(dragon::RenderTarget& target);

	void Update(float dt);

private:

	void GenerateRound(const Round::RoundData& roundData);

	bool IsTurretPlaceable(class Turret* pTurret);
	bool TryPlaceTurret(size_t tileIndex, class Turret* pTurret);

	void BuyTurret(size_t index);
	void SellTurret(size_t index);
	void UpgradeTurret(size_t index);
	Turret* GenerateTurret();

	void UpdateEnemies(float dt);
	void UpdateTurrets(float dt);

	void DrawEnemies(dragon::RenderTarget& target);
	void DrawTurretsAndCursor(dragon::RenderTarget& target);
	void DrawTurretInformation(dragon::RenderTarget& target, class Turret* pTurret);

	void DrawPlacementSquare(dragon::RenderTarget& target, dragon::Vector2 tilePos, dragon::Color color) const;

	void InitializeUserInterface();
	void UpdateInfoText();
	void UpdateGameText();
	void UpdateRoundText();
	void DrawUserInterface(dragon::RenderTarget& target);

#pragma region User Interactions

	void HandleMousePress(dragon::MouseButtonPressed& ev);
	void HandleMouseRelease(dragon::MouseButtonReleased& ev);
	void HandleMouseMove(dragon::MouseMoved& ev);
	void HandleKeyRelease(dragon::KeyReleased& ev);

#pragma endregion

#pragma region Round Utilities

private:

	/// <summary>
	/// Generates the next round.
	/// </summary>
	void NextRound();

	/// <summary>
	/// Tell the round to start spawning waves of enemies or to stop.
	/// </summary>
	void TogglePauseRound();

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