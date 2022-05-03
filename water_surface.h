#ifndef WATER_SURFACE_H
#define WATER_SURFACE_H

#include "glm/glm.hpp"
#include "box_collider.h"
#include <vector>

struct Triangle
{
    glm::vec3 a, b, c;
};

class WaterSurface : public BoxCollider
{
private:
    int size_;
    float *vertices_;
    unsigned int *indices_;
    int triangle_count_;
    int vertices_count_;

    float damping_ = 0.996f;
    float rate_ = 0.005f;
    float surface_size_;

    float *old_h_, *new_h_;

    void BuildMesh();
    void UpdateVertices();
    void UpdateHeight();

    void SetHeight(int i, int j, float height);
    float GetHeight(int i, int j) const;


    // recalculate normal
    glm::vec3 GetVertex(int i, int j) const;
    std::vector<Triangle> GetAdjacentTriangles(int i, int j) const;
    glm::vec3 CalculateTriangleNormal(Triangle T) const;
    void SetNormal(int i, int j, glm::vec3 normal);
    void UpdateNormal();

public:
    WaterSurface(int size, int surface_size);
    ~WaterSurface();

    float* GetVertices() const;
    unsigned int*   GetIndices() const;
    int GetTriangleCount() const;
    int GetVerticesCount() const;
    void Update();
    void Update(unsigned char* height_map);
    void CreateRandomRipples(float height);
    void CreateRipples(const glm::vec3& world_pos, float height);

};



#endif