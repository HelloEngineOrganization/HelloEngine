#pragma once
#include "Component.h"
#include "TransformComponent.h"

#define COMPONENT_NUM 5

class GameObject
{
public:
	GameObject(GameObject* parent, std::string name = "Default", std::string tag = "Default", uint ID = 0);
	GameObject(GameObject* parent, std::string& name, std::string& tag, uint ID = 0);
	~GameObject();

	template <class T>
	T* AddComponent()
	{
		T* newComponent = new T(this);
		_components.push_back(newComponent);

		return newComponent;
	}

	template <class T>
	T* AddComponentCopied(const T& copy)
	{
		T* newComponent = new T(this, copy);
		_components.push_back(newComponent);

		return newComponent;
	}

	Component* AddComponentOfType(Component::Type type);
	Component* AddComponentOfType(Component::Type type, const Component& copy);
	void AddComponentSerialized(Component::Type type, json& jsonFile);

	template<class T>
	T* GetComponent(uint index = 0)
	{
		uint counter = 0;
		for (const auto& component : _components)
		{
			if (component == nullptr)
				continue;

			if (typeid(*component) != typeid(T)) continue;
	
			if (counter == index) return (T*)component;
			else counter++;
		}
		return nullptr;
	}

	std::vector<Component*> GetComponents() { return _components;}

	void DestroyComponent(Component::Type type);
	void DestroyComponent(Component* component);

	GameObject* GetParent() { return _parent; }

	bool AddChild(GameObject* child);

	// Only to be called when a GameObject is destroyed! no Game Object should be without a parent.
	void RemoveChild(GameObject* child);

	bool SetParent(GameObject* parent);

	void SetActive(bool active);

	bool IsActive() { return _isActive; }

	bool IsPendingToDelete() { return _isPendingToDelete; }

	bool IsStatic() { return _isStatic; }

	std::vector<GameObject*>* GetChildren() { return &_children; }

	uint GetID() { return _ID; }



#ifdef STANDALONE
	void OnEditor();
	bool MarkAsDead();

	bool MarkAsAlive();
#endif // STANDALONE

	void Destroy();

	template <class T>
	bool HasComponent()
	{
		for (const auto& component : _components)
		{
			if (component == nullptr)
				continue;

			if (typeid(*component) == typeid(T)) 
				return true;
		}
		return false;
	}

	bool HasComponent(Component::Type type);

public:
	std::string name = "";
	std::string tag = "";
	TransformComponent* transform = nullptr;

	bool isSelected = false;

private:
	std::vector<Component*> _components;
	std::vector<GameObject*> _children;

	GameObject* _parent = nullptr;

	bool _isActive = true;
	bool _isDestroyed = false;
	bool _isStatic = false;

	uint _ID = 0; // ID =  0 is an invalid ID. First ID is 1.

	// On Editor variables
	std::string _comboValues[COMPONENT_NUM] = { "Mesh Renderer", "Material", "Camera", "Script", "UITest"};
	bool _isPendingToDelete = false;
#ifdef STANDALONE
	std::vector<int> _childrenDeletedIndex;
#endif // STANDALONE
	friend class TransformComponent;
	friend class ModuleRenderer3D;
	friend class ModuleResourceManager;
	friend class CommandSetParentGameObject;
	friend class ImWindowHierarchy;
	friend class ImWindowInspector;

};