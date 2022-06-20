#include <array>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "World.hpp"
#include "Pacman.hpp"
#include "MapCollision.hpp"

Pacman::Pacman() : animation_over(0), isDead(0), direction(0), energizer_timer(0), location({0, 0}){}

bool Pacman::Get_Animation_Over() { return animation_over; }

bool Pacman::Get_Dead() { return isDead; }

unsigned char Pacman::Get_Direction() { return direction; }

unsigned short Pacman::Get_Energizer_Timer() { return energizer_timer; }

void Pacman::Draw(bool i_victory, sf::RenderWindow& i_window)
{
	unsigned char frame = static_cast<unsigned char>(floor(animation_timer / static_cast<float>(PACMAN_ANIM_SPEED)));

	sf::Sprite sprite;

	sf::Texture texture;

	sprite.setPosition(location.x, location.y);

	if (1 == isDead || 1 == i_victory)
	{
		if (animation_timer < PACMAN_DEATH_FRAMES * PACMAN_ANIM_SPEED)
		{
			animation_timer++;

			texture.loadFromFile("Resources/Images/PacmanDeath" + std::to_string(CELL_SIZE) + ".png");

			sprite.setTexture(texture);
			sprite.setTextureRect(sf::IntRect(CELL_SIZE * frame, 0, CELL_SIZE, CELL_SIZE));

			i_window.draw(sprite);
		}
		else
		{
			// You can only die once.
			animation_over = 1;
		}
	}
	else
	{
		texture.loadFromFile("Resources/Images/Pacman" + std::to_string(CELL_SIZE) + ".png");

		sprite.setTexture(texture);
		sprite.setTextureRect(sf::IntRect(CELL_SIZE * frame, CELL_SIZE * direction, CELL_SIZE, CELL_SIZE));

		i_window.draw(sprite);

		animation_timer = (1 + animation_timer) % (PACMAN_ANIM_FRAMES * PACMAN_ANIM_SPEED);
	}
}

void Pacman::Reset()
{
	animation_over = 0;
	isDead = 0;

	direction = 0;

	animation_timer = 0;
	energizer_timer = 0;
}

void Pacman::Set_Animation_Timer(unsigned short i_animation_timer)
{
	animation_timer = i_animation_timer;
}

void Pacman::set_Dead(bool i_dead)
{
	isDead = i_dead;

	if (1 == isDead)
	{
		// Making sure that the animation starts at the beginning.
		animation_timer = 0;
	}
}

void Pacman::Set_Position(short i_x, short i_y)
{
	location = { i_x, i_y };
}

void Pacman::Update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map)
{
	std::array<bool, 4> walls{};
	walls[0] = map_collision(0, 0, PACMAN_SPEED + location.x, location.y, i_map);
	walls[1] = map_collision(0, 0, location.x, location.y - PACMAN_SPEED, i_map);
	walls[2] = map_collision(0, 0, location.x - PACMAN_SPEED, location.y, i_map);
	walls[3] = map_collision(0, 0, location.x, PACMAN_SPEED + location.y, i_map);

	if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		if (0 == walls[0])
		{
			direction = 0;
		}
	}

	if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		if (0 == walls[1])
		{
			direction = 1;
		}
	}

	if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		if (0 == walls[2])
		{
			direction = 2;
		}
	}

	if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		if (0 == walls[3])
		{
			direction = 3;
		}
	}

	if (0 == walls[direction])
	{
		switch (direction)
		{
			case 0:
			{
				location.x += PACMAN_SPEED;

				break;
			}

			case 1:
			{
				location.y -= PACMAN_SPEED;

				break;
			}

			case 2:
			{
				location.x -= PACMAN_SPEED;

				break;
			}

			case 3:
			{
				location.y += PACMAN_SPEED;
			}
		}
	}

	if (-CELL_SIZE >= location.x)
	{
		location.x = CELL_SIZE * MAP_WIDTH - PACMAN_SPEED;
	}
	else if (CELL_SIZE * MAP_WIDTH <= location.x)
	{
		location.x = PACMAN_SPEED - CELL_SIZE;
	}

	if (1 == map_collision(1, 0, location.x, location.y, i_map))
	{
		// Pacman becomes energized.
		energizer_timer = static_cast<unsigned short>(ENERGIZE_DURATION / pow(2, i_level));
	}
	else
	{
		energizer_timer = std::max(0, energizer_timer - 1);
	}
}

Location Pacman::Get_Location() { return location; }