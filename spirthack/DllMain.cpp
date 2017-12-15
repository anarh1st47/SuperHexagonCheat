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

int read(LPCVOID address) {
	char buffer[4];
	ReadProcessMemory(g_Handle, address, &buffer, 2, 0);
	return int((unsigned char)(buffer[3]) << 24 |
		(unsigned char)(buffer[2]) << 16 |
		(unsigned char)(buffer[1]) << 8 |
		(unsigned char)(buffer[0]));
}

class Memory {
	HANDLE handle;
	char buffer[64];
	uint16_t buffer_size = 64;
public:
	Memory(){}
	Memory(HANDLE _handle) {
		handle = _handle;
	}

	int read(LPCVOID address) {
		ReadProcessMemory(this->handle, address, &buffer, 2, 0);
		return int((unsigned char)(buffer[3]) << 24 |
			(unsigned char)(buffer[2]) << 16 |
			(unsigned char)(buffer[1]) << 8 |
			(unsigned char)(buffer[0]));
	}
	unsigned char *read_bytes(int address, int size) {
		unsigned char *_buffer = new unsigned char[4*size+2];
		ReadProcessMemory(handle, (LPCVOID) address, &_buffer, size, 0);
		return _buffer;
	}
	void write(int address, uint32_t value) {
		WriteProcessMemory(handle, (LPVOID)address, &value, 4, 0);
	}
	void close_handle() {
		CloseHandle(handle);
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
	//Memory memory;
public:
	SuperHexagon(Memory &mem) {
		//memory = mem;
		ReadProcessMemory(g_Handle, (LPCVOID)Offsets::base_pointer, &g_AppBase, 4, 0);
	}
	std::vector<Wall_t> get_walls() {
		std::vector<Wall_t> wall_list;
		int num_walls = read((LPCVOID)(g_AppBase + Offsets::num_walls));
		for (int index = 0; index < num_walls; index++) {
			auto address = g_AppBase + Offsets::first_wall + index * 0x14;
			unsigned char _buffer[0x14];
			ReadProcessMemory(g_Handle, (LPCVOID)address, &_buffer, 0x14, 0);
			wall_list.push_back(_buffer);
			__asm nop;
		}
		return wall_list;
	}
	int get_num_slots() {
		return read((LPCVOID)(g_AppBase + Offsets::num_slots));
	}
	void set_player_slot(int slot) {
		auto num_slots = get_num_slots();
		auto angle = 360 / num_slots * (slot % num_slots) + (180 / num_slots);
		WriteProcessMemory(g_Handle, (LPVOID)(g_AppBase + Offsets::player_angle), &angle, 4, 0);
		WriteProcessMemory(g_Handle, (LPVOID)(g_AppBase + Offsets::player_angle_2), &angle, 4, 0);
	}
	int get_current_slot() {
		return read((LPCVOID)(g_AppBase + Offsets::first_wall));
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
	auto mem = Memory(handle);
	auto hexagon = SuperHexagon(mem);

	for (;;) {
		auto walls = hexagon.get_walls();
		auto num_slots = hexagon.get_num_slots();
		//std::vector<int> data;
		std::vector<int> slots;
		//data.insert(data.end(), 30, -1);
		slots.insert(slots.end(), num_slots, 10000000);

		system("cls");

		for (auto wall : walls) {
			auto slot = wall.slot;
			auto distance = wall.distance;
			auto width = wall.width;
			/*if (0 < distance &&  distance < 1000000 && width > 0 && -1 < slot && slot < num_slots) {
				auto bExists = false;
				for (auto _ : data) {
					if (_ != -1) {
						bExists = true;
						break;
					}
				}
				if (bExists) {
					if (distance < data[slot])
						data[slot] = distance;
				}
				else
					data[slot] = distance;
			}*/
			if (slots.size() <= slot) {
				continue;
				//slots.insert(slots.end(), slot-slots.size+1, 10000000);
			}
			if (slots[slot] > distance && distance !=0) {
				slots[slot] = distance;
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
			/*else if (slots[i] == -1) {
				best_slot = i;
				break;
			}*/
			else if (slots[i] == 0) { // no walls
				best_slot = i;
				break;
			}
			//break;
		}
		
		std::cout << "Setting to: " << best_slot << std::endl;
		hexagon.set_player_slot(best_slot);
		/*if (data.size()) { //fixme
			//move here
			//std::cout << "moving.."<<std::endl;
			int maxdst = 1, best_slot = -2;
			for (int i = 0; i < data.size(); i++) {
				if (data[i] > maxdst) {
					maxdst = data[i];
					best_slot = i;
				}
				else if (data[i] == -1) {
					best_slot = i;
					break;
				}
					//break;
			}
			std::cout << best_slot << "\t" << hexagon.get_current_slot() << std::endl;;
			if(best_slot)
				hexagon.set_player_slot(best_slot);
			}*/
		//Sleep(100);
	}

	__asm nop;
}