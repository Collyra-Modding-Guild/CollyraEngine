// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "Timer.h"

// ---------------------------------------------
Timer::Timer(bool isRunning) : tick(false)
{
	running = isRunning;

	if(running)
		Start();
}

Timer::~Timer()
{ }

// ---------------------------------------------
void Timer::Start()
{
	running = true;
	started_at = SDL_GetTicks();
}

void Timer::StartFrom(uint time)
{
	started_at = SDL_GetTicks() - time;
}

// ---------------------------------------------
void Timer::Stop()
{
	running = false;
}

// ---------------------------------------------
void Timer::Resume()
{
	running = true;
}

void Timer::Tick(bool step)
{
	tick = step;
	running = step;
}


// ---------------------------------------------
void Timer::Pause()
{
	running = false;
	stopped_at = SDL_GetTicks();
}


// ---------------------------------------------
Uint32 Timer::Read()
{
	if(running == true)
	{
		return SDL_GetTicks() - started_at;
	}
	else
	{
		return stopped_at - started_at;
	}
}

float Timer::ReadSec() const
{
	if (running == true)
	{
		return (SDL_GetTicks() - started_at) / 1000.0f;
	}
	else
	{
		return (stopped_at - started_at) / 1000.0f;
	}
}


