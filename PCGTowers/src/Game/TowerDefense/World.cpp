#include "World.h"

#include <Config.h>

#include <Game/TowerDefense/Turret.h>
#include <Game/TowerDefense/Enemy.h>
#include <Game/TowerDefense/Spawner.h>
#include <Game/Rounds/Round.h>

#include <Dragon/Application/Application.h>
#include <Dragon/Graphics/RenderTarget.h>
#include <Dragon/Application/Window/WindowEvents.h>

#include <SFML/Graphics.hpp>

#include <iostream>

static constexpr dragon::Color g_kTurretRangeColor = dragon::Colors::Black;
static constexpr dragon::Color g_kTurretPlaceableColor = dragon::Colors::LightGreen;
static constexpr dragon::Color g_kTurretNotPlaceableColor = dragon::Colors::OrangeRed;
static constexpr dragon::Color g_kMouseTileColor = dragon::Colors::RebeccaPurple;
static constexpr float g_kTranslucencyValue = 0.4f;
static constexpr float g_kOutlineSize = 1.0f;

World::~World()
{
	m_pCurrentRound->Pause();
	ClearEnemies();

	delete m_pDefaultWaveGenerator;
	delete m_pCurrentRound;
}

bool World::Init()
{
	if (!m_font.loadFromFile("retro_gaming.ttf"))
		return false;

	m_random.Seed(time(0));

	m_tilemap.Init({ g_kMapSize, g_kMapSize }, { g_kTileSize, g_kTileSize });
	m_tilemap.LoadTileset("tileset.png");

	m_mapGenerator.Init();

	m_pDefaultWaveGenerator = new WaveGenerator();
	m_pDefaultWaveGenerator->InitDefaults();

	InitializeUserInterface();

	return true;
}

void World::Reset()
{
	m_playerGold = g_kStarterGold[(size_t)m_difficulty];
	m_score = 0.0f;
}

void World::GenerateWorld()
{
	unsigned int seed = m_random.Random<unsigned int>();
	GenerateWorld(seed);
}

void World::NextRound()
{
	if (m_pCurrentRound)
	{
		m_score += m_pCurrentRound->GetRoundScore();

		ClearEnemies();
		delete m_pCurrentRound;
	}

	// Generate random round information.
	Round::RoundData data;
	data.m_seed = m_random.Random<unsigned int>();
	data.m_temperature = m_random.RandomRange(g_kMinTemperature, g_kMaxTemperature);
	data.m_precipitation = m_random.RandomRange(g_kMinPrecipitation, g_kMaxPrecipitation);

	GenerateRound(data);

	// Disable turrets that shouldn't be active anymore due to change in round.
	for (auto pair : m_turrets)
	{
		if (!IsTurretPlaceable(pair.second))
			pair.second->Disable();
	}
}

void World::GenerateWorld(unsigned int seed)
{
	// Allows for seed to be 0.
	unsigned int worldSeed = dragon::SquirrelNoise::Get1DNoise(1, seed);

	m_random.Seed(worldSeed);

	// Get the max depth for this Game's RoundGraph based on difficulty.
	size_t difficultyDepth = g_kDepthOnDifficulty[(size_t)m_difficulty].GetRandom(m_random);

	// Resets the player 
	Reset();

	// Start and Generate Round.
	NextRound();

	DLOG("Generated World: %u, Difficulty: %i", seed, m_difficulty);
}

void World::OnEvent(dragon::ApplicationEvent& ev)
{
	ev.Dispatch<dragon::KeyReleased>(this, &World::HandleKeyRelease);
	ev.Dispatch<dragon::MouseButtonPressed>(this, &World::HandleMousePress);
	ev.Dispatch<dragon::MouseButtonReleased>(this, &World::HandleMouseRelease);
	ev.Dispatch<dragon::MouseMoved>(this, &World::HandleMouseMove);
}

