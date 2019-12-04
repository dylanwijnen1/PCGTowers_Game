#pragma once

#include <Dragon/Application/Layer.h>
#include <Dragon/Application/Window/WindowEvents.h>

#include <Dragon/Game/Tilemap/Tilemap.h>

struct TileData
{
	float noiseValue;
};

class PCGTowersLayer final : public dragon::Layer
{
	dragon::DataTilemap<TileData> m_map;

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
};