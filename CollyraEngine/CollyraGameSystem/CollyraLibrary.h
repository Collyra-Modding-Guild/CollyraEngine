// CollyraLibrary.h - Contains declarations of game mode functions
#pragma once

#ifdef COLLYRAGAMESYSTEM_EXPORTS
#define COLLYRAGAMESYSTEM_API __declspec(dllexport)
#else
#define COLLYRAGAMESYSTEM_API __declspec(dllimport)
#endif

// The Fibonacci recurrence relation describes a sequence F
// where F(n) is { n = 0, a
//               { n = 1, b
//               { n > 1, F(n-2) + F(n-1)
// for some initial integral values a and b.
// If the sequence is initialized F(0) = 1, F(1) = 1,
// then this relation produces the well-known Fibonacci
// sequence: 1, 1, 2, 3, 5, 8, 13, 21, 34, ...

// Initialize a Fibonacci relation sequence
// such that F(0) = a, F(1) = b.
// This function must be called before any other function.
extern "C" COLLYRAGAMESYSTEM_API void fibonacci_init(
	const unsigned long long a, const unsigned long long b);


extern "C" COLLYRAGAMESYSTEM_API bool fibonacci_next();

extern "C" COLLYRAGAMESYSTEM_API unsigned long long fibonacci_current();

extern "C" COLLYRAGAMESYSTEM_API unsigned fibonacci_index();

extern "C"  COLLYRAGAMESYSTEM_API int hola1;
