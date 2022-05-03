
#include "box_collider.h"
#include <math.h>
BoxCollider::BoxCollider(glm::vec3 min_pos, glm::vec3 max_pos)
    : min_pos_(min_pos), max_pos_(max_pos)
{
}

bool BoxCollider::Hit(const Ray &ray, glm::vec3& hit_point)
{
    const glm::vec3 &min = min_pos_;        // aabb包围盒最小点坐标
    const glm::vec3 &max = max_pos_;        // aabb包围盒最大点坐标
    const glm::vec3 &origin = ray.Origin(); //射线起始点
    const glm::vec3 &dir = ray.Direction(); //方向矢量

    float t;

    //分别判断射线与各面的相交情况

    //判断射线与包围盒x轴方向的面是否有交点
    if (dir.x != 0.f) //射线x轴方向分量不为0 若射线方向矢量的x轴分量为0，射线不可能经过包围盒朝x轴方向的两个面
    {
        /*
          使用射线与平面相交的公式求交点
         */
        if (dir.x > 0) //若射线沿x轴正方向偏移
            t = (min.x - origin.x) / dir.x;
        else //射线沿x轴负方向偏移
            t = (max.x - origin.x) / dir.x;

        if (t > 0.f) // t>0时则射线与平面相交
        {
            hit_point = origin + t * dir; //计算交点坐标
            //判断交点是否在当前面内
            if (min.y < hit_point.y && hit_point.y < max.y && min.z < hit_point.z && hit_point.z < max.z)
            {
                return true; //射线与包围盒有交点
            }
        }
    }

    //若射线沿y轴方向有分量 判断是否与包围盒y轴方向有交点
    if (dir.y != 0.f)
    {
        if (dir.y > 0)
            t = (min.y - origin.y) / dir.y;
        else
            t = (max.y - origin.y) / dir.y;

        if (t > 0.f)
        {
            hit_point = origin + t * dir;

            if (min.z < hit_point.z && hit_point.z < max.z && min.x < hit_point.x && hit_point.x < max.x)
            {
                return true;
            }
        }
    }

    //若射线沿z轴方向有分量 判断是否与包围盒y轴方向有交点
    if (dir.z != 0.f)
    {
        if (dir.z > 0)
            t = (min.z - origin.z) / dir.z;
        else
            t = (max.z - origin.z) / dir.z;

        if (t > 0.f)
        {
            hit_point = origin + t * dir;

            if (min.x < hit_point.x && hit_point.x < max.x && min.y < hit_point.y && hit_point.y < max.y)
            {
                return true;
            }
        }
    }

    return false;
}

void BoxCollider::SetAABBPosition(const glm::vec3& min_pos,const glm::vec3& max_pos)
{
    min_pos_ = min_pos;
    max_pos_ = max_pos;
}