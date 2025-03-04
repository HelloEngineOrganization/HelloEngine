#include "Headers.h"
#include "ImWindowProject.h"
#include "ModuleFiles.h"
#include "FileTree.hpp"
#include "ModuleResourceManager.h"

#include "ModuleWindow.h"
#include "ModuleLayers.h"
#include "TextureImporter.h"
#include "ModuleLayers.h"
#include "LayerEditor.h"


ImWindowProject::ImWindowProject()
{
	windowName = "Project";

	isEnabled = true;

    _app = Application::Instance();

    _window = ModuleWindow::window;

    ModuleResourceManager::S_GetFileTree(_fileTree);

    // Init delete object;
    _deleteFile = nullptr;

    // Init rootNode & currentNode
    UpdateFileNodes();

    // Add onDropListener event
    ModuleInput::S_AddOnDropListener(std::bind(&ImWindowProject::OnDrop, this, std::placeholders::_1));

    // Load Icons 
    {
        char* buffer = nullptr;
        int size = ModuleFiles::S_Load("Resources/Editor/Images/files.dds", &buffer);
        _fileImageID = TextureImporter::LoadEditorDDS(buffer, size);
        RELEASE(buffer);

        buffer = nullptr;
        size = ModuleFiles::S_Load("Resources/Editor/Images/folder.dds", &buffer);
        _folderImageID = TextureImporter::LoadEditorDDS(buffer, size);
        RELEASE(buffer);

        buffer = nullptr;
        size = ModuleFiles::S_Load("Resources/Editor/Images/grid.dds", &buffer);
        _meshImageID = TextureImporter::LoadEditorDDS(buffer, size);
        RELEASE(buffer);

        buffer = nullptr;
        size = ModuleFiles::S_Load("Resources/Editor/Images/modelOpen.dds", &buffer);
        _modelImageID = TextureImporter::LoadEditorDDS(buffer, size);
        RELEASE(buffer)

        buffer = nullptr;
        size = ModuleFiles::S_Load("Resources/Editor/Images/scene.dds", &buffer);
        _sceneImageID = TextureImporter::LoadEditorDDS(buffer, size);
        RELEASE(buffer)

        buffer = nullptr;
        size = ModuleFiles::S_Load("Resources/Editor/Images/image.dds", &buffer);
        _textureImageID = TextureImporter::LoadEditorDDS(buffer, size);
        RELEASE(buffer)

        buffer = nullptr;
        size = ModuleFiles::S_Load("Resources/Editor/Images/h.dds", &buffer);
        _hImageID = TextureImporter::LoadEditorDDS(buffer, size);
        RELEASE(buffer)

        buffer = nullptr;
        size = ModuleFiles::S_Load("Resources/Editor/Images/cpp.dds", &buffer);
        _cppImageID = TextureImporter::LoadEditorDDS(buffer, size);
        RELEASE(buffer)
    }   
}

ImWindowProject::~ImWindowProject()
{
    ModuleInput::S_ClearOnDropListener();
}

void ImWindowProject::Update()
{
    CheckWindowFocus();

    if (_showDeleteMessage)
        DrawDeleteMessage();

   	if (ImGui::Begin(windowName.c_str(), &isEnabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar))
	{
        // Options, Filter
        //if (ImGui::Button("Options")) ImGui::OpenPopup("Options");
        //ImGui::SameLine();
        //filter.Draw("Filter (\"incl,-excl\") (\"error\")", 180);
        //ImGui::SameLine();

        // For change directory
        Directory* newDir = nullptr;

        // Resize children width
        // Warning, is static!!!
        static float widthLeft = 200; // Init Size child 1
        static float widthRight = 1200; // Init Size child 2

        // Adjust window size
        ImVec2 windowSize = ImGui::GetWindowSize();

        ImGui::DrawSplitter(0, 10, &widthLeft, &widthRight, 100, 200);

        widthRight = (windowSize.x - widthLeft - 20);

        // Left window
        if (widthLeft > 0)
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.16f, 0.16f, 0.16f, 1));

            if (ImGui::BeginChild("ChildL", ImVec2(widthLeft, 0), true, ImGuiWindowFlags_HorizontalScrollbar))
                DrawTreeNodePanelLeft(newDir, _rootNode, false);
            ImGui::EndChild();

            ImGui::PopStyleColor(1);
        }

        ImGui::SameLine();

        // Right window
        if (widthRight > 0)
        {
            if (ImGui::BeginChild("ChildR", ImVec2(widthRight, 0), true, ImGuiWindowFlags_HorizontalScrollbar))
                DrawTreeNodePanelRight(newDir);
            ImGui::EndChild();
        }
        
        // Change current directory
        if (newDir)
            _fileTree->_currentDir = newDir;
    }
    ImGui::End();

    if (_openCreateFolderPanel)
        PanelCreateFolder();

    else if (_openCreateScriptPanel)
        PanelCreateScript();

    // If have any file to delete, delete this
    if (_deleteFile && _deleteFileAccepted)
    {
        ModuleFiles::S_Delete(_deleteFile->path);

        if (_deleteFile->metaPath != "none")
            ModuleResourceManager::S_DeleteMetaFile(_deleteFile->metaPath);

        _deleteFile = nullptr;
        _deleteFileAccepted = false;

        UpdateFileNodes();
    }

    // If have any folder to delete, delete this
    if(_deleteDir && _deleteFileAccepted)
    {
        ModuleFiles::S_Delete(_deleteDir->path);

        UpdateFileNodes();

        _deleteDir = nullptr;
        _deleteFileAccepted = false;
    }
}

