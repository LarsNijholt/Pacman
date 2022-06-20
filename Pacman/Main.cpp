#include <array>
#include <chrono>
#include <ctime>
#include <SFML/Graphics.hpp>

#include "World.hpp"
#include "DrawText.hpp"
#include "Pacman.hpp"
#include "Ghost.hpp"
#include "GhostManager.hpp"
#include "DrawMap.hpp"
#include "ConvertSketch.hpp"
#include "MapCollision.hpp"

int main()
{
	// Did we win the game?
	bool won = 0;

	// Make the game frame rate independant.
	unsigned lag = 0;

	unsigned char level = 0;

	// Similar to lag, used to make game framerate independent.
	std::chrono::time_point<std::chrono::steady_clock> previous_time;

	std::array<std::string, MAP_HEIGHT> map_sketch = {
		" ################### ",
		" #........#........# ",
		" #o##.###.#.###.##o# ",
		" #.................# ",
		" #.##.#.#####.#.##.# ",
		" #....#...#...#....# ",
		" ####.### # ###.#### ",
		"    #.#   0   #.#    ",
		"#####.# ##=## #.#####",
		"     .  #123#  .     ",
		"#####.# ##### #.#####",
		"    #.#       #.#    ",
		" ####.# ##### #.#### ",
		" #........#........# ",
		" #.##.###.#.###.##.# ",
		" #o.#.....P.....#.o# ",
		" ##.#.#.#####.#.#.## ",
		" #....#...#...#....# ",
		" #.######.#.######.# ",
		" #.................# ",
		" ################### "
	};

	std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> map{};

	// Initial ghost positions.
	std::array<Location, 4> ghost_locations;

	// SFML event handler.
	sf::Event event;

	sf::RenderWindow window(sf::VideoMode(CELL_SIZE * MAP_WIDTH * SCREEN_RESIZE, (FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT) * SCREEN_RESIZE), "Pac-Man", sf::Style::Close);

	// Resizing the window
	window.setView(sf::View(sf::FloatRect(0, 0, CELL_SIZE * MAP_WIDTH, FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT)));

	GhostManager ghostmanager;

	Pacman pacman;

	// Generating a random seed.
	srand(static_cast<unsigned>(time(0)));

	map = convert_sketch(map_sketch, ghost_locations, pacman);

	ghostmanager.reset(level, ghost_locations);

	// Get the current time and store it in a variable.
	previous_time = std::chrono::steady_clock::now();

	while (1 == window.isOpen())
	{
		// Here we're calculating the lag.
		unsigned delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time).count();

		lag += delta_time;

		previous_time += std::chrono::microseconds(delta_time);

		// While the lag exceeds the maximum allowed frame duration.
		while (FRAME_DURATION <= lag)
		{
			// We decrease the lag.
			lag -= FRAME_DURATION;

			while (1 == window.pollEvent(event))
			{
				switch (event.type)
				{
					case sf::Event::Closed:
					{
						// Making sure the player can close the window
						window.close();
					}
				}
			}

			if (0 == won && 0 == pacman.Get_Dead())
			{
				won = 1;

				pacman.Update(level, map);
				ghostmanager.update(level, map, pacman);

				// We're checking every cell in the map.
				for (const std::array<Cell, MAP_HEIGHT>& column : map)
				{
					for (const Cell& cell : column)
					{
						if (Cell::Pellet == cell) // And if at least one of them has a pellet.
						{
							won = 0; // The game is not yet won.

							break;
						}
					}

					if (0 == won)
					{
						break;
					}
				}

				if (1 == won)
				{
					pacman.Set_Animation_Timer(0);
				}
			}
			else if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) // Restarting the game.
			{
				won = 0;

				if (1 == pacman.Get_Dead())
				{
					level = 0;
				}
				else
				{
					// After each win we reduce the duration of attack waves and Energizers
					level++;
				}

				map = convert_sketch(map_sketch, ghost_locations, pacman);

				ghostmanager.reset(level, ghost_locations);

				pacman.Reset();
			}

			if (FRAME_DURATION > lag)
			{
				window.clear();

				if (0 == won && 0 == pacman.Get_Dead())
				{
					DrawMap(map, window);

					ghostmanager.draw(GHOST_FLASH_START >= pacman.Get_Energizer_Timer(), window);

					DrawText(0, 0, CELL_SIZE * MAP_HEIGHT, "Level: " + std::to_string(1 + level), window);
				}

				pacman.Draw(won, window);

				if (1 == pacman.Get_Animation_Over())
				{
					if (1 == won)
					{
						DrawText(1, 0, 0, "Next level!", window);
					}
					else
					{
						DrawText(1, 0, 0, "Game over", window);
					}
				}
				window.display();
			}
		}
	}
}