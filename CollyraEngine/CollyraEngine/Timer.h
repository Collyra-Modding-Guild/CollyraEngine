#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "SDL\include\SDL.h"

class __declspec(dllexport) Timer
{
public:

	// Constructor
	Timer(bool isRunning = true);
	~Timer();
	

	void Start();
	void StartFrom(uint time);
	void Stop();

	void Pause();
	void Resume();
	void Tick(bool step);
	
	Uint32 Read();
	float ReadSec() const;

	bool	running;
	bool	pause;
	bool    tick;

private:

	Uint32	started_at;
	Uint32	stopped_at;
};

#endif //__TIMER_H__