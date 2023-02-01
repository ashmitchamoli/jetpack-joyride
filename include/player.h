#ifndef PLAYER_H
#define PLAYER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>

class Player
{
public:
    unsigned int VBO, EBO, VAO;
    unsigned int n_triangles;

    Player(float playerWidth, float playerHeight)
    {
        float pos[] = {
            playerWidth, 0.0f, 0.0f,
            0.0f, playerHeight, 0.0f,
            0.0f, 0.0f, 0.0f,
            playerWidth, playerHeight, 0.0f,
        };

        std::vector<float> vertices;
        vertices.insert(vertices.end(), pos, pos+sizeof(pos)/sizeof(float));
        unsigned int ind[] = {
            0, 1, 2,
            0, 1, 3
        };
        std::vector<unsigned int> indices;
        indices.insert(indices.end(), ind, ind+sizeof(ind)/sizeof(int));

        n_triangles = indices.size()/3;

        // create VAO
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        // create VBO
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // create EBO
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(indices[0]), indices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

    void render()
    {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, n_triangles*3, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
};

#endif