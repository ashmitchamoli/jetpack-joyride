#ifndef BG_H
#define BG_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <bits/stdc++.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <shader.h>

class Background
{
public:
    int width, height, nrChannels;
    unsigned int bgTex;
    unsigned int bgVBO, bgVAO, bgEBO;
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);

    Background(Shader& bgShader)
    {
        float bgVertices[] = {
            2.0f, 1.125f,   -0.1,   1.0f, 1.0f,
            2.0f, -1.125f,  -0.1,   1.0f, 0.0f,
            -2.0f, -1.125f, -0.1,   0.0f, 0.0f,
            -2.0f, 1.125f,  -0.1,   0.0f, 1.0f
        };
        unsigned int bgIndices[] = {
            0, 1, 3,
            1, 2, 3
        };

        glGenVertexArrays(1, &bgVAO);
        glGenBuffers(1, &bgVBO);
        glGenBuffers(1, &bgEBO);
        glBindVertexArray(bgVAO);

        glBindBuffer(GL_ARRAY_BUFFER, bgVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(bgVertices), bgVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bgEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(bgIndices), bgIndices, GL_STATIC_DRAW);

        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        
        // texCoords
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // create texture
        glGenTextures(1, &bgTex);
        glBindTexture(GL_TEXTURE_2D, bgTex);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load("../PineForestParallax/MorningLayer1.png", &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);
        
        bgShader.use();
        bgShader.setInt("outTexture", 0);
    }

    void applyTex(Shader& bgShader)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, bgTex);
        bgShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        bgShader.setMat4("model", model);
        glBindVertexArray(bgVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};

#endif