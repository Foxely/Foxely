#include <iostream>
#include <fstream>

extern "C" void elfgen(unsigned char *asem, int len)
{
	std::ofstream gen;
	gen.open("gen.s");
	gen << ".text\n.globl main\nmain: .byte ";
	for (int i = 0; i < len; i++)
	{
		gen << "0x" << std::hex << (int)asem[i];
		if (i != len - 1)
			gen << ", ";
	}
	gen << "\n";
	gen.close();

	system("gcc gen.s -o gen && rm gen.s");
};