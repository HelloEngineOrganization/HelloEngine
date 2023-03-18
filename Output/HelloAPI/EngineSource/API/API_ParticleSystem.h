#pragma once
#include "Globals.h"
#include "API_GameObject.h"

class ParticleSystemComponent;

namespace API
{
	class TO_API API_ParticleSystem
	{
	public:
		API_ParticleSystem();
		~API_ParticleSystem();

		API_GameObject GetGameObject();

		void Play();
		void Pause();
		void Stop();

	private:
		ParticleSystemComponent* GetComponent();
		void SetComponent(ParticleSystemComponent* component);
	private:
		ParticleSystemComponent* _particleSystem = nullptr;

		friend class API_GameObject;
		friend class DragBoxParticleSystem;
	};
}
