#include "Headers.h"
#include "RenderManager.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"

RenderManager::RenderManager()
{
    basicShader = new Shader("Resources/shaders/basic.vertex.shader", "Resources/shaders/basic.fragment.shader");
    lineShader = new Shader("Resources/shaders/lines.vertex.shader", "Resources/shaders/lines.fragment.shader");
}

RenderManager::~RenderManager()
{
    RELEASE(basicShader);
    RELEASE(lineShader);
}

uint RenderManager::SetMeshInformation(Mesh& mesh)
{
    if (initialized) LOG("Tried to call RenderManager::SetMeshInformation more than once in a single Render Manager instance.");
    // Set this RenderManager Mesh information.
    this->totalVertices.insert(totalVertices.begin(), mesh._vertices->begin(), mesh._vertices->end());
    this->totalIndices.insert(totalIndices.begin(), mesh._indices->begin(), mesh._indices->end());

    CreateBuffers();
    CreateNormalsDisplayBuffer();

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
    for (auto& mesh : meshes)
    {
        if (!mesh.second.Update())
            continue;

        modelMatrices.push_back(mesh.second.modelMatrix); // Insert updated matrices
        textureIDs.push_back(mesh.second.OpenGLTextureID);
    }

    if (modelMatrices.empty()) return;

    // Update View and Projection matrices
    basicShader->Bind();
    basicShader->SetMatFloat4v("view", Application::Instance()->camera->currentDrawingCamera->GetViewMatrix());
    basicShader->SetMatFloat4v("projection", Application::Instance()->camera->currentDrawingCamera->GetProjectionMatrix());

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

    for (int i = 0; i < TextureManager::bindedTextures; i++)
    {
        basicShader->SetInt(("textures[" + std::to_string(i) + "]").c_str(), i);
    }

    // Draw
    glDrawElementsInstanced(GL_TRIANGLES, totalIndices.size(), GL_UNSIGNED_INT, 0, modelMatrices.size());
    glBindVertexArray(0);

    // Drawing normals for every mesh instance
    
    if (Application::Instance()->camera->currentDrawingCamera->type == CameraType::SCENE)
    {
        int index = 0;
        for (auto& mesh : meshes)
        {
            if (mesh.second.showNormals == 0) DrawVertexNormals(index);
            else if (mesh.second.showNormals == 1) DrawFaceNormals(index);
            index++;
        }
    }

    // Reset model matrices.
    modelMatrices.clear();
    textureIDs.clear();
    TextureManager::UnBindTextures();
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(float4x4) * 1000000, nullptr, GL_DYNAMIC_DRAW); // TODO: This buffer size should dynamicaly change

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

    glBindVertexArray(0);

    // Create TextureID buffer object
    glGenBuffers(1, &TBO);

    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 10000, nullptr, GL_DYNAMIC_DRAW); // TODO: This buffer size should dynamically change
    
    glBindVertexArray(VAO);

    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);

    glVertexAttribDivisor(7, 1);

    glBindVertexArray(0);
}

void RenderManager::CreateNormalsDisplayBuffer()
{
    { 
        vertexNormalsDisplay.resize(totalVertices.size() * 2);

        float lineMangitude = 0.5f;

        int j = 0;
        for (int i = 0; i < totalVertices.size() * 2; i++)
        {
            if (i % 2 == 0)
            {
                vertexNormalsDisplay[i] = totalVertices[j].position;
            }
            else
            {
                vertexNormalsDisplay[i] = totalVertices[j].position + (totalVertices[j].normals * lineMangitude);
                j++;
            }

        }

        glGenVertexArrays(1, &VertexLineVAO);
        glBindVertexArray(VertexLineVAO);

        // Create Vertex Buffer Object
        glGenBuffers(1, &VertexLineVBO);

        glBindBuffer(GL_ARRAY_BUFFER, VertexLineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * vertexNormalsDisplay.size(), &vertexNormalsDisplay[0], GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float3), (void*)0);

        glBindVertexArray(0);
    }

    {
        faceNormalsDisplay.resize((totalIndices.size() / 3) * 2); // 3 vertices make a face; we need 2 points to display 1 face normal. 

        float lineMangitude = 0.5f;

        int k = 0;
        int l = 0;

        int iterations = faceNormalsDisplay.size() / 2;
        for (int i = 0; i < iterations; i++)
        {
            float3 faceCenter = { 0,0,0 };
            for (int j = 0; j < 3; j++)
            {
                faceCenter += totalVertices[totalIndices[k++]].position;
            }
            faceCenter /= 3;
            faceNormalsDisplay.push_back(faceCenter);
            
            float3 normalsDir = { 0,0,0 };
            for (int j = 0; j < 3; j++)
            {
                normalsDir += totalVertices[totalIndices[l++]].normals;
            }
            normalsDir /= 3;
            normalsDir.Normalize();
            faceNormalsDisplay.push_back(faceCenter + (normalsDir * lineMangitude));
        }

        glGenVertexArrays(1, &FaceLineVAO);
        glBindVertexArray(FaceLineVAO);

        // Create Vertex Buffer Object
        glGenBuffers(1, &FaceLineVBO);

        glBindBuffer(GL_ARRAY_BUFFER, FaceLineVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * faceNormalsDisplay.size(), &faceNormalsDisplay[0], GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float3), (void*)0);

        glBindVertexArray(0);
    }
}

void RenderManager::DrawVertexNormals(int modelMatrixIndex)
{
    lineShader->Bind();
    lineShader->SetMatFloat4v("view", Application::Instance()->camera->currentDrawingCamera->GetViewMatrix());
    lineShader->SetMatFloat4v("projection", Application::Instance()->camera->currentDrawingCamera->GetProjectionMatrix());
    lineShader->SetFloat4("lineColor", 0.36f, 0.75f, 0.72f, 1.0f);

    lineShader->SetMatFloat4v("model", &modelMatrices[modelMatrixIndex].v[0][0]);

    glBindVertexArray(VertexLineVAO);

    glDrawArrays(GL_LINES, 0, vertexNormalsDisplay.size());

    glBindVertexArray(0);

}

void RenderManager::DrawFaceNormals(int modelMatrixIndex)
{
    lineShader->Bind();
    lineShader->SetMatFloat4v("view", Application::Instance()->camera->currentDrawingCamera->GetViewMatrix());
    lineShader->SetMatFloat4v("projection", Application::Instance()->camera->currentDrawingCamera->GetProjectionMatrix());
    lineShader->SetFloat4("lineColor", 0.75f, 0.36f, 0.32f, 1.0f);

    lineShader->SetMatFloat4v("model", &modelMatrices[modelMatrixIndex].v[0][0]);

    glBindVertexArray(FaceLineVAO);

    glDrawArrays(GL_LINES, 0, faceNormalsDisplay.size());

    glBindVertexArray(0);

}