void ImWindowProject::DrawTreeNodePanelLeft(Directory*& newDir, Directory* node, const bool drawFiles) const
{
    ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_OpenOnArrow;

    if(node == _fileTree->_currentDir)
        node_flags |= ImGuiTreeNodeFlags_Selected;

    if(node->directories.empty())
        node_flags |= ImGuiTreeNodeFlags_Leaf;

    if (ImGui::TreeNodeEx(node->name.c_str(), node_flags))
    {
        // Slect node
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            newDir = node;

        // Recursive functions
        for (int i = 0; i < node->directories.size(); i++)
            DrawTreeNodePanelLeft(newDir, node->directories[i], drawFiles);

        // Draw Files
        if(drawFiles)
            for (int i = 0; i < node->files.size(); i++)
            {
                ImGui::Text(node->files[i].name.c_str());
            }
                
        ImGui::TreePop();
    }
}

void ImWindowProject::DrawTreeNodePanelRight(Directory*& newDir)
{
    // Return buttons
    {
        Directory* parent1 = nullptr;
        Directory* parent2 = nullptr;

        if (_fileTree->_currentDir->parent)
            parent1 = _fileTree->_currentDir->parent;

        if (parent1 && parent1->parent)
            parent2 = parent1->parent;

        if (parent2)
        {
            std::string pName = parent2->name + " > ##Return";

            if (ImGui::Button(pName.c_str()))
                newDir = parent2;
        }
        if (parent1)
        {
            ImGui::SameLine();
            std::string pName = parent1->name + " > ##Return";

            if (ImGui::Button(pName.c_str()))
                newDir = parent1;
        }

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4, 0.4, 0.4, 255));

        if (ImGui::Button(_fileTree->_currentDir->name.c_str()))
        {
            // Do nothing here, just fixed space
        }
           
        ImGui::PopStyleColor(1);
    }
    
    ImGui::Separator();

    // Calculate num of colomns we can have
    int numOfColumns = (ImGui::GetContentRegionAvail().x / _itemWidth) -1;

    if (numOfColumns == 0)
        numOfColumns++;

    ImGui::Columns(numOfColumns, "files columns", false);

    // Folders
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3, 0.3, 0.3, 0));
    for (int i = 0; i < _fileTree->_currentDir->directories.size(); i++)
    {
        std::string directoryID = std::to_string(i).c_str() + _fileTree->_currentDir->directories[i]->name;

        if (ImGui::ImageButton(directoryID.c_str(), (ImTextureID)_folderImageID, ImVec2(_itemWidth, _itemHeight)))
            newDir = _fileTree->_currentDir->directories[i];
        
        // Right click
        if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
        {
            if (ImGui::Button("Delete##Folder"))
            {
                _deleteDir = _fileTree->_currentDir->directories[i];
                _showDeleteMessage = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(_fileTree->_currentDir->directories[i]->name.c_str());

        ImGui::TextWrapped(_fileTree->_currentDir->directories[i]->name.c_str());

        ImGui::NextColumn();
    }
    ImGui::PopStyleColor(1);

    // Files
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3, 0.3, 0.3, 0));
    for (int i = 0; i < _fileTree->_currentDir->files.size(); i++)
    {
        // Get file icon ID
        uint icon = 0;
        switch (ModuleFiles::S_GetResourceType(_fileTree->_currentDir->files[i].path))
        {
        case ResourceType::MODEL:
            icon = _modelImageID;
            break;
        case ResourceType::SCENE:
            icon = _sceneImageID;
            break;
        case ResourceType::TEXTURE:         
            icon = _textureImageID;
            break;
        case ResourceType::HSCRIPT:
            icon = _hImageID;
            break;
        case ResourceType::CPPSCRIPT:
            icon = _cppImageID;
            break;
        default:
            icon = _fileImageID;
            break;
        }

        std::string fileID = std::to_string(i).c_str() + _fileTree->_currentDir->files[i].name;

        // Draw Icon button
        if (ImGui::ImageButton(fileID.c_str(), (ImTextureID)icon, ImVec2(_itemWidth, _itemHeight)))
            _fileTree->_currentDir->files[i].pressed = !_fileTree->_currentDir->files[i].pressed;

        // Drag file
        ResourceType type = ModuleFiles::S_GetResourceType(_fileTree->_currentDir->files[i].name);

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
        {
            switch (type)
            {
            case ResourceType::TEXTURE:
                // Find resource path
                _dragUID = _fileTree->_currentDir->files[i].metaFile.UID;

                // Set payload to carry the index of our item (could be anything)
                ImGui::SetDragDropPayload("Texture", &_dragUID, sizeof(uint));
                break;
            case ResourceType::MODEL:
                _dragUID = _fileTree->_currentDir->files[i].metaFile.UID;

                ImGui::SetDragDropPayload("Model", &_dragUID, sizeof(uint));
                break;
            case ResourceType::SCENE:
                ImGui::SetDragDropPayload("Scene", &_fileTree->_currentDir->files[i].path, sizeof(std::string));
                break;
            case ResourceType::HSCRIPT:
            case ResourceType::CPPSCRIPT:
                _dragUID = _fileTree->_currentDir->files[i].metaFile.UID;
                ImGui::SetDragDropPayload("Script", &_dragUID, sizeof(uint));
                break;
            }
            ImGui::EndDragDropSource();
        }
        

        // Right click
        if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
        {
            if (ImGui::Button("Delete##File"))
            {
                _deleteFile = &_fileTree->_currentDir->files[i];
                _showDeleteMessage = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        // Shwo file name when mouse is hovered
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(_fileTree->_currentDir->files[i].name.c_str());

        // Show file name
        ImGui::TextWrapped(_fileTree->_currentDir->files[i].name.c_str());

        // Draw Mesh files
        if (_fileTree->_currentDir->files[i].metaFile.type == ResourceType::MODEL && _fileTree->_currentDir->files[i].pressed)
        {
            ResourceModel* model = (ResourceModel*)ModuleResourceManager::resources[_fileTree->_currentDir->files[i].metaFile.UID];
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3, 0.3, 0.3, 1));
            for (int j = 0; j < model->modelMeshes.size(); j++)
            {
                ImGui::NextColumn();
                ImGui::ImageButton(std::to_string(model->modelMeshes[j]->UID).c_str(), (ImTextureID)_meshImageID, ImVec2(_itemWidth, _itemHeight));

                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    // Find resource path
                    _dragUID = model->modelMeshes[j]->UID;

                    // Set payload to carry the index of our item (could be anything)
                    ImGui::SetDragDropPayload("Mesh", &_dragUID, sizeof(uint));

                    ImGui::EndDragDropSource();
                }
                ImGui::TextWrapped(model->modelMeshes[j]->debugName.c_str());
            }
            ImGui::PopStyleColor(1);
        }

        ImGui::NextColumn();
    }
    ImGui::PopStyleColor(1);

    if (ImGui::BeginPopupContextWindow("WinodwProjectPopUp", ImGuiPopupFlags_NoOpenOverExistingPopup | ImGuiPopupFlags_MouseButtonDefault_))
    {
        if (ImGui::Selectable("Show in Explorer"))
            ModuleFiles::S_OpenFolder(_fileTree->_currentDir->path);

        if (ImGui::Selectable("Create Folder"))
            _openCreateFolderPanel = true;

        if (ImGui::Selectable("Create Script"))
            _openCreateScriptPanel = true;

        ImGui::EndPopup();
    }
}

