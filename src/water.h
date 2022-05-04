#ifndef WATER_SURFACE_H
#define WATER_SURFACE_H

#include "glm.hpp"
#include "hittable.h"
#include "cube.h"
#include <vector>

struct Triangle
{
    glm::vec3 a, b, c;
};

class Water : public Hittable
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

    // PCG Solver
    std::vector<std::vector<bool>> cg_mask_;
    std::vector<std::vector<float>> cg_p_;
    std::vector<std::vector<float>> cg_r_;
    std::vector<std::vector<float>> cg_Ap_;
    float gamma_ = 0.01f;
    void ATimes(const std::vector<std::vector<bool>> &mask,
                const std::vector<std::vector<float>> &x, std::vector<std::vector<float>> &Ax,
                int li, int ui, int lj, int uj);
    void ConjugateGradient(std::vector<std::vector<bool>> &mask, std::vector<std::vector<float>> &b,
                           std::vector<std::vector<float>> &x, int li, int ui, int lj, int uj);
    float Dot(std::vector<std::vector<bool>> &mask,
              std::vector<std::vector<float>> &x, std::vector<std::vector<float>> &y,
              int li, int ui, int lj, int uj);

    // cube - water coupling
    std::vector<std::vector<float>> low_h_;
    void CalNewHeight();
    void CalLowHeightAndLabelMask(const Cube& cube);

public:
    Water(int size, int surface_size);
    ~Water();

    float *GetVertices() const;
    unsigned int *GetIndices() const;
    int GetTriangleCount() const;
    int GetVerticesCount() const;
    void Update(const Cube& cube);
    void Update(unsigned char *height_map);
    void CreateRandomRipples(float height);
    void CreateRipples(const glm::vec3 &world_pos, float height);
};

#endif