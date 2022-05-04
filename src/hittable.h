#ifndef HITTABLE_H
#define HITTABLE_H

#include "ray.h"

class Hittable
{
private:
    glm::vec3 min_pos_;
    glm::vec3 max_pos_;
public:
    Hittable(glm::vec3 min_pos, glm::vec3 max_pos);
    bool Hit(const Ray& ray, glm::vec3& hit_point);
    void SetAABBPosition(const glm::vec3& min_pos,const glm::vec3& max_pos);
    const glm::vec3& GetMinPos() const;
    const glm::vec3& GetMaxPos() const;
};



#endif