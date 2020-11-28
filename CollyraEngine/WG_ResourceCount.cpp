#include "WG_ResourceCount.h"
#include "Globals.h"
#include "Application.h"
#include "M_Resources.h"
#include "R_Resource.h"


WG_ResourceCount::WG_ResourceCount(bool isActive) : WindowGroup(WG_RESOURCECOUNT, isActive)
{}

WG_ResourceCount::~WG_ResourceCount()
{}

updateStatus WG_ResourceCount::Update()
{
	ImGui::Begin("ResourceCount");

	DrawResourceType(R_TYPE::TEXTURE);
	DrawResourceType(R_TYPE::MESH);
	DrawResourceType(R_TYPE::SCENE);
	DrawResourceType(R_TYPE::MATERIAL);

	ImGui::End();


	return UPDATE_CONTINUE;
}

void WG_ResourceCount::Cleanup()
{}

void WG_ResourceCount::DrawResourceType(R_TYPE type)
{
	std::vector<const Resource*> resources;

	static const char* titles[] = {
		 "Models","Scenes", "Mesh","Textures","Material" };

	if (ImGui::TreeNodeEx(titles[(int)type], 0))
	{
		App->resources->GetResourcesByType(resources, type);

		for (int i = 0; i < resources.size(); i++)
		{
			const Resource* currResource = resources[i];

			if (ImGui::TreeNodeEx(currResource->GetName().c_str(), ImGuiTreeNodeFlags_Leaf))
			{
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("UID: %llu", currResource->GetUid());
					ImGui::Text("Source: %s", currResource->GetLibraryPath().c_str());
					ImGui::Text("References: %u", currResource->referenceCount);
					ImGui::EndTooltip();
				}

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
}

