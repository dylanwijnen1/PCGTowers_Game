#pragma once

#include <Game/TowerDefense/World.h>

#include <Dragon/Application/Layer.h>
#include <Dragon/Application/ApplicationEvent.h>

#include <Dragon/Generic/Random.h>

#include <SFML/Graphics/Font.hpp>

class PCGTowersLayer final : public dragon::Layer
{
	World m_world;

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
};