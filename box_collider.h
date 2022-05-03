#ifndef BOX_COLLIDER_H
#define BOX_COLLIDER_H

#include "ray.h"
class Collider
{
public:
    virtual bool Hit(const Ray& ray, glm::vec3& hit_point) = 0;
};

class BoxCollider : public Collider
{
private:
    glm::vec3 min_pos_;
    glm::vec3 max_pos_;
public:
    BoxCollider(glm::vec3 min_pos, glm::vec3 max_pos);
    bool Hit(const Ray& ray, glm::vec3& hit_point);
    void SetAABBPosition(const glm::vec3& min_pos,const glm::vec3& max_pos);
};



#endif