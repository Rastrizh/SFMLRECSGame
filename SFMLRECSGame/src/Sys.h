#ifndef SYS_H
#define SYS_H

#include "Comps.h"
#include <RECS.h>
#include <math.h>

using namespace RECS;

class LifeTimeSystem : public ISystem
{
public:
	virtual void Update(float deltaTime) final
	{
		Engine::each<LifeTimeComponent>([&deltaTime](Entity* e, ComponentHandle<LifeTimeComponent> life) -> void {
			if (e->isAlive)
			{
				if (life->lifeTime <= life->maxLifeTime)
					life->lifeTime += deltaTime;
				else
					Engine::KillEntity(e);
			}
			else
				Engine::KillEntity(e);

			});
	}
};

class MoveSystem : public ISystem
{
public:
	virtual void Update(float deltaTime) final
	{
		if (deltaTime < m_deltaTime)
			return;
		RECS::Engine::each<PositionComponent, VelocityComponent, SpriteComponent>(
			[&deltaTime](RECS::Entity* e, ComponentHandle<PositionComponent> pos, ComponentHandle<VelocityComponent> vel, ComponentHandle<SpriteComponent> sprite) -> void {

				pos->pos += vel->vel;
				sprite->sprite.setPosition(pos->pos);
			});
	}
}; // MoveSystem class


class AnimationSystem : public RECS::ISystem
{
public:
	virtual void Update(float deltaTime) override
	{
		if (deltaTime < m_deltaTime)
			return;

		Engine::each<SpriteComponent, AnimationComponent>(
			[&deltaTime](Entity* e, ComponentHandle<SpriteComponent> sprite, ComponentHandle<AnimationComponent> anim) -> void {
				auto size = sprite->sprite.getTexture()->getSize();
				auto rect = sprite->sprite.getTextureRect();
				u32 rect_number = size.x / anim->rect_size.x;
				anim->counter += anim->speed * deltaTime;
				if (anim->counter >= 1.0f)
				{
					if ((u32)anim->frame < rect_number - 1)
					{
						sprite->sprite.setTextureRect(sf::IntRect(rect.left + anim->rect_size.x, rect.top, rect.width, rect.height));
						anim->frame++;
						anim->counter = 0;
					}
					else
					{
						sprite->sprite.setTextureRect(sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(rect.width, rect.height)));
						anim->frame = 0;
						anim->counter = 0;
					}
				}
			});
	}
}; // Animation System class

class CollisionSystem : public ISystem
{
public:

	void Update(float deltaTime) override
	{
		if (deltaTime < m_deltaTime)
			return;
		std::vector<Entity*> view = Engine::getView<IdentityComponent, SpriteComponent>();
		for (auto &iter1 : view)
		{
			for (auto &iter2 : view)
			{
				if (iter1->GetComponent<IdentityComponent>()->name == "Player" &&
					iter2->GetComponent<IdentityComponent>()->name == "Rock")
				{
					auto player_sprite = iter1->GetComponent<SpriteComponent>();
					auto rock_sprite = iter2->GetComponent<SpriteComponent>();
					if (player_sprite->sprite.getGlobalBounds().intersects(rock_sprite->sprite.getGlobalBounds()))
					{
						iter1->isAlive = false;
						iter2->isAlive = false;

						OnExplode(iter1);
					}
				}
				else if (iter1->GetComponent<IdentityComponent>()->name == "Bullet" &&
					iter2->GetComponent<IdentityComponent>()->name == "Rock")
				{
					auto bullet_sprite = iter1->GetComponent<SpriteComponent>();
					auto rock_sprite = iter2->GetComponent<SpriteComponent>();
					if (bullet_sprite->sprite.getGlobalBounds().intersects(rock_sprite->sprite.getGlobalBounds()))
					{
						iter1->isAlive = false;
						iter2->isAlive = false;

						OnExplode(iter2);
					}
				}
			}
		}
	}

};

class DrawSystem : public ISystem
{
public:
	virtual void Update(float deltaTime) override
	{
	}

	void Draw(sf::RenderWindow &app)
	{
		auto count = EntityManager::TotalEntities();
		for (auto i = 0; i < count; i++)
		{
			auto e = EntityManager::GetEntity(i);
			if (e && e->isDrawable && e->HasComponent<SpriteComponent>())
				app.draw(e->GetComponent<SpriteComponent>()->sprite);
		}
	}
};

#endif