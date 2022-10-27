#pragma once

#include "FrameBuffer.h"

class CameraObject
{
public:
	CameraObject();
	~CameraObject();

	void Look(const float3& Position, const float3& Reference, bool RotateAroundReference = false);
	void LookAt(const float3& Spot);
	void Move(const float3& Movement);
	
	float* GetViewMatrix();
	float* GetProjectionMatrix();

	void RecalculateProjection();

	void RegenerateFrameBuffer(int width, int height);

	void ChangeAspectRatio(float aspectRatio);

	float GetAspectRatio() { return aspectRatio; }

	void SetFOV(float fov);
	float GetFOV() { return FOV; };
public:
	float3 X, Y, Z, Position, Reference;
	bool active = true;
	FrameBuffer frameBuffer;
private:
	void CalculateViewMatrix();

private:
	float4x4 ViewMatrix, ViewMatrixInverse, ProjectionMatrix;
	float3 offset;
	bool changeFOVWithBufferSize;
	float aspectRatio = 0.0f;
	float FOV = 60.0f;
protected:
	Application* app = nullptr;
	Frustum cameraFrustum;

	friend class ModuleCamera3D;
	friend class CameraComponent;
};

