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
    void Initialize(int id, const glm::vec3& pos);
    void AddVelocity(const glm::vec3& v);
    void Update();
    void CollisionWallCheck();
    void Render();
    void SetColor(int id);
    void Release();
    glm::vec3 GetPosition() const;
    void SetPosition(const glm::vec3& pos);
    glm::vec3 GetVelocity() const;
    void SetIsExistFalse();
    bool GetIsExist();
    void SetIsExsist(bool isExist);
    int GetID();
    float GetRadius();
};
