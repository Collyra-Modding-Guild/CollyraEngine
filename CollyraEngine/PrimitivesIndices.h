#pragma once

typedef unsigned int uint;

static uint cubeIndices[] =
{
	0, 1, 2,
	2, 3, 0,

	1, 5, 6,
	6, 2, 1,

	7, 6, 5,
	5, 4, 7,

	4, 0, 3,
	3, 7, 4,

	4, 5, 1,
	1, 0, 4,

	3, 2, 6,
	6, 7, 3
};

static uint pyramidIndices[] =
{
	0, 2, 1,
	0, 3, 2,
	0, 4, 3,
	0, 1, 4,

	4, 1, 2,
	2, 3, 4
};
