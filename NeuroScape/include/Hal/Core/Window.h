#pragma once
#include <GLFW/glfw3.h>
namespace ns::hal::core 
{
	class Window final
	{
		Window() = delete;
		Window(int toto);
		GLFWwindow* window;
	};
}