void World::Render(dragon::RenderTarget& target)
{
	target.Draw(m_tilemap);

	DrawEnemies(target);

	DrawTurretsAndCursor(target);	

	if(m_pCurrentRound)
		m_pCurrentRound->Render(target);

	DrawUserInterface(target);
}

void World::Update(float dt)
{

#if _DEBUG
	// Only necessary in debug mode.
	UpdateInfoText();
#endif

	UpdateGameText();
	UpdateRoundText();

	// Update Round
	if (m_pCurrentRound)
	{
		m_pCurrentRound->Update(dt);

		// Finish the round up if player has killed all the enemies and generate a new round.
		if (m_pCurrentRound->HasFinished() && m_enemies.size() == 0)
		{
			NextRound();
		}
	}

	UpdateTurrets(dt);
	UpdateEnemies(dt);
}

void World::GenerateRound(const Round::RoundData& roundData)
{
	dragon::Random roundRandom(roundData.m_seed);

	m_mapGenerator.SetTemperature(roundData.m_temperature);
	m_mapGenerator.SetPrecipitation(roundData.m_precipitation);

	m_pCurrentRound = new Round(roundData, this);
	m_pCurrentRound->SetDifficulty(m_difficulty);

	size_t spawnerCount = g_kSpawnerCountOnDifficulty[(size_t)m_difficulty].GetRandom(roundRandom);
	DLOG("Generating round with %u spawners.", spawnerCount);

	// Generate the map.
	MapGenerator::PossiblePositions positionsFound;
	positionsFound.reserve(g_kMaxTries);

	m_mapGenerator.Generate(m_tilemap, roundData.m_seed);

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
}

bool World::IsTurretPlaceable(Turret* pTurret)
{
	dragon::Vector2 tilePosition = m_tilemap.WorldToMapCoordinates(pTurret->GetPosition());
	auto tileData = m_tilemap.GetTileData(tilePosition.x, tilePosition.y);
	return tileData.m_isTurretPlaceable;
}

bool World::TryPlaceTurret(size_t tileIndex, Turret* pTurret)
{
	// Place the turret if possible
	if (auto result = m_turrets.find(tileIndex); result == m_turrets.end())
	{
		dragon::Vector2 tilePosition = m_tilemap.PositionFromIndex((int)tileIndex);
		dragon::Vector2f centroid =
		{
			(float)tilePosition.x * g_kTileSize + (g_kTileSize / 2.0f),
			(float)tilePosition.y * g_kTileSize + (g_kTileSize / 2.0f),
		};

		// Insert into the map.
		m_turrets[tileIndex] = pTurret;
		
		// Set Turret Position to tile position
		pTurret->SetPosition(centroid);

		// Enable the turret
		if (IsTurretPlaceable(pTurret))
			pTurret->Enable();
		else
			pTurret->Disable();

		// Clear the turret target so it can start looking for a new target.
		pTurret->ClearTarget();

		return true;
	}
	else
	{
		// Disable the turret
		pTurret->Disable();
		return false;
	}
}

void World::BuyTurret(size_t index)
{
	// Only buy turret if enough gold.
	if (m_playerGold < g_kTurretCost)
		return;

	// Buy a new turret and place under mouse cursor if possible.
	if (m_tilemap.GetTileDataAtIndex(index).m_isTurretPlaceable)
	{
		if (TryPlaceTurret(index, GenerateTurret()))
		{
			// Only subtract gold if buying was successful.
			m_playerGold -= g_kTurretCost;
		}
	}
}

void World::SellTurret(size_t index)
{
	if (auto result = m_turrets.find(index); result != m_turrets.end())
	{
		Turret* pSellingTurret = result->second;
		// Increase player gold.
		m_playerGold += pSellingTurret->GetResaleValue();

		// Remove from turrets and delete the memory.
		delete pSellingTurret;
		m_turrets.erase(result);
	}
}

