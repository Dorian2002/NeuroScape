#pragma once
#include <GLFW/glfw3.h>
namespace ns::hal::core 
{
	class Window final
	{
		void Init();
		GLFWwindow* window;

	public :
		explicit Window(int, int, const char*, GLFWmonitor*, GLFWwindow*);
		~Window();
		GLFWwindow* GetWindow();
	};
}

