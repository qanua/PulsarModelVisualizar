// main.cpp : アプリケーションのエントリ ポイントを定義します。
//

//#include "../include/main.h"
//
//using namespace std;
//
//int main()
//{
//	cout << "Hello CMake." << endl;
//	return 0;
//}

#include <iostream>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(800, 600, "test", nullptr, nullptr);

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}