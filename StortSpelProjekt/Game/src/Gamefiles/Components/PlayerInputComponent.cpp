#include "PlayerInputComponent.h"
#include "../Events/EventBus.h"
#include "../ECS/Entity.h"
#include "../Renderer/PerspectiveCamera.h"
#include "../Renderer/Transform.h"
#include "../ECS/Components/Collision/CollisionComponent.h"

component::PlayerInputComponent::PlayerInputComponent(Entity* parent, unsigned int camFlags)
	:InputComponent(parent)
{
	EventBus::GetInstance().Subscribe(this, &PlayerInputComponent::toggleCameraLock);
	EventBus::GetInstance().Subscribe(this, &PlayerInputComponent::zoom);
	EventBus::GetInstance().Subscribe(this, &PlayerInputComponent::rotate);
	EventBus::GetInstance().Subscribe(this, &PlayerInputComponent::move);
	EventBus::GetInstance().Subscribe(this, &PlayerInputComponent::mouseClick);
	EventBus::GetInstance().Subscribe(this, &PlayerInputComponent::grunt);

	m_CameraFlags = camFlags;

	m_Zoom = 3.0f;

	m_Pitch = 1.0f;

	m_Yaw = 1.0f;

	m_CameraDistance = sqrt(m_Zoom * 4 * m_Zoom * 4 + m_Zoom * m_Zoom);

	m_pCamera = nullptr;
	m_pTransform = nullptr;
}

component::PlayerInputComponent::~PlayerInputComponent()
{
}

void component::PlayerInputComponent::Init()
{
	m_pCamera = static_cast<PerspectiveCamera*>(m_pParent->GetComponent<component::CameraComponent>()->GetCamera());
	m_pTransform = static_cast<Transform*>(m_pParent->GetComponent<component::TransformComponent>()->GetTransform());

	m_pCC = m_pParent->GetComponent<component::CollisionComponent>();
}

void component::PlayerInputComponent::RenderUpdate(double dt)
{
	// Lock camera to player
	if (m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION)
	{
		float3 playerPosition = m_pTransform->GetRenderPositionFloat3();
		float3 forward = m_pTransform->GetForwardFloat3();

		float zoomBack = sqrt(m_CameraDistance * m_CameraDistance - (m_Zoom * m_Pitch) * (m_Zoom * m_Pitch));

		float3 cameraPosition = { playerPosition.x - (zoomBack * forward.x), playerPosition.y + m_Zoom * m_Pitch, playerPosition.z - (zoomBack * forward.z) };

		m_pCamera->SetPosition(cameraPosition.x, cameraPosition.y, cameraPosition.z);
		float directionX = playerPosition.x - cameraPosition.x;
		float directionY = playerPosition.y + 3.0f - cameraPosition.y;
		float directionZ = playerPosition.z - cameraPosition.z;
		m_pCamera->SetDirection(directionX, directionY, directionZ);
	}
	else
	{
		m_pCamera->SetDirection(cos(m_Yaw), m_Pitch * -2, sin(m_Yaw));
	}
}

void component::PlayerInputComponent::toggleCameraLock(ModifierInput* evnt)
{
	if (evnt->key == SCAN_CODES::LEFT_CTRL && evnt->pressed)
	{
		// XOR operator. If m_CameraFlags contains the flag, remove it. Otherwise add it to m_CameraFlags.
		m_CameraFlags ^= CAMERA_FLAGS::USE_PLAYER_POSITION;
		m_pCamera->SetMovement(0.0f, 0.0f, 0.0f);
		m_pTransform->SetMovement(0.0f, 0.0f, 0.0f);
		m_Yaw = sqrt(2);
		m_Pitch = 0.0f;
	}
}

void component::PlayerInputComponent::zoom(MouseScroll* evnt)
{
	m_Zoom = max(m_Zoom - static_cast<float>(evnt->scroll) / 4, 1.5f);
	m_CameraDistance = sqrt(m_Zoom * 4 * m_Zoom * 4 + m_Zoom * m_Zoom);
}

