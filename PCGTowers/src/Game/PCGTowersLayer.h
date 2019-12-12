#pragma once

#include <Dragon/Application/Layer.h>
#include <Dragon/Application/Window/WindowEvents.h>

#include <Game/MapGenerator.h>

#include <SFML/Graphics/Font.hpp>

class PCGTowersLayer final : public dragon::Layer
{
	inline static constexpr size_t m_kMapSize = 45;
	inline static constexpr float m_kTileSize = 16.0f;

	MapGenerator m_mapGenerator;

	TDTilemap m_map;

	dragon::Tilemap m_noiseTilemap;

	dragon::Vector2f m_mousePosition;

	sf::Font m_font;

public:
	PCGTowersLayer()
		: dragon::Layer("PCGTowersLayer")
	{}

	~PCGTowersLayer();

	PCGTowersLayer(const PCGTowersLayer&) = delete;
	PCGTowersLayer(PCGTowersLayer&&) = delete;
	PCGTowersLayer& operator=(const PCGTowersLayer&) = delete;
	PCGTowersLayer& operator=(PCGTowersLayer&&) = delete;

	virtual void OnAttach() final override;
	virtual void OnDetach() final override;

	virtual void Update(float dt) final override;
	virtual void FixedUpdate(float dt) final override;
	virtual void Render(dragon::RenderTarget& target) final override;

	virtual void OnEvent(dragon::ApplicationEvent& ev) final override;

private:
	void HandleKeyPressed(dragon::KeyPressed& keyEvent);
	void HandleKeyReleased(dragon::KeyReleased& keyEvent);
	void HandleMouseScroll(dragon::MouseScrolled& mouseScrollEvent);
	void HandleMouseMoved(dragon::MouseMoved& mouseScrollEvent);

	void Regenerate();
};