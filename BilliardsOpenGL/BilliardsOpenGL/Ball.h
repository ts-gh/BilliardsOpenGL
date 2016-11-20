#pragma once

#include <GLUT/GLUT.h>
#include "glm/glm.hpp"
#include "glm/vec3.hpp"

class Ball
{
private:
    int id;
    bool isExist;
    float radius;
    glm::vec3 position;
    glm::vec3 velocity;
public:
    void Initialize(int id, glm::vec3 pos);
    void AddVelocity(glm::vec3 v);
    void Update();
    void CollisionWallCheck();
    void Render();
    void Release();
    glm::vec3 GetPosition();
    void SetPosition(glm::vec3 pos);
    glm::vec3 GetVelocity();
    void SetIsExistFalse();
    bool GetIsExist();
    void SetIsExsist(bool isExist);
    int GetID();
    float GetRadius();
};
