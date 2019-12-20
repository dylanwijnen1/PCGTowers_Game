#pragma once

#include <Config.h>

#include <Dragon/Generic/Math.h>
#include <EASTL/vector.h>

namespace dragon
{
	class RenderTarget;
}

class Turret
{
	/// <summary>
	/// Position of the turret.
	/// </summary>
	dragon::Vector2f m_position;

	/// <summary>
	/// Damage to deal to the enemy.
	/// </summary>
	float m_damage;

	/// <summary>
	/// Range of the turret.
	/// </summary>
	float m_range;

	/// <summary>
	/// Cooldown timer between damaging.
	/// </summary>
	float m_cooldown;

	/// <summary>
	/// Last time the turret took a shot.
	/// </summary>
	float m_lastDamageTime;

	/// <summary>
	/// Current upgrade level
	/// </summary>
	size_t m_upgradeLevel;

	/// <summary>
	/// Wether or not this turret is enabled.
	/// </summary>
	bool m_enabled;

	/// <summary>
	/// Last closest match, If nullptr the turret will look for the new closest match.
	/// </summary>
	class Enemy* m_pTarget;

public:

	Turret()
		: m_position(0.0f, 0.0f)
		, m_damage(25.0f)
		, m_range(15.0f)
		, m_cooldown(2.0f)
		, m_lastDamageTime(0.0f)
		, m_upgradeLevel(1)
		, m_enabled(true)
		, m_pTarget(nullptr)
	{}

	void Update(float dt);

	void Render(dragon::RenderTarget& target);

	/// <summary>
	/// Find and shoot the closest target.
	/// </summary>
	/// <param name="enemies"></param>
	void FindTarget(eastl::vector<class Enemy*>& enemies);

	/// <summary>
	/// Clears the target. So that the turret can start finding a new target.
	/// </summary>
	void ClearTarget() { m_pTarget = nullptr; }

	void SetPosition(dragon::Vector2f pos) { m_position = pos; }
	dragon::Vector2f GetPosition() const { return m_position; }

	float GetRange() const { return m_range; }
	void SetRange(float range) { m_range = range; }

	float GetDamage() const { return m_damage; }
	void SetDamage(float damage) { m_damage = damage; }

	float GetCooldown() const { return m_cooldown; }
	void SetCooldown(float cooldown) { m_cooldown = cooldown; }

	float GetUpgradeCost() const { return m_upgradeLevel * g_kTurretUpgradeCost * g_kTurretUpgradeCostMultiplier; }
	size_t GetUpgradeLevel() const { return m_upgradeLevel; }

	float GetResaleValue() const { return g_kTurretCost + m_upgradeLevel * g_kTurretUpgradeCost; }
	
	/// <summary>
	/// Upgrades the turret to the next level.
	/// Which increases its Damage, Range and reduces its Cooldown.
	/// </summary>
	void Upgrade();

	void Enable() { m_enabled = true; }
	void Disable() { m_enabled = false; }

	/// <summary>
	/// Wether or not this turret has been enabled or disabled.
	/// </summary>
	/// <returns></returns>
	bool IsActive() const { return m_enabled; }

private:

	void ShootTarget(class Enemy* pEnemy);
};