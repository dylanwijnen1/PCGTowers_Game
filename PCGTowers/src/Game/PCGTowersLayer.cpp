#include "PCGTowersLayer.h"


#include <Game/MapGenerator.h>

PCGTowersLayer::~PCGTowersLayer()
{
}

void PCGTowersLayer::OnAttach()
{
	MapGenerator generator;
	m_map.Init({ 100, 100 }, { 8.0f, 8.0f });
	m_map.LoadTileset("gradient-tiles.png");
	generator.Generate(m_map);
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
	target.Draw(m_map);
}

void PCGTowersLayer::OnEvent(dragon::ApplicationEvent& ev)
{
	ev.Dispatch<dragon::KeyPressed>(this, &PCGTowersLayer::HandleKeyPressed);
	ev.Dispatch<dragon::KeyReleased>(this, &PCGTowersLayer::HandleKeyReleased);
	ev.Dispatch<dragon::MouseScrolled>(this, &PCGTowersLayer::HandleMouseScroll);
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