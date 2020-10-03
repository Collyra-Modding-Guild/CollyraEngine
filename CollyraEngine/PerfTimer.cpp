// ----------------------------------------------------
// PerfTimer.cpp
// Slow timer with microsecond precision
// ----------------------------------------------------

#include "PerfTimer.h"
#include "SDL\include\SDL_timer.h"

uint64 PerfTimer::frequency = 0;

// ---------------------------------------------
PerfTimer::PerfTimer()
{
	Start();
}

// ---------------------------------------------
void PerfTimer::Start()
{
	started_at = SDL_GetPerformanceCounter();
	frequency = SDL_GetPerformanceFrequency();
}

// ---------------------------------------------
double PerfTimer::ReadMs() const
{
	return ((SDL_GetPerformanceCounter() - started_at) * 1000.0f) / (double)frequency;

}

// ---------------------------------------------
uint64 PerfTimer::ReadTicks() const
{
	return (SDL_GetPerformanceCounter() - started_at);
}