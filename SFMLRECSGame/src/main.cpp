#define _USE_MATH_DEFINES
#include <RECS.h>
#include "Comps.h"

#include <iostream>
#include <random>
#include <math.h>
#include "SFML/Graphics.hpp"

using namespace RECS;
using namespace sf;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> hor (0.0f, 600.0f);
std::uniform_real_distribution<float> vert(0.0f, 960.0f);
std::uniform_real_distribution<float> vel (7.0f, 16.0f);
std::uniform_real_distribution<float> real(0.0f, 1.0f);

float spawn_time = 0.0f;

void InitPlayer(Entity* e, const sf::Texture& texture, const sf::Texture& explosion_texture)
{
	e->AddComponent<IdentityComponent>("Player");
	e->AddComponent<SpriteComponent>(sf::Sprite(texture, sf::IntRect(0, 0, 245, 355)));
	e->AddComponent<PositionComponent>(sf::Vector2f(320.0f, 770.0f));
	e->AddComponent<VelocityComponent>(sf::Vector2f(7.0f, 0.0f));
	e->AddComponent<CollisionComponent>(177.0f);
	e->AddComponent<ExplosionComponent>(sf::Sprite(explosion_texture, sf::IntRect(0, 0, 192, 192)));

	auto spriteComponent = e->GetComponent<SpriteComponent>();
	spriteComponent->sprite.setOrigin(122.5f, 180.0f);
	spriteComponent->sprite.setPosition(e->GetComponent<PositionComponent>()->pos);
	spriteComponent->sprite.scale(sf::Vector2f(0.3f, 0.3f));

}

void InitBullet(Entity* e, sf::Vector2f pos, const sf::Texture& texture)
{
	e->AddComponent<IdentityComponent>("Bullet");
	e->AddComponent<SpriteComponent>(sf::Sprite(texture, sf::IntRect(0, 0, 256, 181)));
	e->AddComponent<PositionComponent>(pos);
	e->AddComponent<VelocityComponent>(sf::Vector2f(0.0f, -7.0f));
	e->AddComponent<LifeTimeComponent>(0.0f, 3.0f);
	e->AddComponent<CollisionComponent>(100.0f);
	e->GetComponent<SpriteComponent>()->sprite.setOrigin(128, 181);
	e->GetComponent<SpriteComponent>()->sprite.scale(sf::Vector2f(0.3f, 0.3f));
}

void InitRock(Entity* e, const sf::Texture& texture, const sf::Texture& explosion_texture)
{
	e->AddComponent<IdentityComponent>("Rock");
	e->AddComponent<SpriteComponent>(sf::Sprite(texture, sf::IntRect(0, 0, 64, 64)));
	e->GetComponent<SpriteComponent>()->sprite.setOrigin(32, 32);
	e->AddComponent<PositionComponent>(sf::Vector2f(hor(gen), 0));
	e->AddComponent<VelocityComponent>(sf::Vector2f(0, vel(gen) / (float)M_PI));
	e->AddComponent<LifeTimeComponent>(0.0f, 15.0f);
	e->AddComponent<AnimationComponent>(sf::Vector2u(64, 64), 0.0f, (float)vel(gen), 0.0f);
	e->AddComponent<CollisionComponent>(24.0f);
	e->AddComponent<ExplosionComponent>(sf::Sprite(explosion_texture, sf::IntRect(0, 0, 256, 256)));
}

void InitExplosion(Entity* explosion, sf::Vector2f pos, const sf::Texture& texture)
{
	explosion->AddComponent<SpriteComponent>(sf::Sprite(texture, sf::IntRect(0, 0, 192, 192)));
	explosion->AddComponent<PositionComponent>(pos);
	explosion->GetComponent<SpriteComponent>()->sprite.setPosition(pos);
	explosion->GetComponent<SpriteComponent>()->sprite.setOrigin(96, 96);
	explosion->GetComponent<SpriteComponent>()->sprite.scale(sf::Vector2f(0.4f, 0.4f));
	explosion->AddComponent<LifeTimeComponent>(0.0f, 2.0f);
	explosion->AddComponent<AnimationComponent>(sf::Vector2u(192, 192), 0.0f, 30.0f, 0.0f);
}

