#ifndef ZAPPER_H
#define ZAPPER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
#include <globals.h>

class Zapper
{
public:
    unsigned int VBO, EBO, VAO;
    float thiccness = ZapperThiccness;
    float semiLength = ZapperLength / 2;
    float angular_velocity = ZapAngularVelocity;
    float angle;
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 model = glm::mat4(1.0f);

    unsigned int n_triangles;

    Zapper(float theta, float x_)
    {
        angle = theta;
        float length = semiLength;
        pos.x = x_;
        std::vector<unsigned int> indices;
        std::vector<float> vertices;
        float v[] = {
            length , -thiccness, 0.0f,
            length, thiccness, 0.0f,
            -length, -thiccness, 0.0f,
            -length, thiccness, 0.0f 
        };
        vertices.insert(vertices.end(), v, v+sizeof(v)/sizeof(float));
        unsigned int ind[] = {
            0, 1, 2,
            1, 2, 3
        };
        indices.insert(indices.end(), ind, ind+sizeof(ind)/sizeof(unsigned int));
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

    void update()
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
    }
};

#endif