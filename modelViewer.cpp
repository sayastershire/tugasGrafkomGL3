#include "master.h"

float theta = 60.0f;
bool expandingFOV = true;

int main() {
	glewExperimental = true;
	if (!glfwInit()) {
		fprintf(stderr, "Gagal inisialisasi GLFW!:(\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Opens a window and create its OpenGL context
	GLFWwindow* window;
	window = glfwCreateWindow(WIDTH, HEIGHT, "Model Viewer", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to initialise Window.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialise GLEW.\n");
		return -1;
	}

	// 3d point buffer initialisation
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gVertexBufferData), gVertexBufferData, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gColorBufferData), gColorBufferData, GL_STATIC_DRAW);

	// Vertex Array Object - after context creation
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	

	GLuint programID = LoadShaders("SimpleVertexShader.glsl", "SimpleFragmentShader.glsl");


	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	do {
		// Projection matrix : 90° Field of View, aspect ratio depends on window, display range : 0.1 unit <-> 100 units
		glm::mat4 Projection = glm::perspective(glm::radians(theta), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);

		// Camera matrix
		glm::mat4 View = glm::lookAt(
			glm::vec3(4, 3, 3), // Camera is at (4,3,3), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);

		// Model matrix : an identity matrix (model will be at the origin)
		glm::mat4 Model = glm::mat4(1.0f);
		// Our ModelViewProjection : multiplication of our 3 matrices
		glm::mat4 mvp = Projection * View * Model; // Remember, matrix multiplication is the other way around


		// Get a handle for our "MVP" uniform
		// Only during the initialisation
		GLuint MatrixID = glGetUniformLocation(programID, "MVP");

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_BUFFER);
		glDepthFunc(GL_LESS);
		glUseProgram(programID);

		// draw here
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);
		glDrawArrays(GL_TRIANGLES, 0, 12*3);
		glDisableVertexAttribArray(0);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,
			3,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0
		);

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		// This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

		// end draw here
		// Swap buffers - do not touch!
		glfwSwapBuffers(window);
		glfwPollEvents();
		
		if (theta >= 105.0) expandingFOV = false;
		else if (theta <= 60.0) expandingFOV = true;

		switch (expandingFOV) {
		case false:
			theta -= addValue;
			break;
		case true:
			theta += addValue;
			break;
		}
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
	glfwTerminate();
	return 0;
}