void World::UpgradeTurret(size_t index)
{
	if (auto result = m_turrets.find(index); result != m_turrets.end())
	{
		float cost = result->second->GetUpgradeCost();
		if (cost <= m_playerGold)
		{
			m_playerGold -= cost;
			result->second->Upgrade();
		}
	}
}

Turret* World::GenerateTurret()
{
	Turret* pTurret = new Turret();

	pTurret->SetDamage(10.0f);
	pTurret->SetRange(100.0f);
	pTurret->SetCooldown(1.f);

	return pTurret;
}

void World::UpdateEnemies(float dt)
{
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

	// Delete enemies that have died.
	for (auto it = m_enemies.begin(); it != m_enemies.end();)
	{
		Enemy* pEnemy = *it;

		bool isDead = pEnemy->GetHealth() <= 0.0f;
		if (isDead)
		{
			// Increase player gold. Based on the damage they would've done to the base.
			m_playerGold += pEnemy->GetStats().m_damage;

			delete pEnemy;
			it = m_enemies.erase(it);
		}
		else
		{
			++it;
		}

	}
}

void World::UpdateTurrets(float dt)
{
	// Find Turret Targets and Update
	for (auto& pair : m_turrets)
	{
		pair.second->Update(dt);
		pair.second->FindTarget(m_enemies);
	}
}

void World::DrawEnemies(dragon::RenderTarget& target)
{
	for (Enemy* pEnemy : m_enemies)
	{
		pEnemy->Render(target);
	}
}

void World::DrawTurretsAndCursor(dragon::RenderTarget& target)
{
	dragon::Vector2 mouseTilePosition = m_tilemap.WorldToMapCoordinates(m_lastMousePosition);

	// Draw Turrets
	for (auto& pair : m_turrets)
	{
		pair.second->Render(target);

		dragon::Vector2 turretTilePosition = m_tilemap.WorldToMapCoordinates(pair.second->GetPosition());

		if (!IsTurretPlaceable(pair.second))
		{
			// Draw Red Square underneath turret
			DrawPlacementSquare(target, turretTilePosition, g_kTurretNotPlaceableColor);
		}

		// Check if we are currently hovering over the turret.
		if (turretTilePosition == mouseTilePosition)
		{
			DrawTurretInformation(target, pair.second);
		}

	}

	// Draw currently dragged turret
	if (m_pMovingTurret)
	{
		m_pMovingTurret->Render(target);

		// Draw square if the player could place this turret or not.
		if (IsTurretPlaceable(m_pMovingTurret))
		{
			DrawPlacementSquare(target, mouseTilePosition, g_kTurretPlaceableColor);
		}
		else
		{
			DrawPlacementSquare(target, mouseTilePosition, g_kTurretNotPlaceableColor);
		}
	}
	else
	{
		// Only draw if we're not dragging a turret around.
		DrawPlacementSquare(target, mouseTilePosition, g_kMouseTileColor);
	}
}

void World::DrawTurretInformation(dragon::RenderTarget& target, Turret* pTurret)
{
	// Draw Range of turret.
	dragon::Color translucent = g_kTurretRangeColor;
	translucent.a = g_kTranslucencyValue;
	target.DrawFillCircle(pTurret->GetPosition() - pTurret->GetRange(), pTurret->GetRange(), translucent, g_kTurretRangeColor, g_kOutlineSize);

	// Show Turret Stats in InfoText.
	m_turretInfoText.setString
	(
		"Upgrade Level : " + std::to_string(pTurret->GetUpgradeLevel()) +
		"\nUpgrade Cost : " + std::to_string((unsigned int)pTurret->GetUpgradeCost()) +
		"\nResale Value : " + std::to_string((unsigned int)pTurret->GetResaleValue())
	);

	auto bounds = m_turretInfoText.getLocalBounds();
	m_turretInfoText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

	sf::RenderTarget* pSfTarget = target.GetNativeTarget<sf::RenderTarget*>();
	pSfTarget->draw(m_turretInfoText);

}

