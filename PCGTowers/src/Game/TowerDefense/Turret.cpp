#include "Turret.h"

#include <Config.h>

#include <Game/TowerDefense/Enemy.h>

#include <Dragon/Graphics/RenderTexture.h>
#include <SFML/Graphics.hpp>
#include <Platform/SFML/SfmlHelpers.h>

static constexpr dragon::Color g_kUpgradeOutlineColors[] =
{
	dragon::Color(0.80f, 0.49f, 0.19f), // Bronze
	dragon::Colors::Silver,
	dragon::Colors::Gold,
	dragon::Color(0.895f, 0.894f, 0.893f) // Platinum
};

void Turret::Update(float dt)
{
	// Cooldown timer
	m_lastDamageTime -= dt;

	// Determine if target is still within range and we are enabled.
	if (m_pTarget && m_enabled)
	{
		float distanceSqrd = dragon::Vector2f::DistanceSquared(m_position, m_pTarget->GetPosition());

		// Check if within range.
		if (distanceSqrd < m_range * m_range)
		{
			if (m_lastDamageTime < 0.0f)
			{
				ShootTarget(m_pTarget);
			}
		}
		else
		{
			ClearTarget();
		}
	}

}

void Turret::Render(dragon::RenderTarget& target)
{
	sf::RenderTarget* pSfTarget = target.GetNativeTarget<sf::RenderTarget*>();

	// Draw turret range in debug mode.
	sf::CircleShape turretShape(g_kTileSize / 2.0f, 5);

	// Draw turret, Rotating towards m_pTarget
	float rotation = 0.0f;
	if (m_pTarget)
	{
		dragon::Vector2f directionNormalized = (m_pTarget->GetPosition() - m_position).Normalized();
		rotation = std::atan2(directionNormalized.y, directionNormalized.x);
	}

	turretShape.setRotation(dragon::math::RadToDeg(rotation));
	turretShape.setPosition(sf::Convert(m_position));
	turretShape.setOrigin(g_kTileSize / 2.0f, g_kTileSize / 2.0f);
	turretShape.setFillColor(sf::Convert(dragon::Colors::SaddleBrown));

	if (m_upgradeLevel > 1)
	{
		static constexpr size_t kUpgradeOutlineColorsSize = sizeof(g_kUpgradeOutlineColors) / sizeof(g_kUpgradeOutlineColors[0]);

		dragon::Color outlineColor = g_kUpgradeOutlineColors[m_upgradeLevel % kUpgradeOutlineColorsSize];
		turretShape.setOutlineColor(sf::Convert(outlineColor));

		const size_t kOutlineSize = m_upgradeLevel / kUpgradeOutlineColorsSize;
		turretShape.setOutlineThickness((float)kOutlineSize);
	}

	pSfTarget->draw(turretShape);
}

void Turret::ShootTarget(Enemy* pEnemy)
{
	// Apply cooldown.
	m_lastDamageTime = m_cooldown;

	// Do damage.
	pEnemy->Damage(m_damage);

	// It would've been a lot better to subscribe to some event. But this works.
	if (pEnemy->GetHealth() <= 0.0f)
		ClearTarget();
}

void Turret::FindTarget(eastl::vector<class Enemy*>& enemies)
{
	// Only find a target if we need to. This speeds things up since we're searching over all enemies.
	if (m_pTarget)
		return;

	// Best Match
	Enemy* pClosestTarget = nullptr;
	float closestDistanceSqrd = eastl::numeric_limits<float>::infinity();

	for (Enemy* pEnemy : enemies)
	{
		// Filter out enemies that are dead.
		if (pEnemy->GetHealth() <= 0.0f)
			continue;

		float distanceSqrd = dragon::Vector2f::DistanceSquared(m_position, pEnemy->GetPosition());

		// Within distance and closest.
		if (distanceSqrd < m_range * m_range && closestDistanceSqrd > distanceSqrd)
		{
			closestDistanceSqrd = distanceSqrd;
			pClosestTarget = pEnemy;
		}
	}

	// Set target
	m_pTarget = pClosestTarget;

}

void Turret::Upgrade()
{
	++m_upgradeLevel;
	m_damage *= 1.0f + (m_upgradeLevel * 0.15f);
	m_range *= 1.05f;
	m_cooldown *= .95f;
}