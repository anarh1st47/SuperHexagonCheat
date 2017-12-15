#include "Hexagon.hpp"
#include "Memory.hpp"
extern HANDLE g_Handle;


std::vector<Wall_t> SuperHexagon::getWalls() {
	std::vector<Wall_t> walls;
	for (int index = 0; index < memory.read(g_Offsets.cnt_walls); index++) {
		auto address = app_base + g_Offsets.first_wall + index * 0x14;
		unsigned char _buffer[0x14];
		ReadProcessMemory(g_Handle, (LPCVOID)address, &_buffer, 0x14, 0);
		walls.push_back(_buffer);
	}
	return walls;
}
int SuperHexagon::getCntSlots() {
	return memory.read(g_Offsets.cnt_slots);
}
void SuperHexagon::moveTo(int slot) {
	auto numSlots = getCntSlots();
	auto angle = 360 / numSlots * (slot % numSlots) + (180 / numSlots);
	memory.write(g_Offsets.player_angle, angle);
	memory.write(g_Offsets.player_angle_2, angle);
}

