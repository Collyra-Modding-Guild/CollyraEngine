#pragma once

// Dll export configuration (It will never be import here)
#ifdef COLLYRAGAMESYSTEM_CONFIG
#define COLLYRAGAMESYSTEM_API __declspec(dllexport)
#else
#define COLLYRAGAMESYSTEM_API __declspec(dllimport)
#endif

// For exporting functions that will be extern (used by the engine)
#define COLLYRAGAMESYSTEM_FACTORY extern "C" COLLYRAGAMESYSTEM_API

//Same as above, but used as a flag to identify classConstructors
#define COLLYRAGAMESYSTEM_EXPORTS COLLYRAGAMESYSTEM_FACTORY


//Engine Communication--------------
#include "../../CollObject.h"
#include "../../GameObject.h"

