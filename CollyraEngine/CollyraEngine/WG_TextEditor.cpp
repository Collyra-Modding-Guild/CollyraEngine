#include "WG_TextEditor.h"

#include "Application.h"
#include "M_FileManager.h"
#include "M_Scripting.h"

WG_TextEditor::WG_TextEditor(bool isActive) : WindowGroup(WG_TEXT_EDITOR, isActive), currFile(""), currFileName(""), fileModTime(0)
{
	textEditor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());
}

WG_TextEditor::~WG_TextEditor()
{
}

updateStatus WG_TextEditor::Update()
{
	ImGui::Begin("TextPanelEditor");

	std::map<std::string, uint64>* allScriptFiles = App->scriptInterface->GetFileMap();

	std::map<std::string, uint64>::iterator currFileIterator = allScriptFiles->end();
	std::map<std::string, uint64>::iterator fileIterator = allScriptFiles->begin();

	ImGui::Text("Current File: ");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(110.0f);
	if (ImGui::BeginCombo("##filestoEdit", currFileName.c_str()))
	{
		for (int n = 0; n < allScriptFiles->size(); n++)
		{
			bool is_selected = (currFile == fileIterator->first);

			std::string name = "", extension = "";
			App->physFS->SplitFilePath(fileIterator->first.c_str(), nullptr, nullptr, &name, &extension);
			std::string out = name + "." + extension;

			if (ImGui::Selectable(out.c_str(), is_selected))
			{
				SetFile(fileIterator->first.c_str(), fileIterator->second);
			}

			if (!currFileName.empty() && currFile == fileIterator->first)
			{
				currFileIterator = fileIterator;
			}
			fileIterator++;
		}

		ImGui::EndCombo();
	}

	//Check for delete & modify
	if (!App->physFS->Exists(currFile.c_str()))
	{
		currFile.clear();
		currFileName.clear();
	}
	else if (currFileIterator != allScriptFiles->end())
	{
		if (fileModTime < currFileIterator->second && textEditor.IsTextChanged() == false)
			SetFile(currFileIterator->first.c_str(), currFileIterator->second);
	}


	ImGui::SameLine();

	if (ImGui::Button("Save"))
	{
		if (!currFile.empty())
		{
			std::string saveText = textEditor.GetText();
			App->physFS->Save(currFile.c_str(), saveText.data(), saveText.size());
		}
		else
			LOG("No File Opened! Can't save!");

	}

	if (!currFile.empty())
	{
		textEditor.Render("TextEditor");
	}

	ImGui::End();
	return UPDATE_CONTINUE;
}

void WG_TextEditor::Cleanup()
{
}

void WG_TextEditor::SetFile(const char* newFile, uint64 modTime)
{

	char* buffer = nullptr;

	uint size = App->physFS->Load(newFile, &buffer);

	if (size > 0)
	{
		currFile = newFile;
		textEditor.SetText(std::string(buffer));

		std::string name = "", extension = "";
		App->physFS->SplitFilePath(newFile, nullptr, nullptr, &name, &extension);
		std::string out = name + "." + extension;

		currFileName = out;
		fileModTime = modTime;

		RELEASE(buffer);
	}
	else
		LOG("Could not load file, please check the path");

}
