#include "water_surface.h"
#include <math.h>
#include <iostream>
#include <time.h>
#include <cstdlib>
#include "box_collider.h"
#include "GLFW/glfw3.h"

WaterSurface::WaterSurface(int size, int surface_size)
:size_(size), vertices_(nullptr), indices_(nullptr), triangle_count_(0), vertices_count_(0), surface_size_(surface_size),
BoxCollider(glm::vec3(- surface_size * 0.5f, 0, - surface_size * 0.5f), glm::vec3(surface_size * 0.5f, 0, surface_size * 0.5f))
{
    vertices_   = new float[size * size * 6];
    indices_    = new unsigned int[(size - 1) * (size - 1) * 6];
    new_h_      = new float[size * size];
    old_h_      = new float[size * size];
    BuildMesh();
}

WaterSurface::~WaterSurface()
{
    delete(vertices_);
    delete(indices_);
    delete(old_h_);
    delete(new_h_);
}

void WaterSurface::BuildMesh()
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
            //std::cout<<vertices_[index + 0]<<","<<vertices_[index + 1]<<","<<vertices_[index + 2]<<","<<std::endl;
            vertices_count_ ++;
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

float* WaterSurface::GetVertices() const
{
    return vertices_;
}

unsigned int*  WaterSurface::GetIndices() const
{
    return indices_;
}

int WaterSurface::GetTriangleCount() const
{
    return triangle_count_;
}

int WaterSurface::GetVerticesCount() const
{
    return vertices_count_;
}

void WaterSurface::Update()
{
    UpdateHeight();
    UpdateNormal();
}

void WaterSurface::Update(unsigned char* height_map)
{
    Update();
    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++) 
		{
            int index = (i * size_ + j) * 3;
            height_map[index] = GetHeight(i, j) * 1000;
            height_map[index + 1] = GetHeight(i, j) * 1000;;
            height_map[index + 2] = GetHeight(i, j) * 1000;
		}
    }
}

void WaterSurface::UpdateVertices()
{
    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++) 
		{
            int index = (i * size_ + j) * 6;
            vertices_[index + 1] = std::sin((float)glfwGetTime()) * 0.5f;
            vertices_count_ ++;
		}
    }
}

void WaterSurface::UpdateHeight()
{
    for (int i = 0; i < size_; i++)
    {
        for (int j = 0; j < size_; j++) 
		{
            float height = GetHeight(i, j);
            new_h_[i * size_ + j] = height + damping_ * (height - old_h_[i * size_ + j]) 
            + rate_ * (GetHeight(i - 1, j) + GetHeight(i + 1, j) + GetHeight(i, j - 1) + GetHeight(i, j + 1) - 4 * height);
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

void WaterSurface::SetHeight(int i, int j, float height)
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

float WaterSurface::GetHeight(int i, int j) const
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

void WaterSurface::CreateRandomRipples(float height)
{
    int i = rand() % size_;
    int j = rand() % size_;
    float r = height;
    std::cout<<"Create Random Ripples"<<std::endl;
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

glm::vec3 WaterSurface::GetVertex(int i, int j) const
{
    int x = i;
    int y = j;

    if (i < 0){x = 0;}
    if (i > (size_ - 1)){x = size_ - 1;}
    if (j < 0){y = 0;}
    if (j > (size_ - 1)){y = size_ - 1;}
    
    int index = (x * size_ + y) * 6;
    return glm::vec3(vertices_[index], vertices_[index + 1], vertices_[index + 2]);
}

std::vector<Triangle> WaterSurface::GetAdjacentTriangles(int i, int j) const
{
    int gap = 128;
    std::vector<Triangle> triangles;
    triangles.push_back({
        GetVertex(i, j), GetVertex(i, j - 1), GetVertex(i - 1, j - 1)
    });
    triangles.push_back({
        GetVertex(i, j), GetVertex(i - 1, j - 1), GetVertex(i - 1, j)
    });
    triangles.push_back({
        GetVertex(i, j), GetVertex(i - 1, j), GetVertex(i, j + 1)
    });
    triangles.push_back({
        GetVertex(i, j), GetVertex(i, j + 1), GetVertex(i + 1, j + 1)
    });
    triangles.push_back({
        GetVertex(i, j), GetVertex(i + 1, j + 1), GetVertex(i + 1, j)
    });
    triangles.push_back({
        GetVertex(i, j), GetVertex(i + 1, j), GetVertex(i, j - 1)
    });    
    return triangles;
}

glm::vec3 WaterSurface::CalculateTriangleNormal(Triangle T) const
{
    glm::vec3 N = glm::cross(T.b - T.a, T.c - T.a);
    return N;
}

void WaterSurface::SetNormal(int i, int j, glm::vec3 normal)
{
    int index = (i * size_ + j) * 6;
    vertices_[index + 3] = normal.x;
    vertices_[index + 4] = normal.y;
    vertices_[index + 5] = normal.z;
}

void WaterSurface::UpdateNormal()
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
                    //std::cout << normal.x << " "<<normal.y << " "<<normal.z<<std::endl;
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

void WaterSurface::CreateRipples(const glm::vec3& world_pos, float height)
{
    float dx = surface_size_ / (size_ - 1);
    int i = (world_pos.x + surface_size_ * 0.5f) / dx + 0.5;
    int j = (world_pos.z + surface_size_ * 0.5f) / dx + 0.5;
    float r = height;
    std::cout<<"Create Random Ripples"<<std::endl;
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