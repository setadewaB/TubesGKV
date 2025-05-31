#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <string>  // Untuk std::string


#define PI 3.14159265358979323846f
const float AREA_MIN_X = -100.0f;
const float AREA_MAX_X =  100.0f;
const float AREA_MIN_Z = -100.0f;
const float AREA_MAX_Z =  100.0f;

enum GameState {
    MAIN_MENU,
    GAMEPLAY,
    GAME_OVER
};

GameState gameState = MAIN_MENU;  // Inisialisasi dengan menu utama
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
     if (gameState == MAIN_MENU) {
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            // Memulai permainan
            gameState = GAMEPLAY;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            // Keluar dari game
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    } else if (gameState == GAME_OVER) {
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            // Restart game
            gameState = GAMEPLAY;
            gameOver = false;
        }
    } else {
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

            // Cek collision batas area
            if (newX >= AREA_MIN_X && newX <= AREA_MAX_X) posXBadan = newX;
            if (newZ >= AREA_MIN_Z && newZ <= AREA_MAX_Z) posZBadan = newZ;
        }

    }

    
}





void renderGround() {
    glDisable(GL_LIGHTING);
    glColor3f(0.0f, 1.0f, 0.0f);  // Hijau

    // Gambar permukaan datar (ground)
    glBegin(GL_QUADS);
    glVertex3f(AREA_MIN_X, 0.0f, AREA_MIN_Z);
    glVertex3f(AREA_MAX_X, 0.0f, AREA_MIN_Z);
    glVertex3f(AREA_MAX_X, 0.0f, AREA_MAX_Z);
    glVertex3f(AREA_MIN_X, 0.0f, AREA_MAX_Z);
    glEnd();

    glEnable(GL_LIGHTING);
}


//MAIN MENU
void renderMainMenu() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Membersihkan layar

    glDisable(GL_DEPTH_TEST);  // Nonaktifkan depth test untuk 2D

    glColor3f(1.0f, 1.0f, 1.0f);  // Warna putih untuk teks
    drawText(-0.3f, 0.3f, "Main Menu", 0.1f);  // Judul menu

    drawText(-0.3f, 0.1f, "Press 'S' to Start Game", 0.05f);  // Opsi mulai game
    drawText(-0.3f, 0.0f, "Press 'Q' to Quit", 0.05f);  // Opsi keluar game

    glEnable(GL_DEPTH_TEST);  // Aktifkan kembali depth test untuk 3D
}


//NPC
float npcX = 20.0f, npcY = 0.0f, npcZ = 20.0f;  // Posisi awal NPC
float npcSpeed = 5.0f;  // Kecepatan pergerakan NPC

float collisionDistance = 1.5f;  // Jarak ketika NPC menyentuh pemain (dalam unit)


void updateNpcPosition(float deltaTime) {
    // Hitung arah dari NPC menuju pemain
    float deltaX = posXBadan - npcX;
    float deltaY = posYBadan - npcY;
    float deltaZ = posZBadan - npcZ;

    // Hitung jarak antara NPC dan pemain di ruang 3D
    float distance = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ);

    // Debugging - Periksa apakah jarak dihitung dengan benar
    printf("NPC to Player Distance: %f\n", distance);

    // Jika jarak antara NPC dan pemain lebih besar dari ambang batas, bergerak menuju pemain
    if (distance > 0.1f) {
        // Normalisasi arah
        float moveX = deltaX / distance;
        float moveY = deltaY / distance;  // Perhitungan gerakan Y
        float moveZ = deltaZ / distance;

        // Update posisi NPC dengan kecepatan perlahan
        npcX += moveX * npcSpeed * deltaTime;
        npcY += moveY * npcSpeed * deltaTime;  // Update posisi Y
        npcZ += moveZ * npcSpeed * deltaTime;
    }

    // Cek apakah NPC menyentuh pemain (Game Over)
    if (distance < collisionDistance) {
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

    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(800, 600, "Simple Player & Camera", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewInit();

    init();

    glfwSetFramebufferSizeCallback(window, reshape);
    reshape(window, 800, 600);

    while (!glfwWindowShouldClose(window)) {
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    processInput(window, (float)deltaTime);  // pass deltaTime ke processInput
    updateCamera();
    updateNpcPosition((float)deltaTime);  // Update posisi NPC dengan deltaTime

    if (gameOver) {
        renderGameOver();  // Render layar merah Game Over
    } else {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Membersihkan layar sebelum menggambar ulang
        processInput(window, (float)deltaTime);  // Hanya proses input jika game belum berakhir
        updateCamera();
        updateNpcPosition((float)deltaTime);  // Update posisi NPC
    }

    renderGround();
    drawPlayer();
    drawNpc(); 

    glfwSwapBuffers(window);
    glfwPollEvents();
    }


    gluDeleteQuadric(quadric);
    glfwTerminate();

    return 0;
}
