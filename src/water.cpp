#include "water.h"
#include <math.h>
#include <iostream>
#include <time.h>
#include <cstdlib>
#include "hittable.h"
#include "GLFW/glfw3.h"

Water::Water(int size, int surface_size)
    : size_(size), vertices_(nullptr), indices_(nullptr), triangle_count_(0), vertices_count_(0), surface_size_(surface_size),
      cg_mask_(size, std::vector<bool>(size)), cg_p_(size, std::vector<float>(size)), cg_r_(size, std::vector<float>(size)),
      cg_Ap_(size, std::vector<float>(size)), low_h_(size, std::vector<float>(size)),
      Hittable(glm::vec3(-surface_size * 0.5f, 0, -surface_size * 0.5f), glm::vec3(surface_size * 0.5f, 0, surface_size * 0.5f))
{
    vertices_ = new float[size * size * 6];
    indices_ = new unsigned int[(size - 1) * (size - 1) * 6];
    new_h_ = new float[size * size];
    old_h_ = new float[size * size];
    BuildMesh();
}

Water::~Water()
{
    delete (vertices_);
    delete (indices_);
    delete (old_h_);
    delete (new_h_);
}

void Water::BuildMesh()
{

    float dx = surface_size_ / (size_ - 1);
    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            int index = (i * size_ + j) * 6;
            vertices_[index + 0] = i * dx - surface_size_ * 0.5f;
            vertices_[index + 1] = 0.0f;
            vertices_[index + 2] = j * dx - surface_size_ * 0.5f;
            vertices_[index + 3] = 0.0f;
            vertices_[index + 4] = 1.0f;
            vertices_[index + 5] = 0.0f;
            // std::cout<<vertices_[index + 0]<<","<<vertices_[index + 1]<<","<<vertices_[index + 2]<<","<<std::endl;
            vertices_count_++;
        }
    }

    int index = 0;
    for (int i = 0; i < size_ - 1; i++)
    {
        for (int j = 0; j < size_ - 1; j++)
        {
            indices_[index * 6 + 0] = (i + 0) * size_ + (j + 0);
            indices_[index * 6 + 1] = (i + 0) * size_ + (j + 1);
            indices_[index * 6 + 2] = (i + 1) * size_ + (j + 1);
            indices_[index * 6 + 3] = (i + 0) * size_ + (j + 0);
            indices_[index * 6 + 4] = (i + 1) * size_ + (j + 1);
            indices_[index * 6 + 5] = (i + 1) * size_ + (j + 0);
            triangle_count_ += 2;
            // for(int k = 0; k < 6; k++)
            // {
            //     std::cout<<indices_[index * 6 + k]<<",";
            // }
            // std::cout<<std::endl;

            index++;
        }
    }

    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            new_h_[i * size_ + j] = 99999.0f;
            old_h_[i * size_ + j] = 0.0f;
        }
    }
}

float *Water::GetVertices() const
{
    return vertices_;
}

unsigned int *Water::GetIndices() const
{
    return indices_;
}

int Water::GetTriangleCount() const
{
    return triangle_count_;
}

int Water::GetVerticesCount() const
{
    return vertices_count_;
}