void ImWindowProject::OnDrop(const std::string filePath)
{
    std::string pathNormalized = ModuleFiles::S_NormalizePath(filePath);

    // Use this for global path files
    ModuleFiles::S_ExternalCopy(pathNormalized, _fileTree->_currentDir->path);

    std::string relativePath = _fileTree->_currentDir->path + ModuleFiles::S_GetFileName(pathNormalized);

    // File case
    File file = File(relativePath, ModuleFiles::S_GetFileName(pathNormalized), _fileTree->_currentDir);//_fileTree->_currentDir->files.emplace_back(relativePath, ModuleFiles::S_GetFileName(pathNormalized), _fileTree->_currentDir);
    
    _fileTree->_currentDir->files.push_back(file);
}

void ImWindowProject::UpdateFileNodes()
{
    ModuleResourceManager::S_UpdateFileTree();

    if (ModuleResourceManager::S_GetFileTree(_fileTree))
        _fileTree->GetRootDir(_rootNode);
}

void ImWindowProject::CheckWindowFocus()
{
    Uint32 flags = SDL_GetWindowFlags(_window);

    // When Global Windows has selected -> just the instante
    if ((flags & SDL_WINDOW_INPUT_FOCUS) != 0)
    {
        if (!_isWindowFocus)
        {
            _isWindowFocus = true;

            UpdateFileNodes();
        }
    }
    // When Global Windows has deselected -> just the instante
    else if (_isWindowFocus)
        _isWindowFocus = false;
}

