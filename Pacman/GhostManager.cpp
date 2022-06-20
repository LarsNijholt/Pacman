#include <array>
#include <cmath>
#include <SFML/Graphics.hpp>

#include "World.hpp"
#include "Pacman.hpp"
#include "Ghost.hpp"
#include "GhostManager.hpp"

GhostManager::GhostManager() : current_wave(0), wave_timer(LONG_SCATTER_DURATION), ghosts({ Ghost(0), Ghost(1), Ghost(2), Ghost(3) })
{

}

void GhostManager::draw(bool i_flash, sf::RenderWindow& i_window)
{
	for (Ghost& ghost : ghosts)
	{
		ghost.Draw(i_flash, i_window);
	}
}

void GhostManager::reset(unsigned char i_level, const std::array<Location, 4>& i_ghost_locations)
{
	current_wave = 0;

	// Increasing the difficulty
	wave_timer = static_cast<unsigned short>(LONG_SCATTER_DURATION / pow(2, i_level));

	for (unsigned char a = 0; a < 4; a++)
	{
		ghosts[a].Set_Location(i_ghost_locations[a].x, i_ghost_locations[a].y);
	}

	for (Ghost& ghost : ghosts)
	{
		//We use the blue ghost to get the location of the house and the red ghost to get the location of the exit.
		ghost.Reset(ghosts[2].Get_Location(), ghosts[0].Get_Location());
	}
}

void GhostManager::update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map, Pacman& i_pacman)
{
	if (0 == i_pacman.Get_Energizer_Timer()) // We don't increase the difficulty when Pacman is energized.
	{
		if (0 == wave_timer)
		{
			if (7 > current_wave)
			{
				current_wave++;
				for (Ghost& ghost : ghosts)
				{
					ghost.Switch_Mode();
				}
			}

			if (1 == current_wave % 2)
			{
				wave_timer = CHASE_DURATION;
			}
			else if (2 == current_wave)
			{
				wave_timer = static_cast<unsigned short>(LONG_SCATTER_DURATION / pow(2, i_level));
			}
			else
			{
				wave_timer = static_cast<unsigned short>(SHORT_SCATTER_DURATION / pow(2, i_level));
			}
		}
		else
		{
			wave_timer--;
		}
	}

	for (Ghost& ghost : ghosts)
	{
		ghost.Update(i_level, i_map, ghosts[0], i_pacman);
	}
}