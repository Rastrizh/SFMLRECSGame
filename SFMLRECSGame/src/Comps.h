#ifndef COMPS_H
#define COMPS_H

#include "SFML/Graphics.hpp"
using namespace RECS;

struct IdentityComponent : public Component<IdentityComponent>
{
	const std::string name;
};

struct LifeTimeComponent : public Component<LifeTimeComponent>
{
	float lifeTime;
	float maxLifeTime;
};

struct SpriteComponent : public Component<SpriteComponent>
{
	sf::Sprite sprite;
};

struct PositionComponent : public Component<PositionComponent>
{

	sf::Vector2f pos;
};

struct VelocityComponent : public Component<VelocityComponent>
{
	sf::Vector2f vel;
};

struct AnimationComponent : public Component<AnimationComponent>
{
	sf::Vector2u rect_size;
	float frame;
	float speed;
	float counter;
};

struct CollisionComponent : public Component<CollisionComponent>
{
	float radius;
};

struct ExplosionComponent : public Component<ExplosionComponent>
{
	sf::Sprite sprite;
};

#endif