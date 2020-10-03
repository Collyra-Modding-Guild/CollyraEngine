#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "SDL\include\SDL.h"

class Timer
{
public:

	// Constructor
	Timer();

	void Start();
	void StartFrom(uint time);
	void Stop();
	void ReStart();

	Uint32 Read();
	float ReadSec() const;

	bool	running;


private:

	Uint32	started_at;
	Uint32	stopped_at;
};

#endif //__TIMER_H__