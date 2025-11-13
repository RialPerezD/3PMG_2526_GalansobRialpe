#include "MotArda/Components/Transform.hpp"

MTRD::Transform::Transform(){
    position = { 0,0,0 };
    scale = { 0,0,0 };
    rotation = { 0,0,0 };
}


MTRD::Transform::Transform(const glm::vec3& pos, const glm::vec3& scl, const glm::vec3& rot)
    : position(pos), scale(scl), rotation(rot){
}
