#ifndef COIN_H
#define COIN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
#include <globals.h>

class Coin
{
public:
    unsigned int VBO, EBO, VAO;
    float radius = 0.03f;
    float thiccness = CoinThiccness;
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 model = glm::mat4(1.0f);
    bool toRender = true;
    float angle = 0.0f;
    unsigned int n_triangles;

    Coin(float x, float y)
    {
        pos.x = x;
        pos.y = y;
        std::vector<unsigned int> indices;
        std::vector<float> vertices;

        generatePrism(vertices, indices);

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
        if(angle > 360.0f)
        {
            angle -= 360.0f;
        }
        model = glm::mat4(1.0f);
        model = glm::translate(model, pos);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    }

private:
    void generatePolygon(int n, float zOffset, std::vector<float>& vertices, std::vector<unsigned int>& indices, int index_offset)
    {
        vertices[3*n] = vertices[3*n+1] = 0.0f;
        vertices[3*n+2] = zOffset;
        float theta = 90.0f, delta = (360.0f)/n;
        for(int i = 0; i < n; i++, theta+=delta)
        {
            vertices[3*i]   = radius*cos(glm::radians(theta));
            vertices[3*i+1] = radius*sin(glm::radians(theta));
            vertices[3*i+2] = zOffset;
        }
        for(int i = 0; i < n; i++)
        {
            indices[3*i]    = i + index_offset;
            indices[3*i+1]  = (i+1)%n + index_offset;
            indices[3*i+2]  = n + index_offset;
        }
    }

    void generatePrism(std::vector<float>& vertices, std::vector<unsigned int>& indices)
    {
        int n = 100;
        std::vector<float> tempV(3*(n+1));
        std::vector<unsigned int> tempI(3*n);
        generatePolygon(n, thiccness/2, tempV, tempI, 0);
        for(float v: tempV)
            vertices.push_back(v);
        for(int i: tempI)
            indices.push_back(i);
        
        generatePolygon(n, -thiccness/2, tempV, tempI, n+1);
        for(float v: tempV)
            vertices.push_back(v);
        for(int i: tempI)
            indices.push_back(i);
        
        for(int i = 0; i < n; i++)
        {
            indices.push_back(i);   indices.push_back((i+1)%n);   indices.push_back((i+1)%n + n+1); 
            indices.push_back(i);   indices.push_back(i+n+1);     indices.push_back((i+1)%n + n+1);
        }
    }


};

#endif