float GetX(int i, int j, const std::vector<std::vector<float>>& v)
{
    int size_ = 100;
    int x = i;
    int y = j;

    if (i < 0)
    {
        x = 0;
    }

    if (i > (size_ - 1))
    {
        x = size_ - 1;
    }

    if (j < 0)
    {
        y = 0;
    }

    if (j > (size_ - 1))
    {
        y = size_ - 1;
    }

    return v[x][y];
}
void Water::Update(const Cube& cube)
{
    // UpdateHeight();
    // UpdateNormal();

    //Step 1:
	//TODO: Compute new_h based on the shallow wave model.
    CalNewHeight();

	//Step 2: Block->Water coupling
	//TODO: for block , calculate low_h.
    CalLowHeightAndLabelMask(cube);
	//TODO: then set up b and cg_mask for conjugate gradient.
    std::vector<std::vector<float>> b(size_, std::vector<float>(size_, 0.0f));
    std::vector<std::vector<float>> x(size_, std::vector<float>(size_, 0.0f));
    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            if (cg_mask_[i][j])
            {
                b[i][j] = (new_h_[i * size_ + j] - low_h_[i][j]) / rate_;
            }
            
        }
    }


	//TODO: Solve the Poisson equation to obtain vh (virtual height).
    ConjugateGradient(cg_mask_, b, x, 0, size_, 0, size_);

	//TODO: Diminish vh.

	//TODO: Update new_h by vh.
    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            
            float dv = GetX(i - 1, j, x) + GetX(i + 1, j, x) + GetX(i, j - 1, x) + GetX(i, j + 1, x) - 4.0f * GetX(i, j, x);
            new_h_[i * size_ + j] += gamma_ * rate_ * dv;
        }
    }
    

	//Step 3
	//TODO: old_h <- h; h <- new_h;
    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            old_h_[i * size_ + j] = GetHeight(i, j);
            SetHeight(i, j, new_h_[i * size_ + j]);
        }
    }
    UpdateNormal();
	//Step 4: Water->Block coupling.
	//More TODO here.
}

void Water::CalLowHeightAndLabelMask(const Cube& cube)
{
    const glm::vec3& min_pos = cube.GetMinPos();
    const glm::vec3& max_pos = cube.GetMaxPos();
    float min_x = min_pos.x;
    float max_x = max_pos.x;
    float min_z = min_pos.z;
    float max_z = max_pos.z;
    float dx = surface_size_ / (size_ - 1);
    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            float x = i * dx - surface_size_ * 0.5f;
            float z = j * dx - surface_size_ * 0.5f;
            if(x < min_x || x > max_x || z < min_z || z > max_z)
            {
                cg_mask_[i][j] = false;
            }
            else
            {
                cg_mask_[i][j] = true;
                low_h_[i][j] = cube.GetMinPos().y;
            }
        }
        
    }
    
    
}

void Water::Update(unsigned char *height_map)
{
    //Update();
    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            int index = (i * size_ + j) * 3;
            height_map[index] = GetHeight(i, j) * 1000;
            height_map[index + 1] = GetHeight(i, j) * 1000;
            ;
            height_map[index + 2] = GetHeight(i, j) * 1000;
        }
    }
}

void Water::UpdateVertices()
{
    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            int index = (i * size_ + j) * 6;
            vertices_[index + 1] = std::sin((float)glfwGetTime()) * 0.5f;
            vertices_count_++;
        }
    }
}

void Water::UpdateHeight()
{
    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            float height = GetHeight(i, j);
            new_h_[i * size_ + j] = height + damping_ * (height - old_h_[i * size_ + j]) + rate_ * (GetHeight(i - 1, j) + GetHeight(i + 1, j) + GetHeight(i, j - 1) + GetHeight(i, j + 1) - 4 * height);
        }
    }

    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            old_h_[i * size_ + j] = GetHeight(i, j);
            SetHeight(i, j, new_h_[i * size_ + j]);
        }
    }
}

void Water::CalNewHeight()
{
    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            float height = GetHeight(i, j);
            new_h_[i * size_ + j] = height + damping_ * (height - old_h_[i * size_ + j]) + rate_ * (GetHeight(i - 1, j) + GetHeight(i + 1, j) + GetHeight(i, j - 1) + GetHeight(i, j + 1) - 4 * height);
        }
    }
}

void Water::SetHeight(int i, int j, float height)
{
    int x = i;
    int y = j;

    if (i < 0)
    {
        x = 0;
    }

    if (i > (size_ - 1))
    {
        x = size_ - 1;
    }

    if (j < 0)
    {
        y = 0;
    }

    if (j > (size_ - 1))
    {
        y = size_ - 1;
    }

    int index = (x * size_ + y) * 6;
    vertices_[index + 1] = height;
}

float Water::GetHeight(int i, int j) const
{
    int x = i;
    int y = j;

    if (i < 0)
    {
        x = 0;
    }

    if (i > (size_ - 1))
    {
        x = size_ - 1;
    }

    if (j < 0)
    {
        y = 0;
    }

    if (j > (size_ - 1))
    {
        y = size_ - 1;
    }

    int index = (x * size_ + y) * 6;
    return vertices_[index + 1];
}

