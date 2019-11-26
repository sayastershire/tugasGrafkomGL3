#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

GLuint LoadShaders(const char* vertexFilePath, const char* fragmentFilePath);