void World::DrawPlacementSquare(dragon::RenderTarget& target, dragon::Vector2 tilePos, dragon::Color color) const
{
	dragon::RectF tileRect =
	{
		(float)tilePos.x * g_kTileSize,
		(float)tilePos.y * g_kTileSize,
		g_kTileSize,
		g_kTileSize
	};

	dragon::Color translucent = color;
	translucent.a = g_kTranslucencyValue;
	target.DrawFillRect(tileRect, translucent, color, g_kOutlineSize);
}

void World::InitializeUserInterface()
{
	static constexpr float kGameSize = g_kTileSize * g_kMapSize;

	auto applyStyle = [this](sf::Text& text) 
	{
		text.setFont(m_font);
		text.setCharacterSize((unsigned int)g_kTextSize);

		text.setFillColor(sf::Color::White);
		text.setOutlineColor(sf::Color::Black);
		text.setOutlineThickness(g_kTextSize / 10.0f);
	};

	// Pause Text, Center of the screen.
	{
		applyStyle(m_pauseText);
		m_pauseText.setString("Holding Next Wave - Press Enter to Resume/Pause");

		auto bounds = m_pauseText.getLocalBounds();
		m_pauseText.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f); // Center Origin

		m_pauseText.setPosition(kGameSize / 2.0f, kGameSize / 2.0f);
	}

	// Info Text, On the side of the game.
	applyStyle(m_infoText);
	m_infoText.setPosition(g_kTileSize * g_kMapSize, 0.0f);

	// Update once, Since this isn't updated all the time.
	UpdateInfoText();

	// Round Information, Center top of screen, High Contrast Color.
	applyStyle(m_roundText);
	m_roundText.setPosition(kGameSize / 2.0f, g_kTextSize);

	// Game Information, Right Top of Screen, High Contract color
	applyStyle(m_gameText);

	m_gameText.setPosition(kGameSize, 0.0f);

	// Turret Info
	size_t turretInfoLines = 3;
	applyStyle(m_turretInfoText);
	m_turretInfoText.setPosition(kGameSize / 2.0f, (kGameSize / 2.0f) - (g_kTextSize * turretInfoLines));
	m_turretInfoText.setFillColor(sf::Color::Yellow);
}

void World::UpdateInfoText()
{
#if _DEBUG
	dragon::Vector2 mouseTilePosition = m_tilemap.WorldToMapCoordinates(m_lastMousePosition);
	size_t tileIndex = m_tilemap.IndexFromPosition(mouseTilePosition);
	auto tileData = m_tilemap.GetTileDataAtIndex(tileIndex);
#endif

	std::string text =
		"Turret at mouse cursor:\n\n"
		"B - Buy Turret " + std::to_string(g_kTurretCost) + " Gold.\n"
		"S - Sell Turret\n"
		"U - Upgrade Turret\n\n"
		"Click & Drag turret to move around the map."
		"\nCheats:\n"
		"G - Give Gold (1000)\n"
		"N - Next Round\n"
		"W - Seed World (Console Input)\n"
		"K - Kill All Enemies\n"
#if _DEBUG
		"\nDebug Info: \n"
		"Tile Position: (" + std::to_string(mouseTilePosition.x) + ", " + std::to_string(mouseTilePosition.y) + ")\n"
		"Tile Index: " + std::to_string(tileIndex) + "\n"
		"Tile Data:\n - Noise: " + std::to_string(tileData.m_noise) +
		"\n - Temperature: " + std::to_string(tileData.m_temperature) +
		"\n - Moisture: " + std::to_string(tileData.m_moistureLevel)
#endif
		; // DYLAN: This is really weird syntax, I'll never do this again...

	m_infoText.setString(text);
}

void World::UpdateGameText()
{
	std::string text =
		"Gold: " + std::to_string((unsigned int)m_playerGold) + "\n"
		"Score: " + std::to_string((unsigned int)m_score) + "\n";

	m_gameText.setString(text);
	auto bounds = m_gameText.getLocalBounds();
	m_gameText.setOrigin(bounds.width, 0.0f);
}

