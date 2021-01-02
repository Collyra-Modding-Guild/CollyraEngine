#pragma once

#include "../CollyraFramework.h"

extern "C" COLLYRAGAMESYSTEM_API void fibonacci_init(
	const unsigned long long a, const unsigned long long b);

extern "C" COLLYRAGAMESYSTEM_API bool fibonacci_next();

extern "C" COLLYRAGAMESYSTEM_API unsigned long long fibonacci_current();

extern "C" COLLYRAGAMESYSTEM_API unsigned fibonacci_index();