#ifndef BG_H
#define BG_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <bits/stdc++.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int width, height, nrChannels;
unsigned char *data = stbi_load("../PineForestParallax/MorningLayer1.png", &width, &height, &nrChannels, 0);

unsigned int bgTex;

float vertices[] = {
    2.0f, 1.125f, 0.0f,     1.0f, 1.0f,
    2.0f, -1.125f, 0.0f,    1.0f, 0.0f,
    -2.0f, -1.125f, 0.0f,   0.0f, 0.0f,
    -2.0f, 1.125f, 0.0f,    0.0f, 1.0f
};

void initTexture()
{
    glGenTextures(1, &bgTex);
    glBindTexture(GL_TEXTURE_2D, bgTex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    stbi_image_free(data);
}

void applyTex()
{

}

#endif