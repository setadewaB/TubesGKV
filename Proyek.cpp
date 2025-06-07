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

#define PI 3.14159265358979323846f
const float AREA_MIN_X = -100.0f;
const float AREA_MAX_X =  100.0f;
const float AREA_MIN_Z = -100.0f;
const float AREA_MAX_Z =  100.0f;
GLuint groundTexture;
GLuint skyTexture;



bool gameOver = false;  // Status game over

float posXBadan = 0.0f, posYBadan = 0.0f, posZBadan = 0.0f;
float rotAngleY = 0.0f;  // Rotasi menghadap

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

void drawBuilding(const Building& b) {
    float halfW = b.width / 2.0f;
    float halfD = b.depth / 2.0f;

    glPushMatrix();
    glTranslatef(b.x, 0.0f, b.z);

    glColor3f(0.7f, 0.7f, 0.7f);  // warna gedung

    glBegin(GL_QUADS);
    // gambar sisi bawah (lantai)
    glVertex3f(-halfW, 0, -halfD);
    glVertex3f( halfW, 0, -halfD);
    glVertex3f( halfW, 0,  halfD);
    glVertex3f(-halfW, 0,  halfD);

    // gambar atap
    glVertex3f(-halfW, b.height, -halfD);
    glVertex3f( halfW, b.height, -halfD);
    glVertex3f( halfW, b.height,  halfD);
    glVertex3f(-halfW, b.height,  halfD);

    // sisi depan
    glVertex3f(-halfW, 0, halfD);
    glVertex3f( halfW, 0, halfD);
    glVertex3f( halfW, b.height, halfD);
    glVertex3f(-halfW, b.height, halfD);

    // sisi belakang
    glVertex3f(-halfW, 0, -halfD);
    glVertex3f( halfW, 0, -halfD);
    glVertex3f( halfW, b.height, -halfD);
    glVertex3f(-halfW, b.height, -halfD);

    // sisi kiri
    glVertex3f(-halfW, 0, -halfD);
    glVertex3f(-halfW, 0, halfD);
    glVertex3f(-halfW, b.height, halfD);
    glVertex3f(-halfW, b.height, -halfD);

    // sisi kanan
    glVertex3f(halfW, 0, -halfD);
    glVertex3f(halfW, 0, halfD);
    glVertex3f(halfW, b.height, halfD);
    glVertex3f(halfW, b.height, -halfD);
    glEnd();

    glPopMatrix();
}

void drawBuildings() {
    for (const auto& b : buildings) {
        drawBuilding(b);
    }
}


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
    for (const auto& s : supplies) {
        if (s.active) {
            glColor3f(0.2f, 0.6f, 0.8f);
            glPushMatrix();
            glTranslatef(s.x, s.y, s.z);  // Tambahkan ini untuk memindahkan ke posisi supply
            glScalef(1.5f, 2.5f, 1.0f);
            drawCube(1.0f);
            glPopMatrix();
        }
    }

    if (carryingSupply) {
        glPushMatrix();
        glTranslatef(posXBadan, 1.5f, posZBadan);
        glColor3f(1.0f, 1.0f, 0.0f);
        drawCube(0.3f);
        glPopMatrix();
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
        supplies.push_back({ posXBadan, 1.5f, posZBadan, true });
        carryingSupply = false;
        std::cout << "Supply dijatuhkan!\n";
    }
}




// Supply End



void drawPlayer() {
    glPushMatrix();
    glTranslatef(posXBadan, posYBadan, posZBadan);
    glRotatef(rotAngleY, 0, 1, 0);

    // Badan - cube 2x3x1
    glColor3f(0.2f, 0.6f, 0.8f);
    glPushMatrix();
    glScalef(1.5f, 2.5f, 1.0f);
    drawCube(1.0f);
    glPopMatrix();

    // Kepala - sphere di atas badan
    glPushMatrix();
    glTranslatef(0.0f, 2.0f, 0.0f);
    glColor3f(0.9f, 0.7f, 0.5f);
    gluSphere(quadric, 0.7, 20, 20);
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

    // Posisi langit di posisi kamera agar tidak terlihat bergeser
    // Dapatkan posisi kamera (posXBadan, posYBadan, posZBadan) sebagai pusat langit
    glTranslatef(posXBadan, posYBadan, posZBadan);

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, skyTexture);

    // Sphere besar, radius besar agar menutupi seluruh scene
    float radius = 50.0f;

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
    float rotSpeed = 90.0f;    // Kecepatan rotasi (derajat per detik)

    // Rotasi kiri kanan (A/D)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        rotAngleY += rotSpeed * deltaTime;  // Putar ke kiri
        if (rotAngleY >= 360.0f) rotAngleY -= 360.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        rotAngleY -= rotSpeed * deltaTime;  // Putar ke kanan
        if (rotAngleY < 0.0f) rotAngleY += 360.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !keyPressedE) {
        keyPressedE = true;
        pickupSupply(); // <- panggil ini
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        keyPressedE = false;
        dropSupply();
    }

    // Gerak maju mundur (W/S)
    float forward = 0.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) forward += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) forward -= 1.0f;

    if (forward != 0.0f) {
        float angleRad = rotAngleY * PI / 180.0f;
        float deltaX = sin(angleRad) * forward * speed * deltaTime;
        float deltaZ = cos(angleRad) * forward * speed * deltaTime;

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

    // Gambar NPC sebagai bola kecil
    gluSphere(quadric, 1.0, 10, 10);

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
    unsigned char* image = stbi_load("grasscomp.png", &texWidth, &texHeight, &texChannels, 0);
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

    skyTexture = loadTexture("sky2.png");  // Pastikan ada file sky.jpg


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
        drawPlayer();
        drawNpc();
          // Gambar supply yang ada
        renderGameOver();  // Menampilkan teks Game Over jika gameOver == true

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    gluDeleteQuadric(quadric);  // Bersihkan memori
    glfwTerminate();
    return 0;
}
