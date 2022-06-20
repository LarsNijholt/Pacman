#include <array>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "World.hpp"
#include "Pacman.hpp"
#include "Ghost.hpp"
#include "MapCollision.hpp"

Ghost::Ghost(unsigned char i_id) : id(i_id) {}

bool Ghost::Pacman_Colission(const Location& i_pacman_location)
{
	if (location.x > i_pacman_location.x - CELL_SIZE && location.x < CELL_SIZE + i_pacman_location.x)
	{
		if (location.y > i_pacman_location.y - CELL_SIZE && location.y < CELL_SIZE + i_pacman_location.y)
		{
			return 1;
		}
	}
	return 0;
}

float Ghost::Get_Target_Distance(unsigned char i_direction)
{
	short x = location.x;
	short y = location.y;

	switch (i_direction)
	{
	case 0:
	{
		x += GHOST_SPEED;

		break;
	}

	case 1:
	{
		y -= GHOST_SPEED;

		break;
	}

	case 2:
	{
		x -= GHOST_SPEED;

		break;
	}

	case 3:
	{
		y += GHOST_SPEED;

	}
	}

	// Using the pythagorean theorem to calculate the distance.
	return static_cast<float>(sqrt(pow(x - target.x, 2) + pow(y - target.y, 2)));
}

void Ghost::Draw(bool i_flash, sf::RenderWindow& i_window)
{
	// Current frame of the animation.
	unsigned char Bodyframe = static_cast<unsigned char>(floor(animation_timer / static_cast<float>(GHOST_ANIM_SPEED)));

	sf::Sprite body;
	sf::Sprite face;

	sf::Texture texture;
	texture.loadFromFile("Resources/Images/Ghost" + std::to_string(CELL_SIZE) + ".png");

	body.setTexture(texture);
	body.setPosition(location.x, location.y);

	// Showing images fast to get animation.
	body.setTextureRect((sf::IntRect(CELL_SIZE * Bodyframe, 0, CELL_SIZE, CELL_SIZE)));

	face.setTexture(texture);
	face.setPosition(location.x, location.y);

	// Ghost is not frightened
	if (0 == frightened_mode)
	{
		switch (id)
		{
		case 0: // Red color.
		{
			body.setColor(sf::Color(255, 0, 0));

			break;
		}

		case 1: // Pink color.
		{
			body.setColor(sf::Color(255, 182, 255));

			break;
		}

		case 2: // Blue color.
		{
			body.setColor(sf::Color(0, 255, 255));

			break;
		}

		case 3: // Orange color.
		{
			body.setColor(sf::Color(255, 182, 85));
		}
		}

		face.setTextureRect(sf::IntRect(CELL_SIZE * Direction, CELL_SIZE, CELL_SIZE, CELL_SIZE));

		i_window.draw(body);
	}
	else if (1 == frightened_mode) // GHost is getting frightened.
	{
		body.setColor(sf::Color(36, 36, 255));
		// The face remains the same regardless of where the ghost is going right now.
		face.setTextureRect(sf::IntRect(4 * CELL_SIZE, CELL_SIZE, CELL_SIZE, CELL_SIZE));

		if (1 == i_flash && 0 == Bodyframe % 2)
		{
			body.setColor(sf::Color(255, 255, 255));
			face.setColor(sf::Color(255, 0, 0));
		}
		else
		{
			body.setColor(sf::Color(36, 36, 255));
			face.setColor(sf::Color(255, 255, 255));
		}

		i_window.draw(body);
	}
	else // Ghost is frightened.
	{
		// We only draw the face here.
		face.setTextureRect(sf::IntRect(CELL_SIZE * Direction, 2 * CELL_SIZE, CELL_SIZE, CELL_SIZE));
	}

	i_window.draw(face);

	//------------------------------------< this is to prevent overflowing >------------
	animation_timer = (1 + animation_timer) % (GHOST_ANIM_FRAMES * GHOST_ANIM_SPEED);
}

void Ghost::Reset(const Location& i_home, const Location& i_home_exit)
{
	movement_mode = 0;

	// Every ghost can use the door except the red ghost.
	can_use_door = 0 < id;

	Direction = 0;
	frightened_mode = 0;
	frightened_speed_timer = 0;

	animation_timer = 0;

	home = i_home;
	home_exit = i_home_exit;
	target = i_home_exit;
}

