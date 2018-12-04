#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>

#include <stdlib.h>
#include <stdio.h>

using namespace std;

/* Set error callback.*/
static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error[glfw]: %s\n", description);
}

/* Set key callback while */
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

/* Write a vertex shader with GLSL.*/
const char* vertexShaderSource = {
	"#version 450 core\n"
	"layout (location = 0) in vec3 vPosition;\n"
	"void main(){\n"
	"    gl_Position = vec4(vPosition.xyz, 1.0);\n"
	"}\n"
};

/* Write a fragment shader with GLSL.*/
const char* fragmentShaderSource = {
	"#version 450 core\n"
	"layout (location = 0) out vec4 fColor;\n"
	"void main(){\n"
	"    fColor = vec4(1.0, 0.5, 0.2, 1.0);\n"
	"}\n"
};

int main(void)
{
	/* Declair glfw window and set error callback. */
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);

	/* Initialize the library. */
	if (!glfwInit()) return -1;

	/* Set the version of OpenGL, v4.1 this time. */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window) {
		glfwTerminate();
		fprintf(stderr, "Error[glfw]: %s\n", "Can't create the OpenGL window.");
		return -1;
	}

	/* Set exit with Esc key pressed.*/
	glfwSetKeyCallback(window, key_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	/* Initialize glew library. */
	GLenum err = glewInit(); //must bellow the context 
	if (err != GLEW_OK) {
		fprintf(stderr, "Error[glew]: %s\n", glewGetErrorString(err));
		return  -1;
	}

	/* OpenGL operations. */
	glViewport(0, 0, 640, 480);//set viewport
	GLint success;//indicator of shader compile
	GLchar infoLog[512];

	// (1) Prepare vertex array.
	static const GLfloat vertices[] = {
		-0.5f, -0.5f,  0.0f,
		-0.5f,  0.5f,  0.0f,
		 0.5f,  0.5f,  0.0f,
		 0.5f, -0.5f,  0.0f
	};
	/* Elements indices of the rectangle.*/
	static const GLuint indices[] = {
	    0, 1, 2, //first triangle
	    0, 2, 3  //second triangle
	};

	// (2) Vertex input.
	GLuint VAO;//vertex array object
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);//binding this VAO to current context
	GLuint VBO;//vertex buffer object
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	GLuint EBO;//element buffer object
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// (3) Create vertex shader.
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {//check whether the shader is compiled successful
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		fprintf(stderr, "Error[vertex_shader]:%s\n", infoLog);
	}

	// (4) Create fragment shader.
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {//check whether the shader is compiled successful
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		fprintf(stderr, "Error[fragment_shader]:%s\n", infoLog);
	}

	// (5) Link shader program.
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {//check whether the shader programe is linked succesful
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		fprintf(stderr, "Error[shader_programe]:%s\n", infoLog);
	}

	// (6) Delete the shader after we linked the programe.
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// (7) Update vertex attribute to the GPU.
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// (8) Unlinked the vertex array object.
	glBindVertexArray(0);

	/* Loop until the user closes the window*/
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		/* Swap front and back buffers, update the
		contents rendered on the window. */
		glfwSwapBuffers(window);

		/* Poll for and process events. */
		glfwPollEvents();
	}

	/* Delete VBO and VAO.*/
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	/* Close the window. */
	glfwTerminate();
	return 0;
}