void Water::CreateRandomRipples(float height)
{
    int i = rand() % size_;
    int j = rand() % size_;
    float r = height;
    std::cout << "Create Random Ripples" << std::endl;
    float dh = r / 8;
    SetHeight(i, j, GetHeight(i, j) + dh * 8);
    SetHeight(i - 1, j, GetHeight(i - 1, j) - dh);
    SetHeight(i + 1, j, GetHeight(i + 1, j) - dh);
    SetHeight(i, j - 1, GetHeight(i, j - 1) - dh);
    SetHeight(i, j + 1, GetHeight(i, j + 1) - dh);
    SetHeight(i - 1, j - 1, GetHeight(i - 1, j - 1) - dh);
    SetHeight(i + 1, j - 1, GetHeight(i + 1, j - 1) - dh);
    SetHeight(i + 1, j + 1, GetHeight(i + 1, j + 1) - dh);
    SetHeight(i - 1, j + 1, GetHeight(i - 1, j + 1) - dh);
}

glm::vec3 Water::GetVertex(int i, int j) const
{
    int x = i;
    int y = j;

    if (i < 0)
    {
        x = 0;
    }
    if (i > (size_ - 1))
    {
        x = size_ - 1;
    }
    if (j < 0)
    {
        y = 0;
    }
    if (j > (size_ - 1))
    {
        y = size_ - 1;
    }

    int index = (x * size_ + y) * 6;
    return glm::vec3(vertices_[index], vertices_[index + 1], vertices_[index + 2]);
}

std::vector<Triangle> Water::GetAdjacentTriangles(int i, int j) const
{
    int gap = 128;
    std::vector<Triangle> triangles;
    triangles.push_back({GetVertex(i, j), GetVertex(i, j - 1), GetVertex(i - 1, j - 1)});
    triangles.push_back({GetVertex(i, j), GetVertex(i - 1, j - 1), GetVertex(i - 1, j)});
    triangles.push_back({GetVertex(i, j), GetVertex(i - 1, j), GetVertex(i, j + 1)});
    triangles.push_back({GetVertex(i, j), GetVertex(i, j + 1), GetVertex(i + 1, j + 1)});
    triangles.push_back({GetVertex(i, j), GetVertex(i + 1, j + 1), GetVertex(i + 1, j)});
    triangles.push_back({GetVertex(i, j), GetVertex(i + 1, j), GetVertex(i, j - 1)});
    return triangles;
}

glm::vec3 Water::CalculateTriangleNormal(Triangle T) const
{
    glm::vec3 N = glm::cross(T.b - T.a, T.c - T.a);
    return N;
}

void Water::SetNormal(int i, int j, glm::vec3 normal)
{
    int index = (i * size_ + j) * 6;
    vertices_[index + 3] = normal.x;
    vertices_[index + 4] = normal.y;
    vertices_[index + 5] = normal.z;
}

void Water::UpdateNormal()
{
    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++)
        {
            if (i > 0 && j > 0 && i < (size_ - 1) && j < (size_ - 1))
            {
                std::vector<Triangle> triangles = GetAdjacentTriangles(i, j);
                glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);
                for (int k = 0; k < 6; k++)
                {
                    normal += CalculateTriangleNormal(triangles[k]);
                    // std::cout << normal.x << " "<<normal.y << " "<<normal.z<<std::endl;
                }
                SetNormal(i, j, glm::normalize(normal));
            }
            else
            {
                SetNormal(i, j, glm::vec3(0, 1.0f, 0.0f));
            }
        }
    }
}

