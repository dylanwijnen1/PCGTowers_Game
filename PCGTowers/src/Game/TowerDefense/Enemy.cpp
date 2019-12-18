#include "Enemy.h"

#include <Config.h>

#include <Dragon/Graphics/RenderTarget.h>
#include <Platform/SFML/SfmlHelpers.h>
#include <SFML/Graphics.hpp>

Enemy::~Enemy()
{
	m_pPath = nullptr;
}

void Enemy::SetPath(const Path* pPath)
{
	assert(pPath->size() > 1);

	m_pPath = pPath;
	m_position = pPath->at(0); // Set position to starting position.
	m_nextTile = 1;
}

void Enemy::Update(float dt)
{
	if (!m_pPath)
		return; // Nothing to do here...

	if (m_nextTile >= m_pPath->size())
		return;

	// Move towards tile.
	dragon::Vector2f nextPosition = m_pPath->at(m_nextTile);
	dragon::Vector2f direction = nextPosition - m_position;
	dragon::Vector2f directionNormalized = direction.Normalized();

	m_position += directionNormalized * m_stats.m_speed * dt;

	if (std::abs(direction.LengthSquared()) < 1.f) // TODO: Replace small amount with something more defined.
		++m_nextTile;
}

void Enemy::Render(dragon::RenderTarget& target)
{
	sf::RenderTarget* pSfTarget = target.GetNativeTarget<sf::RenderTarget*>();

	sf::Shape* pShape = nullptr;

	switch (m_shape)
	{
	case Shape::kCircle:
		pShape = new sf::CircleShape(g_kTileSize / 2.0f, 16);
		break;
	case Shape::kSquare:
		pShape = new sf::RectangleShape(sf::Vector2(g_kTileSize, g_kTileSize));
		break;
	case Shape::kTriangle:
		pShape = new sf::CircleShape(g_kTileSize / 2.0f, 3);
		break;
	}

	if (pShape)
	{
		pShape->setPosition(sf::Convert(m_position - (g_kTileSize / 2.0f)));
		pShape->setFillColor(sf::Convert(m_color));
		pSfTarget->draw(*pShape);
		delete pShape;
	}
	
}
