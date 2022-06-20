#pragma once

class Pacman
{
	// used for death animation.
	bool animation_over;
	bool isDead;

	unsigned char direction;

	// timers.
	unsigned short animation_timer;
	unsigned short energizer_timer;

	// Pacman location
	Location location;

public:
	Pacman();

	bool Get_Animation_Over();
	bool Get_Dead();

	unsigned char Get_Direction();

	unsigned short Get_Energizer_Timer();

	void Draw(bool i_victory, sf::RenderWindow& i_window);
	void Reset();
	void Set_Animation_Timer(unsigned short i_animation_timer);
	void set_Dead(bool i_dead);
	void Set_Position(short i_x, short i_y);
	void Update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& i_map);

	Location Get_Location();
};