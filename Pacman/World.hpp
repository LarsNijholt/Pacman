#pragma once

constexpr unsigned char CELL_SIZE = 16;
constexpr unsigned char FONT_HEIGHT = 16;

// start counting from 0 and the pink ghost will target the cell in front of pacman.
// this is the second ghost.
constexpr unsigned char GHOST1_CHASE = 2;
// which cell in front of pacman the blue ghost will use to calculate the target.
constexpr unsigned char GHOST2_CHASE = 1;
// if the distance between the orange ghost and pacman is less than this value, the orange ghost will go into scatter mode.
constexpr unsigned char Ghost3_CHASE = 4;
// Ghost animation frames.
constexpr unsigned char GHOST_ANIM_FRAMES = 6;
// Ghost animation speed.
constexpr unsigned char GHOST_ANIM_SPEED = 4;
// The speed of the ghosts after pacman energizers.
constexpr unsigned char GHOST_ESCAPE_SPEED = 1.7;
// The higher this value the slower the ghost
constexpr unsigned char GHOST_FRIGHTENED_SPEED = 1.2f;

constexpr unsigned char GHOST_SPEED = 1;
constexpr unsigned char MAP_WIDTH = 21;
constexpr unsigned char MAP_HEIGHT = 21;
constexpr unsigned char PACMAN_ANIM_FRAMES = 6;
constexpr unsigned char PACMAN_ANIM_SPEED = 4;
constexpr unsigned char PACMAN_DEATH_FRAMES = 12;
constexpr unsigned char PACMAN_SPEED = 2;
constexpr unsigned char SCREEN_RESIZE = 2;

// Values below are in frames.
constexpr unsigned short CHASE_DURATION = 1024;
constexpr unsigned short ENERGIZE_DURATION = 512;
constexpr unsigned short FRAME_DURATION = 16667;
constexpr unsigned short GHOST_FLASH_START = 64;
constexpr unsigned short LONG_SCATTER_DURATION = 512;
constexpr unsigned short SHORT_SCATTER_DURATION = 256;

enum Cell
{
	Empty,
	Door,
	Energizer,
	Pellet,
	Wall
};

struct Location
{
	short x;
	short y;

	bool operator==(const Location& i_position)
	{
		return this->x == i_position.x && this->y == i_position.y;
	}
};
