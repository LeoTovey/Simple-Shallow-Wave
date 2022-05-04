#include "ray.h"
#include <iostream>


Ray Ray::ScreenPointToRay(const glm::mat4& mat, const glm::vec2& screen_size, const glm::vec2& screen_point, const glm::vec3& camera_pos)
{
    glm::vec4 screen_ray(0, 0, 1.0f, 1.0f);
    screen_ray.x = screen_point.x / screen_size.x * 2.0f - 1.0f;
    screen_ray.y = screen_point.y / screen_size.y * 2.0f - 1.0f;

    glm::vec4 world_ray = glm::inverse(mat) * screen_ray;
    return Ray(camera_pos, glm::normalize(world_ray));
}