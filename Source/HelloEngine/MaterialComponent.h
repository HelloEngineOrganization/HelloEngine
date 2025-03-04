#pragma once
#include "Component.h"
#include "Texture.h"
#include "Mesh.h"
#include "MeshRenderComponent.h"
#include "ModuleResourceManager.h"

class MaterialComponent : public Component
{
public:

	MaterialComponent(GameObject* go);
	~MaterialComponent();


	void OnEnable() override;
	void OnDisable() override;

	void SetMeshRenderer(MeshRenderComponent* mesh);

	Mesh& GetMesh();

	void ChangeTexture(ResourceTexture* resource);
	void ChangeTexture(int ID);

#ifdef STANDALONE
	void OnEditor() override;

	void MarkAsDead() override;
	void MarkAsAlive() override;

#endif // STANDALONE

	void SetAsUI();

	void Serialization(json& j) override;
	void DeSerialization(json& j) override;

	// To use when creating a new mesh inside an alredy created mesh render component.
	void UpdateMaterial();

	uint GetResourceUID();
	void DestroyedResource();
private:
	MeshRenderComponent* meshRenderer = nullptr;
	ResourceTexture* currentResource = nullptr;

	int textureID = -1;

	uint resourceUID = 0; // To be used when using MarkAsAlive only

	bool isUI = false;

};

