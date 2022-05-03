#ifndef CUBE_H
#define CUBE_H

#include "box_collider.h"

class Cube : public BoxCollider
{
private:
    glm::vec3 position_;
    glm::vec3 size_;
    float vertices_[216];
    int triangle_count_ = 12;
    int vertices_count_ = 36;

    void BuildMesh();
public:
    Cube(glm::vec3 position, glm::vec3 size);
    ~Cube();

    void SetPosition(const glm::vec3& position);
    glm::vec3 GetPosition() const;
    float* GetVertices();
    int GetTriangleCount() const;
    int GetVerticesCount() const;
};



#endif