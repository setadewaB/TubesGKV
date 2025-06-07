#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <string>  // Untuk std::string
#include <iostream>  // Untuk std::cerr
#include <cstdlib>  // Untuk std::exit
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define PI 3.14159265358979323846f
const float AREA_MIN_X = -100.0f;
const float AREA_MAX_X =  100.0f;
const float AREA_MIN_Z = -100.0f;
const float AREA_MAX_Z =  100.0f;
GLuint groundTexture;
GLuint skyTexture;
GLuint buildingTexture;



bool gameOver = false;  // Status game over

float posXBadan = 0.0f, posYBadan = 1.0f, posZBadan = 0.0f;
float rotAngleY = 0.0f;  // Rotasi menghadap
float armAngle = 0.0f;
float legAngle = 0.0f;
bool walking = false;
float walkTime = 0.0f;

float npcWalkTime = 0.0f;
float npcArmAngle = 0.0f;
float npcLegAngle = 0.0f;

float animSpeed = 10.0f;

GLUquadric* quadric;

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);  

    quadric = gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
    gluQuadricTexture(quadric, GL_TRUE);
    

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat lightPos[] = { 0, 10, 10, 1 };
    GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

void reshape(GLFWwindow* window, int width, int height) {
    if (height == 0) height = 1;
    float ratio = (float)width / height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void drawCube(float size) {
    float half = size / 2.0f;
    glBegin(GL_QUADS);
    // Front face (z = half)
    glNormal3f(0,0,1);
    glVertex3f(-half, -half, half);
    glVertex3f(half, -half, half);
    glVertex3f(half, half, half);
    glVertex3f(-half, half, half);
    // Back face (z = -half)
    glNormal3f(0,0,-1);
    glVertex3f(-half, -half, -half);
    glVertex3f(-half, half, -half);
    glVertex3f(half, half, -half);
    glVertex3f(half, -half, -half);
    // Left face (x = -half)
    glNormal3f(-1,0,0);
    glVertex3f(-half, -half, -half);
    glVertex3f(-half, -half, half);
    glVertex3f(-half, half, half);
    glVertex3f(-half, half, -half);
    // Right face (x = half)
    glNormal3f(1,0,0);
    glVertex3f(half, -half, -half);
    glVertex3f(half, half, -half);
    glVertex3f(half, half, half);
    glVertex3f(half, -half, half);
    // Top face (y = half)
    glNormal3f(0,1,0);
    glVertex3f(-half, half, -half);
    glVertex3f(-half, half, half);
    glVertex3f(half, half, half);
    glVertex3f(half, half, -half);
    // Bottom face (y = -half)
    glNormal3f(0,-1,0);
    glVertex3f(-half, -half, -half);
    glVertex3f(half, -half, -half);
    glVertex3f(half, -half, half);
    glVertex3f(-half, -half, half);
    glEnd();
}


struct Building {
    float x, z;      // posisi pusat gedung di bidang XZ
    float width;     // lebar gedung (sumbu X)
    float depth;     // kedalaman gedung (sumbu Z)
    float height;    // tinggi gedung (bisa untuk rendering)
    
};

std::vector<Building> buildings;

float randomFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void generateBuildings(int count) {
    buildings.clear();
    srand(static_cast<unsigned int>(time(nullptr)));  // seed random

    for (int i = 0; i < count; i++) {
        Building b;
        b.x = randomFloat(AREA_MIN_X, AREA_MAX_X);
        b.z = randomFloat(AREA_MIN_Z, AREA_MAX_Z);
        b.width = randomFloat(5.0f, 15.0f);   
        b.depth = randomFloat(5.0f, 15.0f);
        b.height = randomFloat(10.0f, 50.0f);

        buildings.push_back(b);
    }
}

void drawBuilding(const Building& b, GLuint buildingTexture) {
    float halfW = b.width / 2.0f;
    float halfD = b.depth / 2.0f;

    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    glTranslatef(b.x, 0.0f, b.z);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, buildingTexture);

    glBegin(GL_QUADS);

    // sisi bawah (lantai)
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfW, 0, -halfD);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( halfW, 0, -halfD);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( halfW, 0,  halfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfW, 0,  halfD);

    // sisi depan
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfW, 0, halfD);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( halfW, 0, halfD);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( halfW, b.height, halfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfW, b.height, halfD);

    // sisi belakang
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfW, 0, -halfD);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( halfW, 0, -halfD);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( halfW, b.height, -halfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfW, b.height, -halfD);

    // sisi kiri
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-halfW, 0, -halfD);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-halfW, 0, halfD);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-halfW, b.height, halfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-halfW, b.height, -halfD);

    // sisi kanan
    glTexCoord2f(0.0f, 0.0f); glVertex3f(halfW, 0, -halfD);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(halfW, 0, halfD);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(halfW, b.height, halfD);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(halfW, b.height, -halfD);

    glEnd();


    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