void component::PlayerInputComponent::move(MovementInput* evnt)
{
	// Check if the key has just been pressed or jsut been released and convert to a float. Multiply by two and subtract one to get 1 for true and -1 for false. If
	// the key has been pressed, the player should start moving in the direction specified by the key -- hence the value 1. If the key has been released, the player's
	// movement should be negated in the direction specified by the key -- hence the value -1
	double pressed = (static_cast<double>(evnt->pressed) * 2 - 1);
	double pressedSpace = static_cast<double>(evnt->pressed);

	// Find out which key has been pressed. Convert to float to get the value 1 if the key pressed should move the player in the positive
	// direction and the value -1 if the key pressed should move the player in the negative direction
	double moveRight = (static_cast<double>(evnt->key == SCAN_CODES::D) - static_cast<double>(evnt->key == SCAN_CODES::A)) * pressed;
	double moveUp = (static_cast<double>(evnt->key == SCAN_CODES::Q) - static_cast<double>(evnt->key == SCAN_CODES::E)) * pressed;
	double moveForward = (static_cast<double>(evnt->key == SCAN_CODES::W) - static_cast<double>(evnt->key == SCAN_CODES::S)) * pressed;

	double jump = static_cast<double>(evnt->key == SCAN_CODES::SPACE) * pressedSpace;

	// Get the rotation matrix to determine in which direction to move
	float3 forward = m_pTransform->GetForwardFloat3();
	float3 right = m_pTransform->GetRightFloat3();

	double moveX = forward.x * moveForward + right.x * moveRight;
	double moveY = jump;
	double moveZ = forward.z * moveForward + right.z * moveRight;

	//(m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION) ? m_pTransform->UpdateMovement(moveX, moveY, moveZ) : m_pCamera->UpdateMovement(-moveRight, moveUp, moveForward);
	double3 vel = m_pCC->GetLinearVelocity();
	(m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION) ? m_pCC->SetVelVector(vel.x + moveX * 10, vel.y + moveY * 20, vel.z + moveZ * 10) : m_pCamera->UpdateMovement(-moveRight, moveUp, moveForward);

	// If all buttons are released, reset the movement
	if (!(Input::GetInstance().GetKeyState(SCAN_CODES::W)) &&
		!(Input::GetInstance().GetKeyState(SCAN_CODES::A)) &&
		!(Input::GetInstance().GetKeyState(SCAN_CODES::S)) &&
		!(Input::GetInstance().GetKeyState(SCAN_CODES::D)) &&
		!(Input::GetInstance().GetKeyState(SCAN_CODES::Q)) &&
		!(Input::GetInstance().GetKeyState(SCAN_CODES::E)) &&
		!(Input::GetInstance().GetKeyState(SCAN_CODES::SPACE)))
	{
		//(m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION) ? m_pTransform->SetMovement(0.0f, 0.0f, 0.0f) : m_pCamera->SetMovement(0.0f, 0.0f, 0.0f);;
		(m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION) ? m_pCC->SetVelVector(0.0f, vel.y + moveY * 20, 0.0f) : m_pCamera->SetMovement(0.0f, 0.0f, 0.0f);;
	}
}

void component::PlayerInputComponent::rotate(MouseMovement* evnt)
{
	// Rotate camera
	// Mouse movement
	int x = evnt->x, y = evnt->y;

	// Determine how much to rotate in radians
	double rotateY = (static_cast<double>(y) / 600.0) * PI;
	double rotateX = -(static_cast<double>(x) / 800.0) * PI;

	m_Pitch = max(min(m_Pitch + rotateY, 3.0f), -3.0f);
	m_Yaw = m_Yaw + rotateX;

	if (m_CameraFlags & CAMERA_FLAGS::USE_PLAYER_POSITION)
	{
		// Rotate transform
		// Determine how much to rotate in radians
		rotateX = (static_cast<double>(x)) / 400.0 * PI;

		// Get rotation to determine current rotation angle
		float3 forward = m_pTransform->GetForwardFloat3();

		float angle = std::atan2(forward.x, forward.z);

		// Set the new rotation
		m_pCC->Rotate({ 0.0, 1.0, 0.0 }, rotateX);
		m_pCC->SetAngularVelocity(0.0, 0.0, 0.0);

		// Get new direction
		forward = m_pTransform->GetForwardFloat3();
		float3 right = m_pTransform->GetRightFloat3();

		// Determine if player is currently moving, if yes, update movement direction
		int isMovingZ = static_cast<int>(Input::GetInstance().GetKeyState(SCAN_CODES::W)) - static_cast<int>(Input::GetInstance().GetKeyState(SCAN_CODES::S));
		int isMovingX = static_cast<int>(Input::GetInstance().GetKeyState(SCAN_CODES::D)) - static_cast<int>(Input::GetInstance().GetKeyState(SCAN_CODES::A));

		double3 vel = m_pCC->GetLinearVelocity();
		m_pCC->SetVelVector((forward.x * isMovingZ + right.x * isMovingX) * 10, vel.y, (forward.z * isMovingZ + right.z * isMovingX) * 10);
	}
}

void component::PlayerInputComponent::mouseClick(MouseClick* evnt)
{
	switch (evnt->button) {
	case MOUSE_BUTTON::LEFT_DOWN:
		Log::Print("Left Mouse button down \n");
		break;
	case MOUSE_BUTTON::RIGHT_DOWN:
		Log::Print("Right Mouse button down \n");
		break;
	}
}

void component::PlayerInputComponent::grunt(Collision* evnt)
{
	if (evnt->ent1 == GetParent() || evnt->ent2 == GetParent())
	{
		GetParent()->GetComponent<component::Audio2DVoiceComponent>()->Play(L"Bruh");
	}
}
