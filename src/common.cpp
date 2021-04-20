

#include "common.h"

uint32_t hashString(const std::string& str)
{
	uint32_t hash = 2166136261u;

	for (int i = 0; i < str.size(); i++) {
		hash ^= str[i];
		hash *= 16777619;
	}
	return hash;
}