void drawBuildings() {
    for (const auto& b : buildings) {
        drawBuilding(b, buildingTexture);
    }
}

bool checkBuildingCollision(float newX, float newZ) {
    float playerSize = 0.5f; // radius collision pemain

    for (const auto& b : buildings) {
        float minX = b.x - b.width / 2.0f - playerSize;
        float maxX = b.x + b.width / 2.0f + playerSize;
        float minZ = b.z - b.depth / 2.0f - playerSize;
        float maxZ = b.z + b.depth / 2.0f + playerSize;

        if (newX >= minX && newX <= maxX &&
            newZ >= minZ && newZ <= maxZ) {
            return true;
        }
    }
    return false;
}

// Zone
struct Zone {
    float x, z;
    float radius;
};
Zone activeZone;
int score = 0;

void spawnZone() {
    float zoneRadius = 3.0f;
    int attempts = 100;
    while (attempts--) {
        float x = randomFloat(AREA_MIN_X + zoneRadius, AREA_MAX_X - zoneRadius);
        float z = randomFloat(AREA_MIN_Z + zoneRadius, AREA_MAX_Z - zoneRadius);

        // Cek tabrakan dengan gedung
        bool collide = false;
        for (const auto& b : buildings) {
            float minX = b.x - b.width / 2.0f - zoneRadius;
            float maxX = b.x + b.width / 2.0f + zoneRadius;
            float minZ = b.z - b.depth / 2.0f - zoneRadius;
            float maxZ = b.z + b.depth / 2.0f + zoneRadius;
            if (x >= minX && x <= maxX && z >= minZ && z <= maxZ) {
                collide = true;
                break;
            }
        }
        if (!collide) {
            activeZone.x = x;
            activeZone.z = z;
            activeZone.radius = zoneRadius;
            return;
        }
    }
    // Jika gagal, letakkan di tengah
    activeZone.x = 0.0f;
    activeZone.z = 0.0f;
    activeZone.radius = zoneRadius;
}


