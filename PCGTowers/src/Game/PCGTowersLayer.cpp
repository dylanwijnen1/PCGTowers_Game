#include "PCGTowersLayer.h"

#include <Game/MapGenerator.h>

#include <SFML/Graphics.hpp>

PCGTowersLayer::~PCGTowersLayer()
{
}

void PCGTowersLayer::OnAttach()
{
	m_mapGenerator.Init();

	m_map.Init({ m_kMapSize, m_kMapSize }, { m_kTileSize, m_kTileSize });
	m_noiseTilemap.Init({ m_kMapSize, m_kMapSize }, { m_kTileSize, m_kTileSize });
	m_noiseTilemap.LoadTileset("gradient-tiles.png");

	m_font.loadFromFile("retro_gaming.ttf");

	Regenerate();
}

void PCGTowersLayer::OnDetach()
{

}

void PCGTowersLayer::Update(float dt)
{

}

void PCGTowersLayer::FixedUpdate(float dt)
{

}

void PCGTowersLayer::Render(dragon::RenderTarget& target)
{
	target.Draw(m_noiseTilemap);

	dragon::Vector2 mouseTilePos = m_map.WorldToMapCoordinates(m_mousePosition);
	dragon::RectF mouseTileRect =
	{
		(float)mouseTilePos.x * m_kTileSize,
		(float)mouseTilePos.y * m_kTileSize,
		m_kTileSize,
		m_kTileSize
	};

	target.DrawRect(mouseTileRect, dragon::Colors::Red);

	const TDTileData& tileData = m_map.GetTileData(mouseTilePos.x, mouseTilePos.y);

	std::string tileInfo = 
		"Mouse Pos: (" + std::to_string(mouseTilePos.x) + ", " + std::to_string(mouseTilePos.y) + ");" +
		"\nNoise: " + std::to_string(tileData.m_noise);

	sf::Text infoText(tileInfo, m_font);
	infoText.setPosition(0, 0);
	infoText.setColor(sf::Color::Red);

	sf::RenderTarget* pSfTarget = static_cast<sf::RenderTarget*>(target.GetNativeTarget());
	pSfTarget->draw(infoText);

	m_mapGenerator.RenderPath(target);
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

void PCGTowersLayer::Regenerate()
{
	m_mapGenerator.Generate(m_map);

	for (size_t x = 0; x < m_kMapSize; ++x)
	{
		for (size_t y = 0; y < m_kMapSize; ++y)
		{
			const TDTileData& tileData = m_map.GetTileData(x, y);
			m_noiseTilemap.SetTile(x, y, (dragon::TileID)(tileData.m_noise * 255u));
		}
	}
}
