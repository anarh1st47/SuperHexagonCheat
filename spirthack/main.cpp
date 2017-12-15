#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <Windows.h>

#include "Memory.hpp"
#include "Hexagon.hpp"

HANDLE g_Handle;
DWORD g_AppBase;
Offsets g_Offsets;


int main() {
	HWND hwnd;
	DWORD procId;

	do {
		hwnd = FindWindow(NULL, "Super Hexagon");
		std::cout << "Waiting for Super Hexagon..." << std::endl;
		Sleep(1000);
	} while (!hwnd);

	GetWindowThreadProcessId(hwnd, &procId);

	auto handle = OpenProcess(PROCESS_ALL_ACCESS, false, (DWORD)procId);
	g_Handle = handle;
	auto mem = Memory(handle, g_Offsets.base_pointer);
	auto hexagon = SuperHexagon(mem);

	for (;;) {
		auto walls = hexagon.getWalls();
		auto num_slots = hexagon.getCntSlots();
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
			//std::cout << wall.slot << "\t" << wall.distance << std::endl;
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
		hexagon.moveTo(best_slot);
	}
}