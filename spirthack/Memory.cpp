#include "Memory.hpp"


DWORD Memory::readAppBase(DWORD address) {
	DWORD appBase;
	ReadProcessMemory(handle, (LPCVOID)(address), &appBase, 4, 0);
	base = appBase;
	return base;
}

int Memory::read(DWORD address) {
	ReadProcessMemory(handle, (LPCVOID)(base + address), &buffer, 2, 0);
	return int((unsigned char)(buffer[3]) << 24 |
		(unsigned char)(buffer[2]) << 16 |
		(unsigned char)(buffer[1]) << 8 |
		(unsigned char)(buffer[0]));
}

void Memory::write(int address, uint32_t value) {
	WriteProcessMemory(handle, (LPVOID)(base + address), &value, 4, 0);
}
