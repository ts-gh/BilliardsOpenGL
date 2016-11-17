#include "Pocket.h"

void Pocket::Initialize(int _id, glm::vec3 _position)
{
    id = _id;
    radius = 6.f;
    position = _position;
}

void Pocket::Render()
{
    // push
    glPushMatrix();
    
    // 行列のリセット
    //glLoadIdentity();
    
    // 色の設定
    glColor3d(128.f/255.f, 128.f/255.f, 128.f/255.f);
    
    // 移動
    glTranslatef(position.x, position.y, position.z);
    
    // 球の描画
    glutSolidSphere(radius, 16, 16);
    
    // pop
    glPopMatrix();
}

void Pocket::FallPocketCheck(Ball *ball, int targetBall)
{
    for(int i=0; i<9; i++){
        if(!ball[i].GetIsExist()){
            continue;
        }
        
        glm::vec3 a_pos = position;
        glm::vec3 b_pos = ball[i].GetPosition();
        
        float abx_v = b_pos.x - a_pos.x;
        float abz_v = b_pos.z - a_pos.z;
        
        float tr = radius + ball[i].GetRadius();
        
        if(tr * tr > (abx_v * abx_v) + (abz_v * abz_v)){
            ball[i].SetIsExsist(false);
        }
    }
}

void Pocket::FoulsCheck()
{

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
