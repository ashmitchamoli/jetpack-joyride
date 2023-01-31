#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <bits/stdc++.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>

#include <shader.h>
#include <mesh.h>
#include <zapper.h>
#include <player.h>
#include <coin.h>
#include <globals.h>
#include <bits/stdc++.h>

struct zapperInfo
{
    float angle;
    glm::vec3 pos;
    bool isStatic;
    bool oscillates;
    float amplitude;
    int phase;
    float y0;
};

struct coinInfo
{
    bool toRender;
    glm::vec3 pos;
};

struct Level
{
    int LevelNum;
    int numZaps;
    int numCoins;
    int numSpecialCoins;
    float levelLength;
    float playerSpeedX;
    struct zapperInfo zappers[100];
    struct coinInfo coins[100];
    struct coinInfo spCoins[10];
};

struct Level Levels[3];

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
bool checkCollision(Zapper &zapper);
bool checkCollisionCoin(Coin &coin);
void generateLevel0();
void generateLevel1();
void generateLevel2();

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  2.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float playerHeight = 0.2f;
float playerWidth = 0.1f;
float gravity = -2.0f;
float netUpThrust = 3.0f;
float playerVelocity = 0.0f;
float upLimit = 0.7;
float downLimit = -0.5f;
float playerX = -0.7f;
float playerDisp = playerX;
float playerSpeedX = 0.3f;
float zapperSpawnLimitUp = upLimit - MAX_ZAPPER_LENGTH/2;
float zapperSpawnLimitDown = downLimit + MAX_ZAPPER_LENGTH/2;
int score = 0;
int coinPoints = 1;
int specialCoinPoints = 10;
int currentLevel = 0; 
bool spacePressed = false;
glm::vec3 playerPos = glm::vec3(playerX, 0.0f, 0.0f);

void loadLevel(int, Zapper&);
void generateLevels();