void Ghost::Set_Location(short i_x, short i_y)
{
	location = { i_x, i_y };
}

void Ghost::Switch_Mode()
{
	movement_mode = 1 - movement_mode;
}

void Ghost::Update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map, Ghost& i_ghost_0, Pacman& pacman)
{
	// Can the ghost move?
	bool move = 0;

	// If this is greater than 1, then the ghost has reached an intersection, we don't consider the way back as an availible way.
	unsigned char availible_ways = 0;
	unsigned char speed = GHOST_SPEED;

	std::array<bool, 4> Walls{};

	// Here the ghosts starts and stops being frightened.
	if (0 == frightened_mode && pacman.Get_Energizer_Timer() == ENERGIZE_DURATION / pow(2, i_level))
	{
		frightened_speed_timer = GHOST_FRIGHTENED_SPEED;

		frightened_mode = 1;
	}
	else if (0 == pacman.Get_Energizer_Timer() && 1 == frightened_mode)
	{
		frightened_mode = 0;
	}

	// Using the modulo operator in case the ghost goes out of the window.
	if (2 == frightened_mode && 0 == location.x % GHOST_ESCAPE_SPEED && 0 == location.y % GHOST_ESCAPE_SPEED);
	{
		speed = GHOST_ESCAPE_SPEED;
	}

	Update_Target(pacman.Get_Direction(), i_ghost_0.Get_Location(), pacman.Get_Location());

	Walls[0] = map_collision(0, can_use_door, speed + location.x, location.y, i_map);
	Walls[1] = map_collision(0, can_use_door, location.x, location.y - speed, i_map);
	Walls[2] = map_collision(0, can_use_door, location.x - speed, location.y, i_map);
	Walls[3] = map_collision(0, can_use_door, location.x, speed + location.y, i_map);

	if (1 != frightened_mode)
	{
		// I used 4 because using a number between 0 and 3 will make the ghost move in a direction it can't move.
		unsigned char optimal_direction = 4;

		// The ghost can move.
		move = 1;

		for (unsigned char a = 0; a < 4; a++)
		{
			// Ghosts can't turn back unless they really have to.
			if (a == (2 + Direction) % 4)
			{
				continue;
			}
			else if (0 == Walls[a])
			{
				if (4 == optimal_direction)
				{
					optimal_direction = a;
				}

				availible_ways++;

				if (Get_Target_Distance(a) < Get_Target_Distance(optimal_direction))
				{
					// The optimal direction is the direction that's closest to the target.
					optimal_direction = a;
				}
			}
		}

		if (1 < availible_ways)
		{
			// When the ghost is at the intersection, it chooses the optimal direction.
			Direction = optimal_direction;
		}
		else
		{
			if (4 == optimal_direction)
			{
				// this is when the ghosts has no other choice but to turn back.
				Direction = (2 + Direction) % 4;
			}
			else
			{
				Direction = optimal_direction;
			}
		}
	}
	else
	{
		unsigned char random_direction = rand() % 4;

		if (0 == frightened_speed_timer)
		{
			// The ghost can move after a certain number of frams.
			move = 1;

			frightened_speed_timer = GHOST_FRIGHTENED_SPEED;

			for (unsigned char a = 0; a < 4; a++)
			{
				// They can't turn back even if they're frightened.
				if (a == (2 + Direction) % 4)
				{
					continue;
				}
				else if (0 == Walls[a])
				{
					availible_ways++;
				}
			}

			if (0 < availible_ways)
			{
				while (1 == Walls[random_direction] || random_direction == (2 + Direction) % 4)
				{
					// We keep picking a random direction until we can use it/
					random_direction == rand() % 4;
				}

				Direction = random_direction;
			}
			else
			{
				// If there is no other way the ghost turns back;
				Direction = (2 + Direction) % 4;
			}
		}
		else
		{
			frightened_speed_timer--;
		}
	}

	// If the ghost can move, we move it.
	if (1 == move)
	{
		switch (Direction)
		{
		case 0:
		{
			location.x += speed;

			break;
		}

		case 1:
		{
			location.y -= speed;

			break;
		}

		case 2:
		{
			location.x -= speed;

			break;
		}

		case 3:
		{
			location.y += speed;
		}
		}

		// Warping
		// When the ghost leaves the map, we move it to the other side.
		if (-CELL_SIZE >= location.x)
		{
			location.x = CELL_SIZE * MAP_WIDTH - speed;
		}
		else if (location.x >= CELL_SIZE * MAP_WIDTH)
		{
			location.x = speed - CELL_SIZE;
		}
	}

	if (1 == Pacman_Colission(pacman.Get_Location()))
	{
		if (0 == frightened_mode) // When the ghost is not frightened and collides with Pacman, we kill Pacman.
		{
			pacman.set_Dead(1);
		}
		else
		{
			can_use_door = 1;

			frightened_mode = 2;

			target = home;
		}
	}
}

