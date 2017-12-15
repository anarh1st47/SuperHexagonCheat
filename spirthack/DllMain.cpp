#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>
HANDLE g_Handle;
DWORD g_AppBase;

namespace Offsets {
	DWORD base_pointer = (DWORD)0x694B00;
	DWORD num_slots = (DWORD)0x1BC;
	DWORD num_walls = (DWORD)0x2930;
	DWORD first_wall = (DWORD)0x220;
	DWORD	player_angle = (DWORD)0x2958;
	DWORD	player_angle_2 = (DWORD)0x2954;
	DWORD	mouse_down_left = (DWORD)0x42858;
	DWORD	mouse_down_right = (DWORD)0x4285A;
	DWORD	mouse_down = (DWORD)0x42C45;
	DWORD	world_angle = (DWORD)0x1AC;
};

class Memory {
	HANDLE handle;
	char buffer[64];
	DWORD base;
public:
	Memory(){}
	Memory(HANDLE _handle, DWORD _base) {
		handle = _handle;
		base = _base;
	}

	DWORD readAppBase(DWORD address) {
		DWORD appBase;
		ReadProcessMemory(handle, (LPCVOID)(address), &appBase, 4, 0);
		base = appBase;
		return base;
	}

	int read(DWORD address) {
		ReadProcessMemory(handle, (LPCVOID)(base + address), &buffer, 2, 0);
		return int((unsigned char)(buffer[3]) << 24 |
			(unsigned char)(buffer[2]) << 16 |
			(unsigned char)(buffer[1]) << 8 |
			(unsigned char)(buffer[0]));
	}

	unsigned char *readBytes(DWORD address, int size) {
		unsigned char *_buffer = new unsigned char[4*size+2];
		ReadProcessMemory(handle, (LPCVOID)(base + address), &_buffer, size, 0);
		return _buffer;
	}

	void write(int address, uint32_t value) {
		WriteProcessMemory(handle, (LPVOID)(base + address), &value, 4, 0);
	}
};

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
		app_base = memory.readAppBase(Offsets::base_pointer);
	}
	std::vector<Wall_t> get_walls() {
		std::vector<Wall_t> wall_list;
		int num_walls = memory.read(Offsets::num_walls);
		for (int index = 0; index < num_walls; index++) {
			auto address = app_base + Offsets::first_wall + index * 0x14;
			unsigned char _buffer[0x14];
			ReadProcessMemory(g_Handle, (LPCVOID)address, &_buffer, 0x14, 0);
			wall_list.push_back(_buffer);
			__asm nop;
		}
		return wall_list;
	}
	int get_num_slots() {
		return memory.read(Offsets::num_slots);
	}
	void set_player_slot(int slot) {
		auto num_slots = get_num_slots();
		auto angle = 360 / num_slots * (slot % num_slots) + (180 / num_slots);
		memory.write(Offsets::player_angle, angle);
		memory.write(Offsets::player_angle_2, angle);
	}
	int get_current_slot() {
		return memory.read(Offsets::first_wall);
	}
};


int main() {
	auto hwnd = FindWindow(NULL, "Super Hexagon");
	DWORD procId;

	while (!hwnd) {
		hwnd = FindWindow(NULL, "Super Hexagon");
		std::cout << "Game window not found." << std::endl;
		Sleep(3000);
		exit(-1);
	}

	GetWindowThreadProcessId(hwnd, &procId);

	auto handle = OpenProcess(PROCESS_ALL_ACCESS, false, (DWORD)procId);
	g_Handle = handle;
	auto mem = Memory(handle, Offsets::base_pointer);
	auto hexagon = SuperHexagon(mem);

	for (;;) {
		auto walls = hexagon.get_walls();
		auto num_slots = hexagon.get_num_slots();
		std::vector<int> slots;
		slots.insert(slots.end(), num_slots, 10000000);

		system("cls");

		for (auto wall : walls) {
			if (slots.size() <= wall.slot) {
				continue;
			}
			if (slots[wall.slot] > wall.distance && wall.distance !=0) {
				slots[wall.slot] = wall.distance;
			}
			std::cout << wall.slot << "\t" << wall.distance << std::endl;
		}
		int maxdst = 1, best_slot = -2;
		
		std::cout << "slots:" << std::endl;
		for (int i = 0; i < slots.size(); i++) {
			std::cout << i << "\t" << slots[i] << std::endl;
			if (slots[i] > maxdst) {
				maxdst = slots[i];
				best_slot = i;
			}
			else if (slots[i] == 0) { // no walls
				best_slot = i;
				break;
			}
		}
		
		std::cout << "Setting to: " << best_slot << std::endl;
		hexagon.set_player_slot(best_slot);
	}

	__asm nop;
}