#include "Pocket.h"

void Pocket::Initialize(int _id, glm::vec3 _position)
{
    id = _id;
    radius = 6.f;
    position = _position;
}

void Pocket::Render()
{
    //push
    glPushMatrix();
    
    //色の設定
    glColor3d(128.f/255.f, 128.f/255.f, 128.f/255.f);
    
    //移動
    glTranslatef(position.x, position.y, position.z);
    
    //球の描画
    glutSolidSphere(radius, 16, 16);
    
    //pop
    glPopMatrix();
}

void Pocket::FoulsCheck()
{
    //未実装（現状ここではやらない）
}

void Pocket::Release()
{
    
}

glm::vec3 Pocket::GetPosition()
{
    return position;
}

float Pocket::GetRadius()
{
    return radius;
}
