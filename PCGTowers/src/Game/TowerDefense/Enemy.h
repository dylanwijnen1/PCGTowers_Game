#pragma once

#include <Game/Path.h>

#include <Dragon/Generic/Math.h>
#include <Dragon/Graphics/Color.h>

#include <Dragon/Generic/Random/Range.h>

#include <EASTL/vector.h>

namespace dragon
{
	class RenderTarget;
}

class Enemy
{
public:

	/// <summary>
	/// Determines the shape of the enemy.
	/// </summary>
	enum struct Shape
	{
		kSquare,
		kCircle,
		kTriangle
	};

	struct Stats
	{
		float m_speed;		// The speed of this enemy.
		float m_damage;		// Damage to base.
		float m_maxHealth;	// Max health of this enemy.

		Stats()
			: m_speed(0.0f)
			, m_damage(0.0f)
			, m_maxHealth(0.0f)
		{}
	};


private:

	/// <summary>
	/// Path the enemy is following.
	/// </summary>
	const Path* m_pPath;

	/// <summary>
	/// The stats of this enemy.
	/// </summary>
	Stats m_stats;

	float m_health;

	dragon::Vector2f m_position;
	size_t m_nextTile;

	// Drawing Data
	Shape m_shape;
	dragon::Color m_color;

public:

	Enemy()
		: m_pPath(nullptr)
		, m_health(0.0f)
		, m_position(0.0f, 0.0f)
		, m_nextTile(0)
		, m_shape(Shape::kSquare)
		, m_color(dragon::Colors::White)
	{}

	~Enemy();

	dragon::Vector2f GetPosition() const { return m_position; }

	// TODO: Possibly returning by value is faster.
	void SetStats(const Stats& stats) { m_stats = stats; SetHealth(stats.m_maxHealth); }
	const Stats& GetStats() const { return m_stats; }

	void SetHealth(float health) { m_health = health; }
	float GetHealth() const { return m_health; }
	void Damage(float damage) { m_health -= damage; }

	void SetShape(Shape shape) { m_shape = shape; }
	Shape GetShape() const { return m_shape; }

	void SetColor(dragon::Color color) { m_color = color; }
	dragon::Color GetColor() const { return m_color; }

	void SetPath(const Path* pPath);
	const Path* GetPath() const { return m_pPath; }

	/// <summary>
	/// Moves along the path.
	/// </summary>
	/// <param name="dt"></param>
	void Update(float dt);

	void Render(dragon::RenderTarget& target);
};