#include "Headers.h"
#include "RenderManager.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"

RenderManager::RenderManager()
{
    basicShader = new Shader("../Source/shaders/basic.vertex.shader", "../Source/shaders/basic.fragment.shader");
}

RenderManager::~RenderManager()
{
    RELEASE(basicShader);
}

uint RenderManager::SetMeshInformation(Mesh& mesh)
{
    if (initialized) LOG("Tried to call RenderManager::SetMeshInformation more than once in a single Render Manager instnace.");
    // Set this RenderManager Mesh information.
    this->totalVertices.insert(totalVertices.begin(), mesh._vertices->begin(), mesh._vertices->end());
    this->totalIndices.insert(totalIndices.begin(), mesh._indices->begin(), mesh._indices->end());

    CreateBuffers();
    
    Mesh firstMesh;
    firstMesh.InitAsMeshInformation(mesh.position, mesh.scale);
    
    mesh.CleanUp(); // Destroy the original vertex and index data (now it is stored inside this render manager).
    initialized = true;

    return AddMesh(firstMesh); // Adds a copy of the original mesh into the mesh list.
}

void RenderManager::Draw()
{
    if (!initialized) return; // This is placed here for security reasons. No RenderManager should be created without being initialized.
    if (meshes.empty())
    {
        LOG("A Render Manager is being updated without any meshes!");
        return;
    }
    for (auto mesh : meshes)
    {
        mesh.second.Update();
        modelMatrices.push_back(mesh.second.modelMatrix); // Insert updated matrices
        textureIDs.push_back(mesh.second.OpenGLTextureID);
    }

    // Draw using Dynamic Geometry
    glBindVertexArray(VAO);

    // Update Model matrices
    glBindBuffer(GL_ARRAY_BUFFER, MBO);
    void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr, &modelMatrices.front(), modelMatrices.size() * sizeof(float4x4));
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // Update TextureIDs
    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    void* ptr2 = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr2, &textureIDs.front(), textureIDs.size() * sizeof(float));
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // Update View and Projection matrices
    basicShader->Bind();
    basicShader->SetMatFloat4v("view", Application::Instance()->camera->GetViewMatrix());
    basicShader->SetMatFloat4v("projection", Application::Instance()->renderer3D->GetProjectionMatrix());

    // Draw
    glDrawElementsInstanced(GL_TRIANGLES, totalIndices.size(), GL_UNSIGNED_INT, 0, modelMatrices.size());
    
    glBindVertexArray(0);
    
    // Reset model matrices.
    modelMatrices.clear();
    textureIDs.clear();
}

uint RenderManager::AddMesh(Mesh& mesh)
{
    if (!initialized)
    {
        LOG("Trying to add mesh information into a RenderManager that has not been initialized yet!");
    }
    uint meshID = ++IDcounter;
    meshes[meshID] = mesh;
    return meshID;
}

void RenderManager::CreateBuffers()
{
    // Create Vertex Array Object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create Vertex Buffer Object
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * totalVertices.size(), &totalVertices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normals));
    // vertex texture coords
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    // Create Index Buffer Object
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * totalIndices.size(), &totalIndices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    // Create Model Matrix buffer object
    glGenBuffers(1, &MBO);

    glBindBuffer(GL_ARRAY_BUFFER, MBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float4x4) * 1000000, nullptr, GL_DYNAMIC_DRAW);

    glBindVertexArray(VAO);

    // You can't pass an entire matrix, so we go row by row.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(float4x4), (void*)0);

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(float4x4), (void*)sizeof(float4));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(float4x4), (void*)(sizeof(float4) * 2));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(float4x4), (void*)(sizeof(float4) * 3));

    // Set instancing interval
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);

    // Create TextureID buffer object
    glGenBuffers(1, &TBO);

    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 100, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);

    glVertexAttribDivisor(7, 1);

    glBindVertexArray(0);
}
