#pragma once
#include <Windows.h>
#include <iostream> // uint32_t

class Offsets {
public:
	DWORD base_pointer = (DWORD)0x694B00;
	DWORD cnt_slots = (DWORD)0x1BC;
	DWORD cnt_walls = (DWORD)0x2930;
	DWORD first_wall = (DWORD)0x220;
	DWORD player_angle = (DWORD)0x2958;
	DWORD player_angle_2 = (DWORD)0x2954;
};
extern Offsets g_Offsets;

class Memory {
	HANDLE handle;
	char buffer[64];
	DWORD base;
public:
	Memory() {}
	Memory(HANDLE _handle, DWORD _base) {
		handle = _handle;
		base = _base;
	}
	DWORD readAppBase(DWORD address);
	int read(DWORD address);
	void write(int address, uint32_t value);
};