RECS::event<Entity*> OnExplode;

#include "Sys.h"

int main()
{
	sf::Texture bullet_texture, back_texture, player_texture, rock_texture, small_rock_texture, player_explosion_texture, rock_explosion_texture;
	bullet_texture.loadFromFile("content/Ship11fire.png");
	back_texture.loadFromFile("content/background.jpg");
	player_texture.loadFromFile("content/Ship11.png");
	rock_texture.loadFromFile("content/rock.png");
	small_rock_texture.loadFromFile("content/rock_small.png");
	player_explosion_texture.loadFromFile("content/explosions/type_B.png");
	rock_explosion_texture.loadFromFile("content/explosions/type_B.png");

	OnExplode += [](Entity* e) {

		auto entity_texture = e->GetComponent<ExplosionComponent>()->sprite.getTexture();
		auto pos = e->GetComponent<PositionComponent>()->pos;

		auto explosion = Engine::CreateEntity();
		explosion->AddComponent<SpriteComponent>(sf::Sprite(*entity_texture, sf::IntRect(0, 0, 192, 192)));
		explosion->AddComponent<PositionComponent>(pos);
		explosion->GetComponent<SpriteComponent>()->sprite.setPosition(pos);
		explosion->GetComponent<SpriteComponent>()->sprite.setOrigin(96, 96);
		explosion->GetComponent<SpriteComponent>()->sprite.scale(sf::Vector2f(0.4f, 0.4f));
		explosion->AddComponent<LifeTimeComponent>(0.0f, 2.0f);
		explosion->AddComponent<AnimationComponent>(sf::Vector2u(192, 192), 0.0f, 30.0f, 0.0f);
	};

	auto scene = Engine::CreateEntity();
	scene->AddComponent<SpriteComponent>(sf::Sprite(back_texture));

	auto player = Engine::CreateEntity();
	InitPlayer(player, player_texture, player_explosion_texture);

	AnimationSystem animSystem;
	MoveSystem moveSystem;
	DrawSystem drawSystem;
	LifeTimeSystem lifeSystem;
	CollisionSystem collisionSystem;

	sf::RenderWindow window(sf::VideoMode(600, 960), "Test");
	window.setFramerateLimit(60);

	while (window.isOpen())
	{		
		Timer::Tick();
		if (Timer::DeltaTime() >= 1.F / 60.F)
		{
			Timer::Reset();
			player->isUpdateble = false;
			sf::Event event;
			while (window.pollEvent(event))
			{

				if (event.type == sf::Event::Closed) window.close();
				if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::Space)
					{
						auto bullet = Engine::CreateEntity();
						InitBullet(bullet, player->GetComponent<PositionComponent>()->pos, bullet_texture);
					}
					if (event.key.code == sf::Keyboard::Escape) window.close();
				}
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			{
				if (player->GetComponent<VelocityComponent>()->vel.x < 0)
				{
					player->GetComponent<VelocityComponent>()->vel.x *= (-1.0f);
				}
				player->isUpdateble = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			{
				if (player->GetComponent<VelocityComponent>()->vel.x > 0)
				{
					player->GetComponent<VelocityComponent>()->vel.x *= (-1.0f);
				}
				player->isUpdateble = true;
			}

			if (spawn_time < 0.4f)
				spawn_time += Timer::DeltaTime();
			else
			{
				auto rock = Engine::CreateEntity();
				if (Engine::EntityCount() % 3 == 0)
					InitRock(rock, rock_texture, rock_explosion_texture);
				else
					InitRock(rock, small_rock_texture, rock_explosion_texture);
				spawn_time = 0.0f;
			}

			collisionSystem.Update(Timer::DeltaTime());
			lifeSystem.Update(Timer::DeltaTime());
			moveSystem.Update(Timer::DeltaTime());
			animSystem.Update(Timer::DeltaTime());

			window.clear();
			drawSystem.Draw(window);

			window.display();
			//std::cout << "FPS: " << 1.F / Timer::DeltaTime() << " fps\n";
			//std::cout << "Entities count: " << RECS::EntityManager::EntityCount() << " \n";
			//system("CLS");
		}
	}
	std::cin.get();
	return 0;
}
