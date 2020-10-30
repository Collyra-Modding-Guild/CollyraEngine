#include "WG_About.h"
#include "Application.h"
#include <windows.h>

WG_About::WG_About(bool isActive) : WindowGroup(WG_ABOUT, isActive)
{

}

WG_About::~WG_About()
{

}

updateStatus WG_About::Update()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::Begin("About", &active, ImGuiWindowFlags_NoResize);

	ImGui::Text("Engine:");
	ImGui::SameLine();
	ImGui::TextColored({ 255 , 255 , 0 , 100 }, "Collyra Engine");

	ImGui::Text("Engine Version:");
	ImGui::SameLine();
	ImGui::TextColored({ 255 , 255 , 0 , 100 }, "%d.%d", COLLYRA_VERSION_MAJOR, COLLYRA_VERSION_MINOR);

	ImGui::Text("Authors:");
	ImGui::SameLine();
	ImGui::TextColored({ 255 , 255 , 0 , 100 }, "Alex Melenchon & Aaron Guerrero");

	ImGui::Spacing();
	ImGui::Spacing();

	if(ImGui::Button("Engine GitHub Page"))
	{
		ShellExecute(NULL, NULL, ENGINE_GITHUB_URL, NULL, NULL, SW_SHOWNORMAL);	
	}

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Engine License:");
	ImGui::SameLine();
	ImGui::TextColored({ 255 , 255 , 0 , 100 }, "MIT License. See LICENSE file for more information.");

	ImGui::Spacing();

	ImGui::Text("Copyright (c) 2020 Collyra");

	ImGui::End();

	return UPDATE_CONTINUE;
}

void WG_About::Cleanup()
{}