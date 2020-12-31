// CollyraLibrary.h - Contains declarations of game mode functions
#pragma once

#ifdef COLLYRAGAMESYSTEM_EXPORTS
#define COLLYRAGAMESYSTEM_API __declspec(dllexport)
#else
#define COLLYRAGAMESYSTEM_API __declspec(dllimport)
#endif

extern "C" COLLYRAGAMESYSTEM_API void fibonacci_init(
	const unsigned long long a, const unsigned long long b);

extern "C" COLLYRAGAMESYSTEM_API bool fibonacci_next();

extern "C" COLLYRAGAMESYSTEM_API unsigned long long fibonacci_current();

extern "C" COLLYRAGAMESYSTEM_API unsigned fibonacci_index();

COLLYRAGAMESYSTEM_API int hola1;