int main(int argc, char** argv)
{
    std::srand((unsigned) std::time(NULL));

    generateLevels();

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // width, height, window title, last 2 are not important
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Jetpack Joyride", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialise GLAD" << std::endl;
        return -1;
    }
    framebuffer_size_callback(window, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    const char* vertexPath = "../src/vertex.shader";
    const char* fragmentPath = "../src/fragment.shader";
    Shader shader = Shader(vertexPath, fragmentPath);
    
    const char* ZvertexPath = "../src/zapvertex.shader";
    const char* ZfragmentPath = "../src/zapfragment.shader";
    Shader Zshader = Shader(ZvertexPath, ZfragmentPath);

    const char* CvertexPath = "../src/coinvertex.shader";
    const char* CfragmentPath = "../src/coinfragment.shader";
    Shader Cshader = Shader(CvertexPath, CfragmentPath);
    
    float pos[] = {
        playerWidth, 0.0f, 0.0f,
        0.0f, playerHeight, 0.0f,
        0.0f, 0.0f, 0.0f,
        playerWidth, playerHeight, 0.0f,
    };

    std::vector<float> Pvertices;
    Pvertices.insert(Pvertices.end(), pos, pos+sizeof(pos)/sizeof(float));
    unsigned int ind[] = {
        0, 1, 2,
        0, 1, 3
    };
    std::vector<unsigned int> Pindices;
    Pindices.insert(Pindices.end(), ind, ind+sizeof(ind)/sizeof(int));
    Player player = Player(Pvertices, Pindices);

    Zapper zapMesh = Zapper(30.0f, 0.35f, 0.3f, 100.0f);
    
    Coin coin = Coin(0.0f, 0.0f);
    
    glEnable(GL_DEPTH_TEST);

    glm::mat4 projection = glm::mat4(1.0f); 
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    
    Zshader.use();
    Zshader.setMat4("projection", projection);
    Zshader.setMat4("view", view);
    
    Cshader.use();
    Cshader.setMat4("projection", projection);
    Cshader.setMat4("view", view);
    
    // render loop
    lastFrame = glfwGetTime();
    while(!glfwWindowShouldClose(window))
    {
        if(playerDisp >= Levels[currentLevel].levelLength + 0.5f)
        {
            loadLevel(++currentLevel, zapMesh);
            playerDisp = playerX;
        }
        playerDisp += playerSpeedX * deltaTime;
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        processInput(window); 

        // rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();

        if(!spacePressed)
            playerVelocity += gravity*deltaTime;
        else
            playerVelocity += netUpThrust*deltaTime;    

        if(playerPos.y <= downLimit)
        {
            playerVelocity = 0.0f;
            playerPos.y = downLimit;
            if(spacePressed) playerVelocity = netUpThrust*deltaTime;
        }
        if((playerPos.y+playerHeight) >= upLimit)
        {
            playerVelocity = gravity*deltaTime;
            playerPos.y = upLimit - playerHeight; 
        }

        glm::mat4 playerMat = glm::mat4(1.0f);
        playerPos += glm::vec3(0.0f, playerVelocity*deltaTime, 0.0f);
        playerMat = glm::translate(playerMat, playerPos);
        shader.setMat4("model", playerMat);
        player.render();

        Zshader.use();

        for(int i = 0; i < Levels[currentLevel].numZaps; i++)
        {
            if(Levels[currentLevel].zappers[i].pos.x + MAX_ZAPPER_LENGTH/2 < -2.0f) continue;
            if(!Levels[currentLevel].zappers[i].isStatic) Levels[currentLevel].zappers[i].angle -= ZapAngularVelocity * deltaTime;
            if(Levels[currentLevel].zappers[i].oscillates)
            {
                Levels[currentLevel].zappers[i].pos.y = Levels[currentLevel].zappers[i].y0 + 
                                                        Levels[currentLevel].zappers[i].amplitude * 
                                                        (glm::sin(100.0f * glm::radians(glfwGetTime() + Levels[currentLevel].zappers[i].phase)));
                // std::cout << Levels[currentLevel].zappers[i].pos.y << " ";
            }
            Levels[currentLevel].zappers[i].pos.x -=  Levels[currentLevel].playerSpeedX * deltaTime;
            zapMesh.angle = Levels[currentLevel].zappers[i].angle;
            zapMesh.pos = Levels[currentLevel].zappers[i].pos;
            zapMesh.update();
            Zshader.setMat4("model", zapMesh.model);
            zapMesh.render();
            // handle collision
            if(checkCollision(zapMesh))
            {
                glfwSetWindowShouldClose(window, true);
            }

        }
        Cshader.use();

        coin.angle++;
        for(int i = 0; i < Levels[currentLevel].numCoins; i++)
        {
            if(Levels[currentLevel].coins[i].pos.x + CoinRadius < -2.0f || !Levels[currentLevel].coins[i].toRender) continue;
            Levels[currentLevel].coins[i].pos.x -= Levels[currentLevel].playerSpeedX * deltaTime;
            coin.pos = Levels[currentLevel].coins[i].pos;
            coin.update();
            Cshader.setMat4("model", coin.model);
            coin.render();
            if(checkCollisionCoin(coin))
            {
                Levels[currentLevel].coins[i].toRender = false;
                score++;
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        spacePressed = true;
    }
    if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        spacePressed = false;
    }
}

bool checkCollision(Zapper &zapper)
{
    glm::vec3 playerBox[4] = {
        playerPos,
        playerPos + glm::vec3(0.0f, playerHeight, 0.0f),
        playerPos + glm::vec3(playerWidth, 0.0f, 0.0f),
        playerPos + glm::vec3(playerWidth, playerHeight, 0.0f),
    };
    float x0 = zapper.pos.x, y0 = zapper.pos.y;
    float cosT = glm::cos(glm::radians(zapper.angle)), sinT = glm::sin(glm::radians(zapper.angle));
    float x1 = x0 + zapper.semiLength * cosT, y1 = y0 + zapper.semiLength * sinT;
    float x2 = x0 - zapper.semiLength * cosT, y2 = y0 - zapper.semiLength * sinT;

    bool collided = false;
    collided = collided | ((playerBox[0].x <= x1 && playerBox[3].x >= x1 && playerBox[0].y <= y1 && playerBox[3].y >= y1) ||
                           (playerBox[0].x <= x2 && playerBox[3].x >= x2 && playerBox[0].y <= y2 && playerBox[3].y >= y2));
    for(auto p: playerBox)
    {
        collided = collided | ((glm::abs<float>(sinT * (p.x - x0) + cosT * (y0 - p.y)) <= zapper.thiccness) && 
                               (glm::abs<float>(cosT * (x1 - p.x) + sinT * (y1 - p.y)) <= 2*zapper.semiLength) &&
                               (glm::abs<float>(cosT * (x2 - p.x) + sinT * (y2 - p.y)) <= 2*zapper.semiLength));
    }

    return collided;
}

bool checkCollisionCoin(Coin &coin)
{
    bool collided = false;
    glm::vec3 playerBox[4] = {
        playerPos,
        playerPos + glm::vec3(0.0f, playerHeight, 0.0f),
        playerPos + glm::vec3(playerWidth, 0.0f, 0.0f),
        playerPos + glm::vec3(playerWidth, playerHeight, 0.0f),
    };

    float collection_range = coin.radius;
    collided = collided | ((coin.pos.x >= playerBox[3].x) && (coin.pos.x - playerBox[3].x <= collection_range) && (coin.pos.y <= playerBox[3].y) && (coin.pos.y >= playerBox[0].y));
    collided = collided | ((coin.pos.y <= playerBox[0].y) && (coin.pos.x <= playerBox[3].x) && (coin.pos.x >= playerBox[0].x) && (playerBox[0].y - coin.pos.y <= collection_range));
    collided = collided | ((coin.pos.y >= playerBox[3].y) && (coin.pos.x <= playerBox[3].x) && (coin.pos.x >= playerBox[0].x) && (coin.pos.y - playerBox[3].y <= collection_range));
    return collided;
}


void loadLevel(int levelNum, Zapper &zap)
{
    playerPos = glm::vec3(playerX, 0.0f, 0.0f);
    playerVelocity = 0;
    playerSpeedX = Levels[levelNum].playerSpeedX;
    currentLevel = levelNum;
    if(levelNum == 2)
    {
        zap.angular_velocity *= 3.0f;
    }
}


void generateLevels()
{
    int numCoins[] = {10, 50, 100};
    int numSpCoins[] = {0, 1, 5};
    int numZappers[] = {15, 25, 50};
    float playerSpeeds[] = {playerSpeedX, playerSpeedX*2, playerSpeedX*3};
    float levelLengths[] = {35.0f * playerSpeeds[0], 50.0f * playerSpeeds[1], 60.0f * playerSpeeds[2]}; // in seconds*playerSpeedX
    for(int i = 0; i < 3; i++)
    {
        Levels[i].LevelNum = i;
        Levels[i].numCoins = numCoins[i];
        Levels[i].numSpecialCoins = numSpCoins[i];
        Levels[i].levelLength = levelLengths[i];
        Levels[i].playerSpeedX = playerSpeeds[i];       
        Levels[i].numZaps = numZappers[i];
    }
    generateLevel0();
    generateLevel1();
    generateLevel2();
}

void generateLevel0()
{
    // all zappers static
    // first numBinary zappers horizontal or vertical
    int numBinary = 8;
    float lz = Levels[0].levelLength/Levels[0].numZaps;
    float lc = Levels[0].levelLength/Levels[0].numCoins;
    for(int i = 0; i < numBinary; i++)
    {
        Levels[0].zappers[i].isStatic = true;
        Levels[0].zappers[i].angle = (std::rand() % 2) * 90.0f;
        Levels[0].zappers[i].pos = glm::vec3(((float) std::rand() / RAND_MAX) * (lz) + i*(lz), ((float)std::rand() / RAND_MAX) * (zapperSpawnLimitUp - zapperSpawnLimitDown) + zapperSpawnLimitDown, 0.0f);
    }

    for(int i = numBinary; i < Levels[0].numZaps; i++)
    {   
        Levels[0].zappers[i].isStatic = true;
        Levels[0].zappers[i].angle = 45.0f + 90.0f * (std::rand() % 2);
        Levels[0].zappers[i].pos = glm::vec3(((float) std::rand() / RAND_MAX) * (lz) + i*(lz), ((float)std::rand() / RAND_MAX) * (zapperSpawnLimitUp - zapperSpawnLimitDown) + zapperSpawnLimitDown, 0.0f);
    }

    for(int i = 0; i < Levels[0].numCoins; i++)
    {
        Levels[0].coins[i].toRender = true;
        Levels[0].coins[i].pos = glm::vec3(((float) std::rand() / RAND_MAX) * (lc) + i*(lc), ((float)std::rand() / RAND_MAX) * (upLimit - downLimit - 2*CoinRadius) + downLimit + CoinRadius, 0.0f); 
    }
}

void generateLevel1()
{
    // some zappers rotate
    // first numStatic zappers are static
    int numStatic = 10;
    float lz = Levels[1].levelLength/Levels[1].numZaps;
    float lc = Levels[1].levelLength/Levels[1].numCoins;
    for(int i = 0; i < numStatic; i++)
    {
        Levels[1].zappers[i].isStatic = true;
        Levels[1].zappers[i].angle = std::rand() % 180;
        Levels[1].zappers[i].pos = glm::vec3(((float) std::rand() / RAND_MAX) * (lz) + i*(lz), ((float)std::rand() / RAND_MAX) * (zapperSpawnLimitUp - zapperSpawnLimitDown) + zapperSpawnLimitDown, 0.0f);
    }

    for(int i = numStatic; i < Levels[1].numZaps; i++)
    {   
        Levels[1].zappers[i].isStatic = false;
        Levels[1].zappers[i].angle = 45.0f + 90.0f * (std::rand() % 2);
        Levels[1].zappers[i].pos = glm::vec3(((float) std::rand() / RAND_MAX) * (lz) + i*(lz), ((float)std::rand() / RAND_MAX) * (zapperSpawnLimitUp - zapperSpawnLimitDown) + zapperSpawnLimitDown, 0.0f);
    }

    for(int i = 0; i < Levels[1].numCoins; i++)
    {
        Levels[1].coins[i].toRender = true;
        Levels[1].coins[i].pos = glm::vec3(((float) std::rand() / RAND_MAX) * (lc) + i*(lc), ((float)std::rand() / RAND_MAX) * (upLimit - downLimit - 2*CoinRadius) + downLimit + CoinRadius, 0.0f); 
    }
}

void generateLevel2()
{
    int numStatic = 5;
    int numOscilating = 5;
    int numRotating =  5;
    float lz = Levels[2].levelLength/Levels[2].numZaps;
    float lc = Levels[2].levelLength/Levels[2].numCoins;
    for(int i = 0; i < Levels[2].numZaps; i++)
    {
        Levels[2].zappers[i].angle = 45.0f * (std::rand() % 4);
        Levels[2].zappers[i].pos = glm::vec3(((float) std::rand() / RAND_MAX) * (lz) + i*(lz), ((float)std::rand() / RAND_MAX) * (zapperSpawnLimitUp - zapperSpawnLimitDown) + zapperSpawnLimitDown, 0.0f);
        if(numStatic) 
        {
            Levels[2].zappers[i].oscillates = false;
            Levels[2].zappers[i].isStatic = true;
            numStatic--;
        }
        else if(numRotating)
        {
            Levels[2].zappers[i].oscillates = false;
            Levels[2].zappers[i].isStatic = false;
            numRotating--;
        } 
        else if(numOscilating)
        {
            Levels[2].zappers[i].isStatic = true;
            Levels[2].zappers[i].oscillates = true;
            Levels[2].zappers[i].amplitude = MAX_ZAPPER_LENGTH/2;
            Levels[2].zappers[i].phase = std::rand() % 90;
            Levels[2].zappers[i].y0 = Levels[2].zappers[i].pos.y;
            numOscilating--;
        }
        else
        {
            Levels[2].zappers[i].isStatic = false;
            Levels[2].zappers[i].oscillates = true;
            Levels[2].zappers[i].amplitude = MAX_ZAPPER_LENGTH/2;
            Levels[2].zappers[i].phase = std::rand() % 90;
            Levels[2].zappers[i].y0 = Levels[2].zappers[i].pos.y;
        }
    }
    for(int i = 0; i < Levels[2].numCoins; i++)
    {
        Levels[2].coins[i].toRender = true;
        Levels[2].coins[i].pos = glm::vec3(((float) std::rand() / RAND_MAX) * (lc) + i*(lc), ((float)std::rand() / RAND_MAX) * (upLimit - downLimit - 2*CoinRadius) + downLimit + CoinRadius, 0.0f); 
    }
}