void drawZone() {
    glPushMatrix();
    glTranslatef(activeZone.x, 0.05f, activeZone.z);
    glColor4f(0.0f, 1.0f, 0.0f, 0.5f); // hijau transparan
    glDisable(GL_LIGHTING);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(0, 0, 0);
    for (int i = 0; i <= 32; ++i) {
        float angle = 2.0f * PI * i / 32;
        glVertex3f(cos(angle) * activeZone.radius, 0, sin(angle) * activeZone.radius);
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix();
}
// Zone End

// Supply
struct Supply {
    float x, y, z;
    bool active;
};
std::vector<Supply> supplies;
bool carryingSupply = false;
bool firstSupplySpawned = false;
bool keyPressedE = false;

void drawSupply() {
    // Gambar semua supply yang masih aktif di dunia
    for (const auto& s : supplies) {
        if (s.active) {
            glColor3f(0.2f, 0.6f, 0.8f);
            glPushMatrix();
            glTranslatef(s.x, s.y, s.z);
            glScalef(1.5f, 0.5f, 1.0f);
            drawCube(1.0f);
            glPopMatrix();
        }
    }

    // Jika sedang membawa supply, gambar supply di atas kepala
    if (carryingSupply) {
        glPushMatrix();
        // Pusatkan ke posisi player
        glTranslatef(posXBadan, posYBadan, posZBadan);
        // Rotasi sesuai arah player
        glRotatef(rotAngleY, 0, 1, 0);
        // Pindahkan ke atas kepala (relatif terhadap badan)
        glTranslatef(0.0f, 1.7f, 0.0f);
        glColor3f(0.2f, 0.6f, 0.8f);
        glScalef(1.5f, 0.5f, 1.0f);
        drawCube(1.0f);
        glPopMatrix();
    }
}




void spawnSupply() {
    float headY = 1.5f; // Sama seperti posisi kepala karakter

    if (!firstSupplySpawned) {
        supplies.push_back({ posXBadan, headY, posZBadan - 3.0f, true });
        firstSupplySpawned = true;
    } else {
        int attempts = 100;
        while (attempts--) {
            float x = ((rand() % 1000) / 10.0f) - 50.0f;
            float z = ((rand() % 1000) / 10.0f) - 50.0f;
            if (!checkBuildingCollision(x, z)) {
                supplies.push_back({ x, headY, z, true });
                break;
            }
        }
    }
}


void pickupSupply() {
    if (carryingSupply) return;

    for (auto& s : supplies) {
        if (s.active) {
            float dx = s.x - posXBadan;
            float dz = s.z - posZBadan;
            float distance = sqrt(dx * dx + dz * dz);
            if (distance < 2.0f) { // dekat cukup
                s.active = false;
                carryingSupply = true;
                std::cout << "Picked up supply!\n";
                break;
            }
        }
    }
}


void dropSupply() {
    if (carryingSupply) {
        // Cek apakah supply dijatuhkan di dalam zona
        float dx = posXBadan - activeZone.x;
        float dz = posZBadan - activeZone.z;
        float dist = sqrt(dx * dx + dz * dz);
        if (dist <= activeZone.radius) {
            score++;
            std::cout << "Supply in zone! Score: " << score << std::endl;
            spawnSupply();
            spawnZone();
        } else {
            supplies.push_back({ posXBadan, 1.5f, posZBadan, true });
        }
        carryingSupply = false;
        std::cout << "Supply dijatuhkan!\n";
    }
}

// Supply End

void drawPlayer() {
    glPushMatrix();
    glTranslatef(posXBadan, posYBadan, posZBadan);
    glRotatef(rotAngleY, 0, 1, 0);

    // Badan atas
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glColor3f(0.0f, 0.2f, 1.0f);
    glPushMatrix();
    glScalef(0.75f, 0.4f, 0.4f);
    drawCube(1.0f);
    glPopMatrix();

    // Badan bawah
    glPushMatrix();
    glTranslatef(0.0f, -0.3f, 0.0f);
    glColor3f(1.0f, 0.2f, 0.0f);
    glPushMatrix();
    glScalef(0.65f, 0.3f, 0.25f);
    drawCube(1.0f);
    glPopMatrix();
    glPopMatrix();

    // Kepala
    glPushMatrix();
    glTranslatef(0.0f, 1.1f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    GLUquadric* quad = gluNewQuadric();
    gluSphere(quad, 0.4f, 20, 20);
    gluDeleteQuadric(quad);
    glPopMatrix();

    // Antena Kiri
    glPushMatrix();
    glTranslatef(-0.3f, 1.6f, 0.3f);
    glRotatef(45, 0, 0, 1);
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glScalef(0.05f, 0.7f, 0.05f);
    drawCube(1.0f);
    glPopMatrix();
    glPopMatrix();

    // Antena Kanan
    glPushMatrix();
    glTranslatef(0.3f, 1.6f, 0.3f);
    glRotatef(-45, 0, 0, 1);
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glScalef(0.05f, 0.7f, 0.05f);
    drawCube(1.0f);
    glPopMatrix();
    glPopMatrix();

    // Kotak Merah di Antara Antena
    glPushMatrix();
    glTranslatef(0.0f, 1.4f, 0.3f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glScalef(0.2f, 0.2f, 0.2f);
    drawCube(1.0f);
    glPopMatrix();
    glPopMatrix();

    // Lengan Kiri (pivot di bahu)
    glPushMatrix();
    glTranslatef(-0.5f, 0.7f, 0.0f);      // ke posisi bahu
    if (carryingSupply) {
        glRotatef(-180.0f, 1, 0, 0); // ke atas (sumbu X negatif)
    } else {
        glRotatef(armAngle, 1, 0, 0); // animasi jalan
    }
    glTranslatef(0.0f, -0.375f, 0.0f);    // ke tengah lengan
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glScalef(0.25f, 0.75f, 0.25f);
    drawCube(1.0f);
    glPopMatrix();

    // Tangan Kiri (sphere di ujung bawah lengan)
    glPushMatrix();
    glTranslatef(0.0f, -0.375f, 0.0f); // ke ujung bawah lengan
    glColor3f(0.5f, 0.5f, 0.5f);
    GLUquadric* handLeft = gluNewQuadric();
    gluSphere(handLeft, 0.15f, 16, 16);
    gluDeleteQuadric(handLeft);
    glPopMatrix();

    glPopMatrix();

    // Lengan Kanan (pivot di bahu)
    glPushMatrix();
    glTranslatef(0.5f, 0.7f, 0.0f);
    if (carryingSupply) {
        glRotatef(-180.0f, 1, 0, 0); // ke atas juga
    } else {
        glRotatef(-armAngle, 1, 0, 0);
    }
    glTranslatef(0.0f, -0.375f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glScalef(0.25f, 0.75f, 0.25f);
    drawCube(1.0f);
    glPopMatrix();

    // Tangan Kanan (sphere di ujung bawah lengan)
    glPushMatrix();
    glTranslatef(0.0f, -0.375f, 0.0f);
    glColor3f(0.5f, 0.5f, 0.5f);
    GLUquadric* handRight = gluNewQuadric();
    gluSphere(handRight, 0.15f, 16, 16);
    gluDeleteQuadric(handRight);
    glPopMatrix();

    glPopMatrix();

    // Pinggang
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glScalef(0.75f, 0.2f, 0.3f);
    drawCube(1.0f);
    glPopMatrix();
    glPopMatrix();

// Kaki Kiri (pivot di pinggang)
glPushMatrix();
glTranslatef(-0.25f, -0.225f, 0.0f);    // ke posisi pangkal kaki kiri
glRotatef(legAngle, 1, 0, 0);          // rotasi kaki kiri di pangkal
glTranslatef(0.0f, -0.25f, 0.0f);     // ke tengah kaki kiri
glColor3f(1.0f, 1.0f, 1.0f);
glPushMatrix();
glScalef(0.25f, 0.75f, 0.25f);
drawCube(1.0f);
glPopMatrix();

// Telapak Kiri (ikut kaki)
glPushMatrix();
glTranslatef(0.0f, -0.35f, 0.1f); // ke ujung bawah kaki kiri, lalu sedikit ke depan
glColor3f(1.0f, 0.0f, 0.0f);
glPushMatrix();
glScalef(0.35f, 0.15f, 0.7f);
drawCube(1.0f);
glPopMatrix();
glPopMatrix();


// Kaki Kanan (pivot di pinggang)
glPushMatrix();
glTranslatef(0.25f, -0.225f, 0.0f);     // ke posisi pangkal kaki kanan
glRotatef(-legAngle, 1, 0, 0);         // rotasi kaki kanan di pangkal
glTranslatef(0.0f, -0.25f, 0.0f);     // ke tengah kaki kanan
glColor3f(1.0f, 1.0f, 1.0f);
glPushMatrix();
glScalef(0.25f, 0.75f, 0.25f);
drawCube(1.0f);
glPopMatrix();

// Telapak Kanan (ikut kaki)
glPushMatrix();
glTranslatef(0.0f, -0.35f, 0.1f); // ke ujung bawah kaki kanan, lalu sedikit ke depan
glColor3f(1.0f, 0.0f, 0.0f);
glPushMatrix();
glScalef(0.35f, 0.15f, 0.7f);
drawCube(1.0f);
glPopMatrix();
glPopMatrix();

    glPopMatrix();
}

GLuint loadTexture(const char* filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return textureID;
}

void renderSky() {
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);
    // Posisi langit di posisi kamera agar tidak terlihat bergeser
    // Dapatkan posisi kamera (posXBadan, posYBadan, posZBadan) sebagai pusat langit
    glTranslatef(posXBadan, posYBadan, posZBadan);

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, skyTexture);

    // Sphere besar, radius besar agar menutupi seluruh scene
    float radius = 60.0f;

    // Saat render sphere untuk langit, biasanya kita balik normal dan inside-out biar texture tampil benar dari dalam sphere.
    // gluQuadricOrientation = GLU_INSIDE untuk normal mengarah ke dalam sphere
    gluQuadricOrientation(quadric, GLU_INSIDE);

    gluSphere(quadric, radius, 40, 40);

    // Kembalikan ke luar sphere untuk objek lain
    gluQuadricOrientation(quadric, GLU_OUTSIDE);

    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);

    glPopMatrix();
}


void updateCamera() {
    float camDistance = 10.0f;   // Jarak kamera dari pemain
    float camHeight = 5.0f;      // Kalau kamu ingin, bisa pakai untuk offset vertikal tambahan
    float pitchAngle = -10.0f;    // Pitch dalam derajat (positif = lihat ke bawah)

    // Konversi sudut ke radian
    float yawRad = rotAngleY * PI / 180.0f;
    float pitchRad = pitchAngle * PI / 180.0f;

    // Hitung posisi kamera relatif terhadap pemain dengan pitch dan yaw
    float camX = posXBadan - camDistance * cos(pitchRad) * sin(yawRad);
    float camY = posYBadan + camDistance * sin(pitchRad) + camHeight;
    float camZ = posZBadan - camDistance * cos(pitchRad) * cos(yawRad);

    // Titik fokus kamera adalah posisi pemain dengan offset vertikal (misal kepala)
    float targetX = posXBadan;
    float targetY = posYBadan + 3.0f;  // sedikit naik agar fokus ke kepala
    float targetZ = posZBadan;

    glLoadIdentity();
    gluLookAt(camX, camY, camZ,
              targetX, targetY, targetZ,
              0, 1, 0);
}


void processInput(GLFWwindow* window, float deltaTime) {
    if (gameOver) return;

    float speed = 25.0f;        // Kecepatan gerak (unit per detik)
    float rotSpeed = 90.0f;     // Kecepatan rotasi (derajat per detik)

    // Rotasi kamera dengan Arrow Left/Right
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        rotAngleY += rotSpeed * deltaTime;  // Putar ke kiri
        if (rotAngleY >= 360.0f) rotAngleY -= 360.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        rotAngleY -= rotSpeed * deltaTime;  // Putar ke kanan
        if (rotAngleY < 0.0f) rotAngleY += 360.0f;
    }

    // Gerak maju mundur (W/S)
    float forward = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) forward += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) forward -= 1.0f;

    // Gerak kiri/kanan (A/D)
    float strafe = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) strafe -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) strafe += 1.0f;

    // Hitung arah gerak
    float angleRad = rotAngleY * PI / 180.0f;
    float moveX = sin(angleRad) * forward + cos(angleRad) * strafe;
    float moveZ = cos(angleRad) * forward - sin(angleRad) * strafe;

    if (forward != 0.0f || strafe != 0.0f) {
        // Normalisasi agar kecepatan diagonal tetap
        float len = sqrt(moveX * moveX + moveZ * moveZ);
        if (len > 0.0f) {
            moveX /= len;
            moveZ /= len;
        }

        float deltaX = moveX * speed * deltaTime;
        float deltaZ = moveZ * speed * deltaTime;

        float newX = posXBadan + deltaX;
        float newZ = posZBadan + deltaZ;

        // Cek collision batas area terlebih dahulu
        if (newX >= AREA_MIN_X && newX <= AREA_MAX_X &&
            newZ >= AREA_MIN_Z && newZ <= AREA_MAX_Z) {

            // Cek collision dengan bangunan
            bool collideX = checkBuildingCollision(newX, posZBadan);
            bool collideZ = checkBuildingCollision(posXBadan, newZ);
            bool collideBoth = checkBuildingCollision(newX, newZ);

            // Sliding collision
            if (!collideBoth) {
                posXBadan = newX;
                posZBadan = newZ;
            } else if (!collideX && collideZ) {
                posXBadan = newX;
            } else if (collideX && !collideZ) {
                posZBadan = newZ;
            }
            // Jika collideX && collideZ: tidak bergerak (terhalang sepenuhnya)
        }
    }

    // Pickup/drop supply tetap
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !keyPressedE) {
        keyPressedE = true;
        pickupSupply();
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        keyPressedE = false;
        dropSupply();
    }
}


