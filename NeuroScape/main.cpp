#ifdef NDEBUG
# define WIN32_LEAN_AND_MEAN
# define WIN32_EXTRA_LEAN
# include <Windows.h>
#endif /* NDEBUG */

#define GLEW_STATIC
#include <GL/glew.h>
#pragma comment(lib, "OpenGl32.lib")
#pragma comment(lib, "glew32s.lib")

#include <GLFW/glfw3.h>
#ifndef NDEBUG
# pragma comment(lib, "glfw3-s.lib")
#else /* !NDEBUG */
# pragma comment(lib, "glfw3-s-d.lib")
#endif /* !NDEBUG */

#include <stdexcept>

//Data vertices et indices
float vertices[15] = {
	 0.0f,  0.5f, 1.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	 0.5f, -0.5f, 0.0f, 0.0f, 1.0f
};
unsigned int indices[3] = { 0, 1, 2 };

//Shaders sources
const char* vsSrc = R"(#version 460 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;
out vec3 vColor;
void main()
{
	gl_Position = vec4(aPos, 0.0, 1.0);
	vColor = aColor;
})";
const char* fsSrc = R"(#version 460 core
in vec3 vColor;
layout(location = 0) out vec4 oFragColor;
void main()
{
	oFragColor = vec4(vColor, 1.0);
})";

int main(int argc, char** argv)
{
	glfwInit();

	// Set context as OpenGL 4.6 Core, forward compat, with debug depending on build config
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#ifndef NDEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#else /* !NDEBUG */
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
#endif /* !NDEBUG */
	glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	GLFWwindow* window = glfwCreateWindow(800, 480, "NeuroScape Demo", nullptr, nullptr);
	if (!window)
		throw std::runtime_error("Unable to initialize GLFW");

	glfwMakeContextCurrent(window);

	glewExperimental = true;
	glewInit();

	//memory allocation in VRAM + adding data
	unsigned int buffers[2];
	glCreateBuffers(2, buffers);
	glNamedBufferStorage(buffers[0], 15 * sizeof(float), vertices, 0);
	glNamedBufferStorage(buffers[1], 3 * sizeof(unsigned int), indices, 0);

	//Geometry
	unsigned int vao;
	glCreateVertexArrays(1, &vao);
	glVertexArrayVertexBuffer(vao, 0, buffers[0], 0, 5 * sizeof(float));
	glEnableVertexArrayAttrib(vao, 0);
	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribFormat(vao, 0, 2, GL_FLOAT, GL_FALSE, 0);
	glEnableVertexArrayAttrib(vao, 1);
	glVertexArrayAttribBinding(vao, 1, 0);
	glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(float));
	glVertexArrayElementBuffer(vao, buffers[1]);

	//Create Vertex shader from src and compile it
	unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vsSrc, nullptr);
	glCompileShader(vs);

	//Create Fragment shader from src and compile it
	unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fsSrc, nullptr);
	glCompileShader(fs);

	//Create program, attach it shaders and link it
	unsigned int sp = glCreateProgram();
	glAttachShader(sp, vs);
	glAttachShader(sp, fs);
	glLinkProgram(sp);

	do
	{
		glfwPollEvents();

		glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//Bind vao
		glBindVertexArray(vao);
		//Use program (with vao)
		glUseProgram(sp);
		//Draw call
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
		//Remove the Program from using
		glUseProgram(0);
		//Unbind the vao
		glBindVertexArray(0);

		//swap frame buffers
		glfwSwapBuffers(window);
	} while (!glfwWindowShouldClose(window));

	//Delete
	glDeleteProgram(sp);
	glDeleteShader(fs);
	glDeleteShader(vs);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(2, buffers);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}

#ifdef NDEBUG
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	return main(0, nullptr);
}
#endif /* NDEBUG */