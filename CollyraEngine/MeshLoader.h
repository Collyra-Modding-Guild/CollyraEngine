#ifndef __MeshLoader_H__
#define __MeshLoader_H__

#include <vector>

class Mesh;

namespace MeshLoader
{
	void Init();

	void CleanUp();

	std::vector<Mesh> Load(const char* path);

}

#endif //__MeshLoader_H__