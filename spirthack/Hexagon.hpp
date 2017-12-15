#pragma once
#include "Memory.hpp"
#include <vector>
#include <Windows.h> //dword

struct Wall_t {
	int slot, distance, width;
	Wall_t(unsigned char *buffer) {
		width = int((unsigned char)(buffer[11]) << 24 |
			(unsigned char)(buffer[10]) << 16 |
			(unsigned char)(buffer[9]) << 8 |
			(unsigned char)(buffer[8]));
		distance = int((unsigned char)(buffer[7]) << 24 |
			(unsigned char)(buffer[6]) << 16 |
			(unsigned char)(buffer[5]) << 8 |
			(unsigned char)(buffer[4]));
		slot = int((unsigned char)(buffer[3]) << 24 |
			(unsigned char)(buffer[2]) << 16 |
			(unsigned char)(buffer[1]) << 8 |
			(unsigned char)(buffer[0]));
	}
};


class SuperHexagon {
	Memory memory;
	DWORD app_base;
public:
	SuperHexagon(Memory &mem) {
		memory = mem;
		app_base = memory.readAppBase(g_Offsets.base_pointer);
	}
	std::vector<Wall_t> getWalls();
	int getCntSlots();
	void moveTo(int slot);
};

