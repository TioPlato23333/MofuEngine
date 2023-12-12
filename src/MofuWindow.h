#ifndef SRC_MOFUWINDOW_H_
#define SRC_MOFUWINDOW_H_

#include <glm/glm.hpp>

#include "Camera.h"

class GLFWwindow;

class MofuWindow {
public:
	MofuWindow() = default;
	virtual ~MofuWindow() = default;

	void ShowWindow();

private:
	void ProcessInput(GLFWwindow* window);

	float delta_time_ = 0.0f;
	float last_time_ = 0.0f;

	static bool mouse_pressed_;
	static float last_x_;
	static float last_y_;
	static Camera camera_;
	static constexpr int SCR_WIDTH = 800;
	static constexpr int SCR_HEIGHT = 600;
	static constexpr char WINDOW_NAME[] = "MofuEngine";
};

#endif  // SRC_MOFUWINDOW_H_
