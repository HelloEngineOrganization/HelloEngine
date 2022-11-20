#include "Headers.h"
#include "ModelRenderManager.h"
#include "ModuleLayers.h"
#include "MeshRenderComponent.h"
#include "IL/il.h"
#include "IL/ilut.h"
#include "MeshImporter.h"
#include "CameraObject.h"
#include "ModuleCamera3D.h"

ModelRenderManager::ModelRenderManager()
{
}

ModelRenderManager::~ModelRenderManager()
{
	RELEASE(_textureManager);
}

void ModelRenderManager::Init()
{
	_textureManager = new TextureManager();
}

RenderManager* ModelRenderManager::GetRenderManager(uint ID)
{
	return &_renderMap[ID];
}

void ModelRenderManager::Draw()
{
	// Draw opaque meshes instanced.
	for (auto& obj : _renderMap)
	{
		obj.second.Draw();
	}

	CameraObject* currentCamera = Application::Instance()->camera->currentDrawingCamera;

	// Draw transparent objects with a draw call per mesh.
	for (auto& mesh : _transparencyMeshes)
	{
		_orderedMeshes.emplace(std::make_pair<float, Mesh*>(mesh.second.modelMatrix.TranslatePart().DistanceSq(currentCamera->cameraFrustum.pos), &mesh.second)); 
	}

	// Does this iterate the map in order?
	for (auto& mesh : _orderedMeshes)
	{
		// Do camera culling checks first
		if (currentCamera->isCullingActive)
		{
			if (!currentCamera->IsInsideFrustum(mesh.second->globalAABB))
			{
				mesh.second->outOfFrustum = true;
				continue;
			}
			else
				mesh.second->outOfFrustum = false;
		}
		else if (currentCamera->type != CameraType::SCENE)
		{
			mesh.second->outOfFrustum = false;
		}

		// Update mesh. If the mesh should draw this frame, call Draw.
		if (mesh.second->Update())
		{
			mesh.second->Draw();
		}
	}
}

uint ModelRenderManager::AddTransparentMesh(RenderManager* previousRenderer, MeshRenderComponent* component)
{
	uint randomID = HelloUUID::GenerateUUID();

	_transparencyMeshes[randomID].InitAsMesh(previousRenderer->totalVertices, previousRenderer->totalIndices);
	_transparencyMeshes[randomID].localAABB = previousRenderer->localAABB;
	_transparencyMeshes[randomID].isTransparent = true;
	_transparencyMeshes[randomID].CreateBufferData();

	return randomID;
}

void ModelRenderManager::CreatePrimitive(GameObject* parent, PrimitiveType type)
{
	if (parent == nullptr) parent = Application::Instance()->layers->rootGameObject;

	switch (type)
	{
		case PrimitiveType::CUBE:
		{
			GameObject* cube = MeshImporter::LoadMesh("Assets/Primitives/cube.fbx");
			cube->SetParent(parent);
			cube->name = "Cube";
			break;
		}
		case PrimitiveType::SPHERE:
		{
			GameObject* sphere = MeshImporter::LoadMesh("Assets/Primitives/sphere.fbx");
			sphere->SetParent(parent);
			sphere->name = "Sphere";
			break;
		}
		case PrimitiveType::CYLINDER:
		{
			GameObject* cylinder = MeshImporter::LoadMesh("Assets/Primitives/cylinder.fbx");
			cylinder->SetParent(parent);
			cylinder->name = "Cylinder";
			break;
		}
		case PrimitiveType::PLANE:
		{
			GameObject* plane = MeshImporter::LoadMesh("Assets/Primitives/plane.fbx");
			plane->SetParent(parent);
			plane->name = "Plane";
			break;
		}
	}
}