void World::UpdateRoundText()
{
	if (m_pCurrentRound)
	{
		std::string text = std::to_string((int)m_pCurrentRound->GetWaveTime());
		
		m_roundText.setString(text);
		auto bounds = m_roundText.getLocalBounds();
		m_roundText.setOrigin(bounds.width / 2.0f, 0.0f);
	}
}

void World::DrawUserInterface(dragon::RenderTarget& target)
{
	sf::RenderTarget* pSfTarget = target.GetNativeTarget<sf::RenderTarget*>();
	pSfTarget->draw(m_infoText);
	pSfTarget->draw(m_gameText);
	pSfTarget->draw(m_roundText);

	if (m_pCurrentRound->IsPaused())
		pSfTarget->draw(m_pauseText);
}

void World::HandleMousePress(dragon::MouseButtonPressed& ev)
{
	// Find turret underneath the last mouse position. (Clamped to tilemap coords)
	dragon::Vector2 tilePosition = m_tilemap.WorldToMapCoordinates(m_lastMousePosition);
	size_t index = m_tilemap.IndexFromPosition(tilePosition);

	if (auto result = m_turrets.find(index); result != m_turrets.end())
	{
		m_pMovingTurret = result->second;
		// Remove from the list. This stops it from shooting and other things that a placed turret would do.
		m_turrets.erase(result);
	}

}

void World::HandleMouseRelease(dragon::MouseButtonReleased& ev)
{
	if (m_pMovingTurret)
	{
		dragon::Vector2 tilePosition = m_tilemap.WorldToMapCoordinates(m_lastMousePosition);
		size_t index = m_tilemap.IndexFromPosition(tilePosition);

		TryPlaceTurret(index, m_pMovingTurret);
		m_pMovingTurret = nullptr;
	}
}

void World::HandleMouseMove(dragon::MouseMoved& ev)
{
	// Keep mouse within tilemap coords!
	dragon::Vector2 tilePosition = m_tilemap.WorldToMapCoordinates(ev.m_position);
	if(m_tilemap.WithinBounds(tilePosition))
		m_lastMousePosition = ev.m_position;

	if (m_pMovingTurret)
	{
		m_pMovingTurret->SetPosition(m_lastMousePosition);
	}
}

void World::HandleKeyRelease(dragon::KeyReleased& ev)
{
	dragon::Vector2 tilePosition = m_tilemap.WorldToMapCoordinates(m_lastMousePosition);
	size_t index = m_tilemap.IndexFromPosition(tilePosition);

	if (ev.m_keyCode == dragon::Key::B)
	{
		BuyTurret(index);
	}
	else if (ev.m_keyCode == dragon::Key::S)
	{
		SellTurret(index);
	}
	else if (ev.m_keyCode == dragon::Key::U)
	{
		UpgradeTurret(index);
	}

	if (ev.m_keyCode == dragon::Key::Enter)
	{
		TogglePauseRound();
	}

	// Cheats
	if (ev.m_keyCode == dragon::Key::G)
	{
		m_playerGold += 1000.0f;
	}
	else if(ev.m_keyCode == dragon::Key::N)
	{
		NextRound();
	}
	else if (ev.m_keyCode == dragon::Key::W)
	{
		std::cout << "Enter a seed: ";

		std::hash<std::string> hasher;

		std::string input;
		std::getline(std::cin, input);
		GenerateWorld((unsigned int)hasher(input));

		std::cout << std::endl;
	}
	else if (ev.m_keyCode == dragon::Key::K)
	{
		ClearEnemies();
	}
}

void World::TogglePauseRound()
{
	if (m_pCurrentRound)
	{
		if (m_pCurrentRound->IsPaused())
			m_pCurrentRound->Resume();
		else
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

	for (auto pair : m_turrets)
	{
		pair.second->ClearTarget();
	}
}
