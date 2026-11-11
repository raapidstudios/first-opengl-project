#include "camera.h"

// State variables for smooth camera orbit / movement damping
static glm::vec3 currentVelocity(0.0f);
static glm::vec2 currentOrbitVelocity(0.0f);
static double lastTime = 0.0;

// Spherical coordinates for orbiting around the shape center (0, 0, 0)
static float radius = 2.0f;
static float yawAngle = -90.0f; // in degrees
static float pitchAngle = 0.0f; // in degrees
static bool initializedOrbit = false;

Camera::Camera(int width, int height, glm::vec3 position)
{
	Camera::width = width;
	Camera::height = height;
	Position = position;
}

Camera::Camera(glm::vec3 position)
{
	Camera::width = 800;
	Camera::height = 600;
	Position = position;
}

glm::mat4 Camera::GetViewMatrix() const
{
	return glm::lookAt(Position, Position + Orientation, Up);
}

void Camera::Matrix(float FOVdeg, float nearPlane, float farPlane, Shader& shader, const char* uniform)
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	view = glm::lookAt(Position, Position + Orientation, Up);
	projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);

	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

void Camera::processInput(GLFWwindow* window)
{
	// Compute delta time for frame-rate independent smooth movement
	double currentTime = glfwGetTime();
	if (lastTime == 0.0)
	{
		lastTime = currentTime;
	}
	float deltaTime = (float)(currentTime - lastTime);
	lastTime = currentTime;

	// Clamp delta time to avoid large jumps if the window is moved or paused
	if (deltaTime > 0.1f) deltaTime = 0.1f;
	if (deltaTime < 0.0001f) deltaTime = 0.0001f;

	// Initialize spherical coordinates from initial position if not done yet
	if (!initializedOrbit)
	{
		radius = glm::length(Position);
		if (radius < 0.1f) radius = 2.0f;
		pitchAngle = glm::degrees(asin(glm::clamp(Position.y / radius, -1.0f, 1.0f)));
		yawAngle = glm::degrees(atan2(Position.z, Position.x));
		initializedOrbit = true;
	}

	// Dynamic speed adjustment for keyboard
	float baseSpeed = 2.5f;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		baseSpeed = 7.5f;
	}

	// Calculate target move direction
	glm::vec3 moveDir(0.0f);
	glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		moveDir += Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		moveDir -= Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		moveDir += right;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		moveDir -= right;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		moveDir += Up;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		moveDir -= Up;
	}

	glm::vec3 targetVelocity(0.0f);
	if (glm::length(moveDir) > 0.0001f)
	{
		targetVelocity = glm::normalize(moveDir) * baseSpeed;
	}

	// Smooth acceleration & deceleration (damping / exponential lerp)
	float moveDamping = 12.0f;
	float moveBlend = 1.0f - exp(-moveDamping * deltaTime);
	currentVelocity = glm::mix(currentVelocity, targetVelocity, moveBlend);

	Position += currentVelocity * deltaTime;

	// Update radius if position moved via keyboard
	if (glm::length(currentVelocity) > 0.01f)
	{
		radius = glm::length(Position);
		if (radius < 0.1f) radius = 0.1f;
		pitchAngle = glm::degrees(asin(glm::clamp(Position.y / radius, -1.0f, 1.0f)));
		yawAngle = glm::degrees(atan2(Position.z, Position.x));
	}

	// Orbit rotation when holding Left Mouse Button:
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2.0), (height / 2.0));
			firstClick = false;
			currentOrbitVelocity = glm::vec2(0.0f);
		}

		double mouseX;
		double mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		float deltaX = (float)(mouseX - (width / 2.0));
		float deltaY = (float)(mouseY - (height / 2.0));

		// Rotation sensitivity
		float sens = 1.2f;
		glm::vec2 targetOrbitVelocity(deltaX * sens, deltaY * sens);

		// Smooth look damping for fluid orbit response
		float orbitDamping = 20.0f;
		float orbitBlend = 1.0f - exp(-orbitDamping * deltaTime);
		currentOrbitVelocity = glm::mix(currentOrbitVelocity, targetOrbitVelocity, orbitBlend);

		yawAngle += currentOrbitVelocity.x * deltaTime * 50.0f;
		pitchAngle += currentOrbitVelocity.y * deltaTime * 50.0f;

		// Constrain pitch to avoid flipping over the poles
		if (pitchAngle > 89.0f) pitchAngle = 89.0f;
		if (pitchAngle < -89.0f) pitchAngle = -89.0f;

		// Recalculate camera position around target center (0, 0.4, 0)
		glm::vec3 targetCenter(0.0f, 0.4f, 0.0f);
		float radYaw = glm::radians(yawAngle);
		float radPitch = glm::radians(pitchAngle);

		Position.x = targetCenter.x + radius * cos(radPitch) * cos(radYaw);
		Position.y = targetCenter.y + radius * sin(radPitch);
		Position.z = targetCenter.z + radius * cos(radPitch) * sin(radYaw);

		Orientation = glm::normalize(targetCenter - Position);

		glfwSetCursorPos(window, (width / 2.0), (height / 2.0));
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		firstClick = true;
		currentOrbitVelocity = glm::vec2(0.0f);
	}
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
	float velocity = 2.5f * deltaTime;
	glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));
	if (direction == FORWARD)
		Position += Orientation * velocity;
	if (direction == BACKWARD)
		Position -= Orientation * velocity;
	if (direction == LEFT)
		Position -= right * velocity;
	if (direction == RIGHT)
		Position += right * velocity;
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
	float sens = 0.1f;
	xoffset *= sens;
	yoffset *= sens;

	Orientation = glm::rotate(Orientation, glm::radians(-xoffset), Up);
	glm::vec3 right = glm::normalize(glm::cross(Orientation, Up));
	glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(yoffset), right);

	if (!constrainPitch || abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
	{
		Orientation = newOrientation;
	}
}

void Camera::ProcessMouseScroll(float yoffset)
{
	Zoom -= (float)yoffset;
	if (Zoom < 1.0f)
		Zoom = 1.0f;
	if (Zoom > 45.0f)
		Zoom = 45.0f;
}
