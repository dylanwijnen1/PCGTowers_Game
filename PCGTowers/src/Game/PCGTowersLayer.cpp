#include "PCGTowersLayer.h"

#include <SFML/Graphics.hpp>

PCGTowersLayer::~PCGTowersLayer()
{
}

void PCGTowersLayer::OnAttach()
{
	m_font.loadFromFile("retro_gaming.ttf");

	m_world.Init();
	m_world.SetDifficulty(GameDifficulty::kNormal);

	// Generate World { 0 }
	m_world.GenerateWorld(1576697224);
	//m_world.GenerateWorld(1576698459);
}

void PCGTowersLayer::OnDetach()
{

}

void PCGTowersLayer::Update(float dt)
{
	m_world.Update(dt);
}

void PCGTowersLayer::FixedUpdate(float dt)
{

}

void PCGTowersLayer::Render(dragon::RenderTarget& target)
{
	m_world.Render(target);

	//dragon::Vector2 mouseTilePos = tilemap.WorldToMapCoordinates(m_mousePosition);
	//dragon::RectF mouseTileRect =
	//{
	//	(float)mouseTilePos.x * m_kTileSize,
	//	(float)mouseTilePos.y * m_kTileSize,
	//	m_kTileSize,
	//	m_kTileSize
	//};

	//target.DrawRect(mouseTileRect, dragon::Colors::Red);

	//const TDTileData& tileData = tilemap.GetTileData(mouseTilePos.x, mouseTilePos.y);

	//std::string tileInfo =
	//	"Mouse Pos: (" + std::to_string(mouseTilePos.x) + ", " + std::to_string(mouseTilePos.y) + ");" +
	//	"\nTile Influence: (" + std::to_string(tileData.m_temp.x) + ", " + std::to_string(tileData.m_temp.y) + ");" +
	//	"\nNoise: " + std::to_string(tileData.m_noise) +
	//	"\nPersistance: " + std::to_string(m_mapGenerator.m_persistance) +
	//	"\nZoom: " + std::to_string(m_mapGenerator.m_zoom) +
	//	"\nOctaves: " + std::to_string(m_mapGenerator.m_octaves);

	//sf::Text infoText(tileInfo, m_font);
	//infoText.setPosition(45 * 16, 0);
	//infoText.setColor(sf::Color::Red);

	//sf::RenderTarget* pSfTarget = static_cast<sf::RenderTarget*>(target.GetNativeTarget());
	//pSfTarget->draw(infoText);

}

void PCGTowersLayer::OnEvent(dragon::ApplicationEvent& ev)
{
	ev.Dispatch<dragon::KeyPressed>(this, &PCGTowersLayer::HandleKeyPressed);
	ev.Dispatch<dragon::KeyReleased>(this, &PCGTowersLayer::HandleKeyReleased);
	ev.Dispatch<dragon::MouseScrolled>(this, &PCGTowersLayer::HandleMouseScroll);
	ev.Dispatch<dragon::MouseMoved>(this, &PCGTowersLayer::HandleMouseMoved);
}

void PCGTowersLayer::HandleKeyPressed(dragon::KeyPressed& keyEvent)
{
	m_world.GenerateWorld();

	//dragon::Vector2 mouseTilePos = tilemap.WorldToMapCoordinates(m_mousePosition);
	//size_t mouseTileIndex = tilemap.IndexFromPosition(mouseTilePos.x, mouseTilePos.y);
}

void PCGTowersLayer::HandleKeyReleased(dragon::KeyReleased& keyEvent)
{

}

void PCGTowersLayer::HandleMouseScroll(dragon::MouseScrolled& mouseScrollEvent)
{

}

void PCGTowersLayer::HandleMouseMoved(dragon::MouseMoved& mouseMoveEvent)
{
	m_mousePosition = mouseMoveEvent.m_position;
}
