
#include "CorePlots.h"

static void glfw_error_callback(int error, const char *description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(int, char **)
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	const char *glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only


	// Create window with graphics context
	GLFWwindow *window = glfwCreateWindow(1280, 720, "Dear ImGui - Conan", NULL, NULL);
	if (window == NULL)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	bool err = glewInit() != GLEW_OK;

	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
	}

	int screen_width, screen_height;
	glfwGetFramebufferSize(window, &screen_width, &screen_height);
	glViewport(0, 0, screen_width, screen_height);


	//Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;     // Enable Docking
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;   // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;          // Force all windows to use separate viewport
	io.ConfigWindowsMoveFromTitleBarOnly = true;          // Allows mouse functions in world visualization

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::StyleColorsDark(); // Setup Dear ImGui style


	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
		glClear(GL_COLOR_BUFFER_BIT);

		// feed inputs to dear imgui, start new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::CreateContext();
		ImPlot::CreateContext();

        bool status = true;
        bool* open = &status;

        ImPlot::ShowDemoWindow(open);


		// Render dear imgui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glfwSwapBuffers(window);
	}


	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}


unsigned int handleGLErrors()
{
	int         errorStackCount = 0;
	GLenum      err;
	std::string error;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		// log the error.
		errorStackCount++;
		switch (err)
		{
		case GL_INVALID_ENUM:
			error = "AN UNACCEPTABLE VALUE IS SPECIFIED FOR AN ENUMERATED ARGUMENT";
			break;
		case GL_INVALID_VALUE:
			error = "A NUMERIC ARGUMENT IS OUT OF RANGE";
			break;
		case GL_INVALID_OPERATION:
			error = "THE SPECIFIED OPERATION IS NOT ALLOWED IN THE CURRENT STATE";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error = "THE FRAMEBUFFER OBJECT IS NOT COMPLETE";
			break;
		case GL_OUT_OF_MEMORY:
			error = "THERE IS NOT ENOUGH MEMORY LEFT TO EXECUTE THE COMMAND";
			break;
		case GL_STACK_UNDERFLOW:
			error = "AN INTERNAL STACK TO UNDERFLOW";
			break;
		case GL_STACK_OVERFLOW:
			error = "AN INTERNAL STACK TO OVERFLOW";
			break;
		};
	}

	//if (errorStackCount > 0)
	//{
	//	std::stringstream ss;
	//	ss << "handleGLErrors: " << error << std::endl;
	//	throw std::runtime_error(ss.str());
	//}

	return errorStackCount;
}

