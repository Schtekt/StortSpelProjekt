#ifndef LIGHT_H
#define LIGHT_H
#include "Core.h"
#include "structs.h"

class BaseCamera;
enum FLAG_LIGHT
{
	// Set flag to make the light position inherit the position of the corresponding m_pMesh
	USE_TRANSFORM_POSITION = BIT(1),

	// Option to make the light cast shadows or not with different resolutions
	CAST_SHADOW = BIT(2),

	// 1. If this is set, lightData is only copied once to VRAM (onInitScene)
	// 2. Lights are interpreted as "DYNAMIC" as default
	STATIC = BIT(3),

	FLICKER = BIT(4),

	// etc..
	NUM_FLAGS_LIGHT = 4
};

static unsigned int s_LightIdCounter = 0;

class Light
{
public:
	Light(CAMERA_TYPE camType, unsigned int lightFlags = 0);
	virtual ~Light();

	bool operator== (const Light& other);

	virtual void Update(double dt) = 0;

	void SetColor(float3 color);
	void SetIntensity(float intensity);

	void SetFlickerRate(float rate);
	void SetFlickerAmplitude(float amplitude);

	// Gets
	unsigned int GetLightFlags() const;
	virtual void* GetLightData() const = 0;
	BaseCamera* GetCamera() const;

protected:
	BaseLight* m_pBaseLight = nullptr;
	unsigned int m_LightFlags = 0;
	unsigned int m_Id = 0;

	BaseCamera* m_pCamera = nullptr;
	CAMERA_TYPE m_CameraType;

	// Orthographic
	void CreateOrthographicCamera(
		float3 position, float3 direction,
		float left = -30.0f,
		float right = 30.0f,
		float bot = -30.0f,
		float top = 30.0f,
		float nearZ = 0.01f,
		float farZ = 1000.0f);

	// Perspective
	void CreatePerspectiveCamera(
		float3 position, float3 direction,
		float fov = 90.0f,
		float aspectRatio = 1.0f,
		float nearZ = 0.1f,
		float farZ = 1000.0f);

	virtual void UpdateLightColorIntensity() = 0;

	// Amplitude used in flickerIntensityFunction
	float m_FlickerAmplitude = 0.12;
	// used to scale up dt in update
	float m_FlickerRate = 0.2;
	float m_FlickerTimer = 0.0;
	float m_UnflickeredIntensity = 1.0;


	// f(time) function that calculates an intensity for a light 
	inline float flickerIntensityFunction(float x) { return 1 + m_FlickerAmplitude * (sinf(x) + sinf(2*x) + sinf(4*x) + sinf(8*x)); }
	
	void flicker(double dt);

};

#endif