void Ghost::Update_Target(unsigned char i_pacman_direction, const Location& i_ghost_location, const Location& i_pacman_location)
{
	if (1 == can_use_door) // If the ghost can use the door.
	{
		if (location == target)
		{
			if (home_exit == target) // If the ghost has reached the exit.
			{
				can_use_door = 0; // It can no longer use the door. 
			}
			else if (home == target) // If the ghost has reached its home.
			{
				frightened_mode = 0; // It stops being frightened.

				target = home_exit; // And starts leaving the house.
			}
		}
	}
	else
	{
		if (0 == movement_mode) // The scatter mode.
		{
			switch (id)
			{
			case 0:
			{
				target = { CELL_SIZE * (MAP_WIDTH - 1), 0 };

				break;
			}

			case 1:
			{
				target = { 0,0 };

				break;
			}

			case 2:
			{
				target = { CELL_SIZE * (MAP_WIDTH - 1), CELL_SIZE * (MAP_HEIGHT - 1) };

				break;
			}

			case 3:
			{
				target = { 0, CELL_SIZE * (MAP_HEIGHT - 1) };
			}
			}
		}
		else // The chase mode.
		{
			switch (id)
			{
			case 0: // The red ghost will chase Pacman.
			{
				target = i_pacman_location;

				break;
			}
			case 1: // The pink ghost will chase the 4th cell in front of Pacman.
			{
				target = i_pacman_location;

				switch (i_pacman_direction)
				{
				case 0:
				{
					target.x += CELL_SIZE * GHOST1_CHASE;

					break;
				}

				case 1:
				{
					target.y -= CELL_SIZE * GHOST1_CHASE;

					break;
				}
				case 2:
				{
					target.x -= CELL_SIZE * GHOST1_CHASE;

					break;
				}
				case 3:
				{
					target.y += CELL_SIZE * GHOST1_CHASE;
				}
				}
				break;
			}
			case 2: // The blue ghost
			{
				target = i_pacman_location;

				// Getting the second cell in front of pacman.
				switch (i_pacman_direction)
				{
				case 0:
				{
					target.x += CELL_SIZE * GHOST2_CHASE;

					break;
				}

				case 1:
				{
					target.y -= CELL_SIZE * GHOST2_CHASE;

					break;
				}

				case 2:
				{
					target.x -= CELL_SIZE * GHOST2_CHASE;

					break;
				}

				case 3:
				{
					target.y += CELL_SIZE * GHOST2_CHASE;
				}
				}

				// We're sending a cell from the red ghost to the blue ghost
				// Then we double it

				target.x += target.x - i_ghost_location.x;
				target.y += target.y - i_ghost_location.y;

				break;
			}
			case 3: //The orange gohst will chase Pacman until it gets close to him. Then it'll switch to the scatter mode.
			{
				// Pythagoras theorem.
				if (CELL_SIZE * Ghost3_CHASE <= sqrt(pow(location.x - i_pacman_location.x, 2) + pow(location.y - i_pacman_location.y, 2)))
				{
					target = i_pacman_location;
				}
				else
				{
					target = { 0, CELL_SIZE * (MAP_HEIGHT - 1) };
				}
			}
			}

		}
	}
}

Location Ghost::Get_Location() { return location; }