void ImWindowProject::PanelCreateFolder()
{
    ImGui::OpenPopup("Insert Name##Folder");
    if (ImGui::BeginPopupModal("Insert Name##Folder", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Name: "); ImGui::SameLine();
        ImGui::InputText("##inputTextFolderName", &_temporalName);

        if (ImGui::Button("Accept"))
        {
            if(ModuleFiles::S_MakeDir(_fileTree->_currentDir->path + _temporalName))
            {
                _fileTree->_currentDir->directories.push_back(
                    new Directory(
                        _fileTree->_currentDir->path + _temporalName + "/",
                        _temporalName, 
                        _fileTree->_currentDir)
                );
            }

            _temporalName = "default";
            
            _openCreateFolderPanel = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            _temporalName = "default";

            _openCreateFolderPanel = false;
        }

        ImGui::EndPopup();
    }
}

void ImWindowProject::PanelCreateScript()
{
    ImGui::OpenPopup("Insert Name##Script");
    if (ImGui::BeginPopupModal("Insert Name##Script", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("Name: "); ImGui::SameLine();
        ImGui::InputText("##inputTextScriptName", &_temporalName);

        if (ImGui::Button("Accept"))
        {
            if (ModuleFiles::S_CreateScriptFile(_temporalName, _fileTree->_currentDir->path.c_str()))
            {
                _fileTree->_currentDir->files.emplace_back(_fileTree->_currentDir->path + _temporalName + ".h", _temporalName + ".h", _fileTree->_currentDir);
                _fileTree->_currentDir->files.emplace_back(_fileTree->_currentDir->path+ _temporalName + ".cpp", _temporalName+".cpp", _fileTree->_currentDir);
            }
            else
                LayerEditor::S_AddPopUpMessage("The script with this name is already exist");

            _temporalName = "default";

            _openCreateScriptPanel = false;
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            _temporalName = "default";

            _openCreateScriptPanel = false;
        }
           

        ImGui::EndPopup();
    }
}

void ImWindowProject::DrawDeleteMessage()
{
    ImGui::OpenPopup("Delete file");
    if (ImGui::BeginPopupModal("Delete file", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
    {
        ImGui::Text("WARNING: This action cannot be undone!");
        ImGui::Text("Are you sure you want to delete this file? Any reference to it on the current scene will be destroyed.");

        if (ImGui::Button("Close"))
        {
            _deleteFile = nullptr;
            _showDeleteMessage = false;
        }
        ImGui::SameLine();
        
        if (ImGui::Button("Delete"))
        {
            _deleteFileAccepted = true;
            _showDeleteMessage = false;
        }
        ImGui::EndPopup();
    }
}