void renderGround() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    glDisable(GL_LIGHTING);

    glColor3f(1.0f, 1.0f, 1.0f); // warna putih agar tekstur tidak dipengaruhi

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(AREA_MIN_X, 0.0f, AREA_MIN_Z);
    glTexCoord2f(10.0f, 0.0f); glVertex3f(AREA_MAX_X, 0.0f, AREA_MIN_Z);
    glTexCoord2f(10.0f, 10.0f); glVertex3f(AREA_MAX_X, 0.0f, AREA_MAX_Z);
    glTexCoord2f(0.0f, 10.0f); glVertex3f(AREA_MIN_X, 0.0f, AREA_MAX_Z);
    glEnd();

    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
}



//NPC
float npcX = 20.0f, npcY = 2.0f, npcZ = 20.0f;  // Posisi awal NPC
float npcSpeed = 5.0f;  // Kecepatan pergerakan NPC
float dx = posXBadan - npcX;
float dz = posZBadan - npcZ;
float npcYaw = atan2(dx, dz) * 180.0f / M_PI;
float collisionDistance = 1.5f;  // Jarak ketika NPC menyentuh pemain (dalam unit)


void updateNpcPosition(float deltaTime) {
    // Hitung arah dari NPC menuju pemain (hanya X dan Z untuk gerak horizontal)
    float deltaX = posXBadan - npcX;
    float deltaY = posYBadan - npcY;
    float deltaZ = posZBadan - npcZ;

    // Hitung jarak 3D
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);

    if (distance > 0.1f) {
        // Normalisasi arah
        float moveX = deltaX / distance;
        float moveY = deltaY / distance;
        float moveZ = deltaZ / distance;

        // Posisi baru horizontal
        float newX = npcX + moveX * npcSpeed * deltaTime;
        float newZ = npcZ + moveZ * npcSpeed * deltaTime;

        // Cek collision gedung
        bool collisionFull = checkBuildingCollision(newX, newZ);
        bool collisionX = checkBuildingCollision(newX, npcZ);
        bool collisionZ = checkBuildingCollision(npcX, newZ);

        // Sliding logic
        if (!collisionFull) {
            npcX = newX;
            npcZ = newZ;
        } else if (!collisionX && collisionZ) {
            npcX = newX;
            // npcZ tetap
        } else if (collisionX && !collisionZ) {
            // npcX tetap
            npcZ = newZ;
        } else {
            // collision di dua sumbu, NPC berhenti bergerak
        }

        // Update posisi Y (naik turun) tanpa collision untuk sekarang
        npcY += moveY * npcSpeed * deltaTime;
    }

    // Cek apakah NPC menyentuh pemain (Game Over)
    float dist3D = sqrt(
        (posXBadan - npcX) * (posXBadan - npcX) +
        (posYBadan - npcY) * (posYBadan - npcY) +
        (posZBadan - npcZ) * (posZBadan - npcZ)
    );

    if (dist3D < collisionDistance) {
        gameOver = true;
    }
}

