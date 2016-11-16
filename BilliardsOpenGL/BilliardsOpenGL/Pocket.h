#pragma onece

#include <GLUT/GLUT.h>
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include "Ball.h"

class Pocket
{
private:
    int id;
    float radius;
    glm::vec3 position;
public:
    void Initialize(int _id, glm::vec3 _position);
    void Render();
    void FallPocketCheck(Ball *ball);
    void Release();
};
