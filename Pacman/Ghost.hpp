#pragma once

class Ghost
{
	// It can be in normal move mode or scatter mode.
	bool movement_mode;

	// Can ghost use door?
	bool can_use_door;

	//Current Direction
	unsigned char Direction;

	// 0 not in frightened mode.
	// 1 starting to get frightened.
	// 2 returns to home.
	unsigned char frightened_mode;

	// To make the ghosts move slowly, it needs to happen after a certain number of frames. so that's why the timer.
	unsigned char frightened_speed_timer;

	// 0 Red.
	// 1 Pink.
	// 2 Blue.
	// 3 Orange.
	unsigned char id;

	unsigned short animation_timer;

	// The ghost will go here when escaping.
	Location home;
	// Define exit for home
	Location home_exit;
	// Current location;
	Location location;
	// Current target
	Location target;

public:
	Ghost(unsigned char i_id);

	bool Pacman_Colission(const Location& i_pacman_location);
	float Get_Target_Distance(unsigned char i_direction);

	void Draw(bool i_flash, sf::RenderWindow& i_window);
	void Reset(const Location& i_home, const Location& i_home_exit);
	void Set_Location(short i_x, short i_y);
	void Switch_Mode();
	void Update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map, Ghost& i_ghost_0, Pacman& pacman);
	void Update_Target(unsigned char i_pacman_direction, const Location& i_ghost_location, const Location& i_pacman_location);

	Location Get_Location();
};