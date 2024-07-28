#pragma once

#include "LIB_vectors.h"
#include "LIB_matrices.h"
#include "LIB_colours.h"

struct Vertex {
    Vector3<float> Position;
    Vector3<float> Normal;
    Vector2<float> TexCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
};

//class Mesh {
//public:
//    std::vector<Vertex>       vertices;
//    std::vector<unsigned int> indices;
//    std::vector<Texture>      textures;
//
//    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
//    //void Draw(Shader& shader);
//private:
//    unsigned int VAO, VBO, EBO;
//
//    void setupMesh();
//};
