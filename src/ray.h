#ifndef RAY_H
#define RAY_H

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

class Ray
{
private:
    glm::vec3 origin_;
    glm::vec3 direction_;
public:
    Ray(glm::vec3 origin, glm::vec3 direction) : origin_(origin), direction_(direction){}

    glm::vec3 Origin() const { return origin_; }
    glm::vec3 Direction() const { return direction_; }
    glm::vec3 At(float t) const {return (origin_ + t * direction_); }

    static Ray ScreenPointToRay(const glm::mat4& mat, 
    const glm::vec2& screen_size, const glm::vec2& screen_point, const glm::vec3& camera_pos);
};



#endif