void drawNpc() {
    glPushMatrix();
    glTranslatef(npcX, npcY, npcZ);  // Posisi NPC
    glColor3f(1.0f, 0.0f, 0.0f);    // Warna merah

    float dx = posXBadan - npcX;
    float dz = posZBadan - npcZ;
    float npcYaw = atan2(dx, dz) * 180.0f / M_PI;
    glRotatef(npcYaw, 0, 1, 0);

        // Badan atas
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, 0.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glScalef(0.75f, 0.4f, 0.4f);
    drawCube(1.0f);
    glPopMatrix();

    // Badan bawah
    glPushMatrix();
    glTranslatef(0.0f, -0.3f, 0.0f);
    glColor3f(1.0f, 0.2f, 0.0f);
    glPushMatrix();
    glScalef(0.65f, 0.3f, 0.25f);
    drawCube(1.0f);
    glPopMatrix();
    glPopMatrix();

    // Kepala
    glPushMatrix();
    glTranslatef(0.0f, 1.1f, 0.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    GLUquadric* quad = gluNewQuadric();
    gluSphere(quad, 0.4f, 20, 20);
    gluDeleteQuadric(quad);
    glPopMatrix();

    // Antena Kiri
    glPushMatrix();
    glTranslatef(-0.3f, 1.6f, 0.3f);
    glRotatef(45, 0, 0, 1);
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glScalef(0.05f, 0.7f, 0.05f);
    drawCube(1.0f);
    glPopMatrix();
    glPopMatrix();

    // Antena Kanan
    glPushMatrix();
    glTranslatef(0.3f, 1.6f, 0.3f);
    glRotatef(-45, 0, 0, 1);
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
    glScalef(0.05f, 0.7f, 0.05f);
    drawCube(1.0f);
    glPopMatrix();
    glPopMatrix();

    // Kotak Merah di Antara Antena
    glPushMatrix();
    glTranslatef(0.0f, 1.4f, 0.3f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glScalef(0.2f, 0.2f, 0.2f);
    drawCube(1.0f);
    glPopMatrix();
    glPopMatrix();

    // Lengan Kiri (pivot di bahu)
    glPushMatrix();
    glTranslatef(-0.5f, 0.7f, 0.0f);      // ke posisi bahu
    glRotatef(npcArmAngle, 1, 0, 0);         // rotasi di bahu
    glTranslatef(0.0f, -0.375f, 0.0f);    // ke tengah lengan
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glScalef(0.25f, 0.75f, 0.25f);
    drawCube(1.0f);
    glPopMatrix();

    // Tangan Kiri (sphere di ujung bawah lengan)
    glPushMatrix();
    glTranslatef(0.0f, -0.375f, 0.0f); // ke ujung bawah lengan
    glColor3f(0.5f, 0.5f, 0.5f);
    GLUquadric* handLeft = gluNewQuadric();
    gluSphere(handLeft, 0.15f, 16, 16);
    gluDeleteQuadric(handLeft);
    glPopMatrix();

    glPopMatrix();

    // Lengan Kanan (pivot di bahu)
    glPushMatrix();
    glTranslatef(0.5f, 0.7f, 0.0f);
    glRotatef(-npcArmAngle, 1, 0, 0);
    glTranslatef(0.0f, -0.375f, 0.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glScalef(0.25f, 0.75f, 0.25f);
    drawCube(1.0f);
    glPopMatrix();

    // Tangan Kanan (sphere di ujung bawah lengan)
    glPushMatrix();
    glTranslatef(0.0f, -0.375f, 0.0f);
    glColor3f(0.5f, 0.5f, 0.5f);
    GLUquadric* handRight = gluNewQuadric();
    gluSphere(handRight, 0.15f, 16, 16);
    gluDeleteQuadric(handRight);
    glPopMatrix();

    glPopMatrix();

    // Pinggang
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glScalef(0.75f, 0.2f, 0.3f);
    drawCube(1.0f);
    glPopMatrix();
    glPopMatrix();

    // Kaki Kiri (pivot di pinggang)
    glPushMatrix();
    glTranslatef(-0.25f, -0.225f, 0.0f);    // ke posisi pangkal kaki kiri
    glRotatef(npcLegAngle, 1, 0, 0);          // rotasi kaki kiri di pangkal
    glTranslatef(0.0f, -0.25f, 0.0f);     // ke tengah kaki kiri
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glScalef(0.25f, 0.75f, 0.25f);
    drawCube(1.0f);
    glPopMatrix();

    // Telapak Kiri (ikut kaki)
    glPushMatrix();
    glTranslatef(0.0f, -0.35f, 0.1f); // ke ujung bawah kaki kiri, lalu sedikit ke depan
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glScalef(0.35f, 0.15f, 0.7f);
    drawCube(1.0f);
    glPopMatrix();
    glPopMatrix();


    // Kaki Kanan (pivot di pinggang)
    glPushMatrix();
    glTranslatef(0.25f, -0.225f, 0.0f);     // ke posisi pangkal kaki kanan
    glRotatef(-npcLegAngle, 1, 0, 0);         // rotasi kaki kanan di pangkal
    glTranslatef(0.0f, -0.25f, 0.0f);     // ke tengah kaki kanan
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glScalef(0.25f, 0.75f, 0.25f);
    drawCube(1.0f);
    glPopMatrix();

    // Telapak Kanan (ikut kaki)
    glPushMatrix();
    glTranslatef(0.0f, -0.35f, 0.1f); // ke ujung bawah kaki kanan, lalu sedikit ke depan
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glScalef(0.35f, 0.15f, 0.7f);
    drawCube(1.0f);
    glPopMatrix();
    glPopMatrix();

    glPopMatrix();
}



//TEXT
#define WIDTH 800
#define HEIGHT 600

// Bitmap font ASCII 32-127
const unsigned char font[96][5] = {
    {0x7E, 0x09, 0x09, 0x09, 0x7E}, // A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // C
    {0x7F, 0x41, 0x41, 0x41, 0x3E}, // D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // F
    {0x3E, 0x41, 0x49, 0x49, 0x2E}, // G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H
    {0x41, 0x41, 0x7F, 0x41, 0x41}, // I
    {0x02, 0x01, 0x01, 0x01, 0x7E}, // J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // K
    {0x7F, 0x01, 0x01, 0x01, 0x01}, // L
    {0x7F, 0x20, 0x10, 0x20, 0x7F}, // M
    {0x7F, 0x10, 0x20, 0x40, 0x7F}, // N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // P
    {0x3E, 0x41, 0x49, 0x49, 0x3E}, // Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // V
    {0x3F, 0x40, 0x30, 0x40, 0x3F}, // W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // X
    {0x11, 0x0A, 0x04, 0x0A, 0x11}, // Y
    {0x71, 0x49, 0x45, 0x43, 0x00}, // Z
    // Add characters for digits, punctuation, etc.
};

// Font angka 8x8 (vertikal, tiap baris = 1 byte, 0 = kosong, 1 = isi)
const unsigned char digitFont[10][8] = {
    {0x3C,0x66,0x6E,0x76,0x66,0x66,0x3C,0x00}, // 0
    {0x18,0x38,0x18,0x18,0x18,0x18,0x7E,0x00}, // 1
    {0x3C,0x66,0x06,0x0C,0x18,0x30,0x7E,0x00}, // 2
    {0x3C,0x66,0x06,0x1C,0x06,0x66,0x3C,0x00}, // 3
    {0x0C,0x1C,0x3C,0x6C,0x7E,0x0C,0x0C,0x00}, // 4
    {0x7E,0x60,0x7C,0x06,0x06,0x66,0x3C,0x00}, // 5
    {0x1C,0x30,0x60,0x7C,0x66,0x66,0x3C,0x00}, // 6
    {0x7E,0x06,0x0C,0x18,0x30,0x30,0x30,0x00}, // 7
    {0x3C,0x66,0x66,0x3C,0x66,0x66,0x3C,0x00}, // 8
    {0x3C,0x66,0x66,0x3E,0x06,0x0C,0x38,0x00}  // 9
};

// Fungsi untuk menampilkan angka besar vertikal (hanya angka)
void drawBigNumber(float x, float y, const std::string& num, float scale = 6.0f) {
    // Hapus glDisable(GL_LIGHTING); dan glEnable(GL_LIGHTING); dari sini!
    float charWidth = 8 * scale;
    float charHeight = 8 * scale;
    glPushMatrix();
    glTranslatef(x, y, 0);
    glDisable(GL_TEXTURE_2D);
    for (size_t i = 0; i < num.length(); i++) {
        char c = num[i];
        if (c >= '0' && c <= '9') {
            const unsigned char* bitmap = digitFont[c - '0'];
            for (int row = 0; row < 8; ++row) {
                unsigned char bits = bitmap[row];
                for (int col = 0; col < 8; ++col) {
                    if (bits & (1 << (7 - col))) {
                        glBegin(GL_QUADS);
                        glVertex2f(i * charWidth + col * scale, -row * scale);
                        glVertex2f(i * charWidth + (col + 1) * scale, -row * scale);
                        glVertex2f(i * charWidth + (col + 1) * scale, -(row + 1) * scale);
                        glVertex2f(i * charWidth + col * scale, -(row + 1) * scale);
                        glEnd();
                    }
                }
            }
        }
    }
    glEnable(GL_TEXTURE_2D);
    glPopMatrix();
}


void drawText(float x, float y, const std::string& text) {
    glPushMatrix();
    glRasterPos2f(x, y);  // Tentukan posisi awal teks

    for (size_t i = 0; i < text.length(); i++) {
        unsigned char character = text[i];
        if (character >= 32 && character <= 127) {
            // Ambil font berdasarkan indeks ASCII
            const unsigned char* charBitmap = font[character - 32];
            glBitmap(8, 16, 0, 0, 0, 0, charBitmap);  // Gambar karakter
        }
    }

    glPopMatrix();
}


void renderGameOver() {
    if (gameOver) {
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);  // Mengubah latar belakang menjadi merah
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Membersihkan layar dengan warna merah

        glColor3f(1.0f, 1.0f, 1.0f);  // Warna putih untuk teks

        // Tentukan posisi teks di tengah layar
        glRasterPos2f(-0.3f, 0.0f);

        // Tampilkan teks "Game Over!"
        drawText(-0.3f, 0.0f, "Game Over!");
    }
}





//MAIN

int main() {
    double lastTime = glfwGetTime();
    generateBuildings(30);
    spawnZone();
    spawnSupply();  // Spawn supply pertama kali
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(800, 600, "Simple Player & Camera", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();

        init();  // Inisialisasi OpenGL (lighting, quadric, dsb.)
    glEnable(GL_TEXTURE_2D);
    

    // Load texture (jika ada file .jpg atau .png untuk ground)
    int texWidth, texHeight, texChannels;
    unsigned char* image = stbi_load("textures/dirt2.png", &texWidth, &texHeight, &texChannels, 0);
    if (image) {
        glGenTextures(1, &groundTexture);
        glBindTexture(GL_TEXTURE_2D, groundTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, texChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(image);
    } else {
        std::cerr << "Failed to load ground texture!\n";
        std::exit(EXIT_FAILURE);
    }

    skyTexture = loadTexture("textures/sky3.png");  // Pastikan ada file sky.jpg
    buildingTexture = loadTexture("textures/buildingtext.png");

    glfwSetFramebufferSizeCallback(window, reshape);
    reshape(window, WIDTH, HEIGHT);  // Set viewport awal

    // Loop utama
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastTime);
        lastTime = currentTime;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        updateCamera();

        if (!gameOver) {
            processInput(window, deltaTime);
            updateNpcPosition(deltaTime);
        }
        renderSky();     
        renderGround();
        drawBuildings();
        drawSupply();
        
        if (
            glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS
        ) {
            walking = true;
            walkTime += deltaTime * animSpeed; // kecepatan animasi
        } else {
            walking = false;
            walkTime = 0.0f;
        }
        if (walking) {
            armAngle = 30.0f * sin(walkTime);
            legAngle = 30.0f * -sin(walkTime);
        } else {
            armAngle = 0.0f;
            legAngle = 0.0f;
        }

        drawZone();  
        drawPlayer();
        drawNpc();
        
        // Tampilkan skor
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluOrtho2D(0, WIDTH, 0, HEIGHT);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
        glDisable(GL_LIGHTING); // Matikan lighting sebelum skor
        glColor3f(1,1,0);
        drawBigNumber(20, HEIGHT - 40, std::to_string(score), 8.0f);
        glEnable(GL_LIGHTING);  // Aktifkan kembali lighting setelah skor
        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        npcWalkTime += deltaTime * animSpeed; // kecepatan animasi NPC
        npcArmAngle = 30.0f * sin(npcWalkTime);
        npcLegAngle = 30.0f * -sin(npcWalkTime);
        

        renderGameOver();  // Menampilkan teks Game Over jika gameOver == true

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    gluDeleteQuadric(quadric);  // Bersihkan memori
    glfwTerminate();
    return 0;
}
