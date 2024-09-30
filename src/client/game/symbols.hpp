#pragma once

#include "structs.hpp"

#define WEAK __declspec(selectany)

namespace game
{
	// Re-implementations
	bool I_islower(int c);
	bool I_isupper(int c);

	unsigned int Scr_CanonHash(const char* str);
}