void Water::CreateRipples(const glm::vec3 &world_pos, float height)
{
    float dx = surface_size_ / (size_ - 1);
    int i = (world_pos.x + surface_size_ * 0.5f) / dx + 0.5;
    int j = (world_pos.z + surface_size_ * 0.5f) / dx + 0.5;
    float r = height;
    std::cout << "Create Random Ripples" << std::endl;
    float dh = r / 8;
    SetHeight(i, j, GetHeight(i, j) + dh * 8);
    SetHeight(i - 1, j, GetHeight(i - 1, j) - dh);
    SetHeight(i + 1, j, GetHeight(i + 1, j) - dh);
    SetHeight(i, j - 1, GetHeight(i, j - 1) - dh);
    SetHeight(i, j + 1, GetHeight(i, j + 1) - dh);
    SetHeight(i - 1, j - 1, GetHeight(i - 1, j - 1) - dh);
    SetHeight(i + 1, j - 1, GetHeight(i + 1, j - 1) - dh);
    SetHeight(i + 1, j + 1, GetHeight(i + 1, j + 1) - dh);
    SetHeight(i - 1, j + 1, GetHeight(i - 1, j + 1) - dh);
}

void Water::ATimes(const std::vector<std::vector<bool>> &mask,
                          const std::vector<std::vector<float>> &x, std::vector<std::vector<float>> &Ax,
                          int li, int ui, int lj, int uj)
{

    for (int i = li; i <= ui; i++)
    {
        for (int j = lj; j <= uj; j++)
        {
            if (i >= 0 && j >= 0 && i < size_ && j < size_ && mask[i][j])
            {
                Ax[i][j] = 0;
                if (i != 0)
                    Ax[i][j] -= x[i - 1][j] - x[i][j];
                if (i != size_ - 1)
                    Ax[i][j] -= x[i + 1][j] - x[i][j];
                if (j != 0)
                    Ax[i][j] -= x[i][j - 1] - x[i][j];
                if (j != size_ - 1)
                    Ax[i][j] -= x[i][j + 1] - x[i][j];
            }
        }
    }
}

void Water::ConjugateGradient(std::vector<std::vector<bool>> &mask, std::vector<std::vector<float>> &b,
                                     std::vector<std::vector<float>> &x, int li, int ui, int lj, int uj)
{
    // Solve the Laplacian problem by CG.
    ATimes(mask, x, cg_r_, li, ui, lj, uj);

    for (int i = li; i <= ui; i++)
    {
        for (int j = lj; j <= uj; j++)
        {
            if (i >= 0 && j >= 0 && i < size_ && j < size_ && mask[i][j])
            {
                cg_r_[i][j] = b[i][j] - cg_r_[i][j];
                cg_p_[i][j] = cg_r_[i][j];
            }

        }

    }


    float rk_norm = Dot(mask, cg_r_, cg_r_, li, ui, lj, uj);

    for (int k = 0; k < 128; k++)
    {
        if (rk_norm < 1e-10f)
            break;
        ATimes(mask, cg_p_, cg_Ap_, li, ui, lj, uj);
        float alpha = rk_norm / Dot(mask, cg_p_, cg_Ap_, li, ui, lj, uj);

        for (int i = li; i <= ui; i++)
            for (int j = lj; j <= uj; j++)
                if (i >= 0 && j >= 0 && i < size_ && j < size_ && mask[i][j])
                {
                    x[i][j] += alpha * cg_p_[i][j];
                    cg_r_[i][j] -= alpha * cg_Ap_[i][j];
                }

        float _rk_norm = Dot(mask, cg_r_, cg_r_, li, ui, lj, uj);
        float beta = _rk_norm / rk_norm;
        rk_norm = _rk_norm;

        for (int i = li; i <= ui; i++)
            for (int j = lj; j <= uj; j++)
                if (i >= 0 && j >= 0 && i < size_ && j < size_ && mask[i][j])
                {
                    cg_p_[i][j] = cg_r_[i][j] + beta * cg_p_[i][j];
                }
    }
}

float Water::Dot(std::vector<std::vector<bool>> &mask,
                        std::vector<std::vector<float>> &x, std::vector<std::vector<float>> &y,
                        int li, int ui, int lj, int uj)
{
    float ret = 0;
    for (int i = li; i <= ui; i++)
        for (int j = lj; j <= uj; j++)
            if (i >= 0 && j >= 0 && i < size_ && j < size_ && mask[i][j])
            {
                ret += x[i][j] * y[i][j];
            }
    return ret;
}
