#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

float playerX = 0.0f, playerZ = 5.0f;
float speed = 2.78f / 60.0f; // 10 km/jam dalam m/s

void drawCube(float size) {
    float x = size / 2.0f;
    glBegin(GL_QUADS);
    // Front
    glVertex3f(-x, -x, x); glVertex3f(x, -x, x); glVertex3f(x, x, x); glVertex3f(-x, x, x);
    // Back
    glVertex3f(-x, -x, -x); glVertex3f(-x, x, -x); glVertex3f(x, x, -x); glVertex3f(x, -x, -x);
    // Top
    glVertex3f(-x, x, -x); glVertex3f(-x, x, x); glVertex3f(x, x, x); glVertex3f(x, x, -x);
    // Bottom
    glVertex3f(-x, -x, -x); glVertex3f(x, -x, -x); glVertex3f(x, -x, x); glVertex3f(-x, -x, x);
    // Right
    glVertex3f(x, -x, -x); glVertex3f(x, x, -x); glVertex3f(x, x, x); glVertex3f(x, -x, x);
    // Left
    glVertex3f(-x, -x, -x); glVertex3f(-x, -x, x); glVertex3f(-x, x, x); glVertex3f(-x, x, -x);
    glEnd();
}

void drawWindows(float w, float h, float depth) {
    glColor3f(0.3f, 0.7f, 1.0f); // Biru terang

    int rows = (int)(h / 0.4f);
    int cols = (int)(w / 0.4f);
    float winW = w / (cols * 1.5f);
    float winH = h / (rows * 1.5f);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            float x = -w / 2 + j * (winW * 1.5f) + winW / 2;
            float y = -h / 2 + i * (winH * 1.5f) + winH / 2;

            glBegin(GL_QUADS);
            glVertex3f(x - winW / 2, y - winH / 2, depth);
            glVertex3f(x + winW / 2, y - winH / 2, depth);
            glVertex3f(x + winW / 2, y + winH / 2, depth);
            glVertex3f(x - winW / 2, y + winH / 2, depth);
            glEnd();
        }
    }
}

void drawBuilding(float width, float height, float depth) {
    glColor3f(0.5f, 0.5f, 0.5f); // Abu-abu
    glPushMatrix();
    glScalef(width, height, depth);
    drawCube(1.0f);
    glPopMatrix();

    // Tambahkan jendela pada sisi depan
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, depth / 2 + 0.01f);
    drawWindows(width, height, 0.0f);
    glPopMatrix();
}

void drawGround() {
    float repeatLength = 100.0f;
    float offsetZ = fmod(playerZ, repeatLength);

    for (int i = -1; i <= 1; ++i) {
        float zShift = i * repeatLength + offsetZ;
        glColor3f(0.1f, 0.1f, 0.1f); // Jalan
        glBegin(GL_QUADS);
        glVertex3f(-1.5f, 0.0f, zShift - repeatLength);
        glVertex3f(1.5f, 0.0f, zShift - repeatLength);
        glVertex3f(1.5f, 0.0f, zShift);
        glVertex3f(-1.5f, 0.0f, zShift);
        glEnd();
    }
}

void drawEnvironment() {
    float repeatLength = 100.0f;
    float offsetZ = fmod(playerZ, repeatLength);

    for (int i = -1; i <= 1; ++i) {
        float zShift = i * repeatLength + offsetZ;

        for (float z = zShift; z > zShift - repeatLength; z -= 15.0f) {
            glPushMatrix();
            glTranslatef(-10.0f, 4.0f, z);
            drawBuilding(2.0f, 8.0f, 2.0f);
            glPopMatrix();

            glPushMatrix();
            glTranslatef(10.0f, 4.0f, z);
            drawBuilding(2.0f, 8.0f, 2.0f);
            glPopMatrix();
        }
    }
}

void drawCharacter() {
    glPushMatrix();
    glTranslatef(playerX, 0.5f, playerZ);
    glColor3f(1.0f, 0.0f, 0.0f);
    drawCube(0.5f);
    glPopMatrix();
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        playerX -= 0.1f;
        if (playerX < -1.0f) playerX = -1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        playerX += 0.1f;
        if (playerX > 1.0f) playerX = 1.0f;
    }
}

void display(GLFWwindow* window) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    gluLookAt(playerX, 1.5f, playerZ + 2.0f, playerX, 1.0f, playerZ, 0.0f, 1.0f, 0.0f);

    drawGround();
    drawCharacter();
    drawEnvironment();

    glfwSwapBuffers(window);
}

int main() {
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(800, 600, "Temple Run 3D with Buildings", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, 800.0 / 600.0, 0.1, 500.0);
    glMatrixMode(GL_MODELVIEW);

    while (!glfwWindowShouldClose(window)) {
        playerZ -= speed; // Jalan otomatis
        processInput(window);
        display(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
