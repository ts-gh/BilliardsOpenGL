#include "Ball.h"

float wallBounce = 0.7;    //壁とボールの反発係数
float poolTable_width = 160;
float poolTable_height = 290;


void Ball::Initialize(int _id, glm::vec3 pos)
{
    id = _id;
    isExist = true; //ボールがpoolに存在するか　ー＞　ポケットに落ちたらfalse
    radius = 5.0;
    position = pos;
    velocity = glm::vec3(0.f);
}

void Ball::AddVelocity(glm::vec3 v)
{
    velocity = v;
}

void Ball::Update()
{
    if(isExist){
        position += velocity;
        
        //速度減衰　ー＞　摩擦、空気抵抗、跳ね返り係数・・・　deltaTimeを使う
        velocity *= 0.98f;
        
        //速度が一定以下になれば止める（速度0に）
        if(glm::length(velocity) < 0.05){
            velocity = glm::vec3(0.f);
        }
    }
}

void Ball::CollisionWallCheck(){
    //左の壁（z軸上にある）との衝突判定
    if(position.x < radius + (poolTable_width/2) * -1){
        position.x = radius + (poolTable_width/2) * -1;
        velocity.x *= wallBounce * -1;
        velocity.z *= wallBounce;
    }
    //右の壁
    if(position.x > -1 * radius + (poolTable_width/2)){
        position.x = -1 * radius + poolTable_width/2;
        velocity.x *= wallBounce * -1;
        velocity.z *= wallBounce;
    }
    //上の壁
    if(position.z < radius + (poolTable_height/2) * -1){
        position.z = radius + (poolTable_height/2) * -1;
        velocity.x *= wallBounce;
        velocity.z *= wallBounce * -1;
    }
    //下の壁
    if(position.z > -1 * radius + (poolTable_height/2)){
        position.z = -1 * radius + poolTable_height/2;
        velocity.x *= wallBounce;
        velocity.z *= wallBounce * -1;
    }
}

void Ball::Render()
{
    //push
    glPushMatrix();
    
    //IDにあったカラーのセット
    if(id == 0){
        glColor3d(1.0, 1.0, 1.0);
    }
    if(id == 1){
        glColor3d(1.0, 1.0, 0.0);
    }
    if(id == 2){
        glColor3d(0.0, 0.0, 1.0);
    }
    if(id == 3){
        glColor3d(1.0, 0.0, 0.0);
    }
    if(id == 4){
        glColor3d(1.0, 192.f/255.f, 203.f/255.f);
    }
    if(id == 5){
        glColor3d(1.0, 165.f/255.f, 0.0);
    }
    if(id == 6){
        glColor3d(0.0, 1.0, 0.0);
    }
    if(id == 7){
        glColor3d(165.f/255.f, 42.f/255.f, 42.f/255.f);
    }
    if(id == 8){
        glColor3d(0.0, 0.0, 0.0);
    }
    if(id == 9){
        glColor3d(1.0, 1.0, 0.5);
    }
    
    //移動
    glTranslatef(position.x, position.y, position.z);
    
    //球の描画
    glutSolidSphere(radius, 16, 16);
    
    //pop
    glPopMatrix();
    
}

void Ball::Release()
{
    //未実装
}

glm::vec3 Ball::GetPosition()
{
    return position;
}

void Ball::SetPosition(glm::vec3 pos)
{
    position = pos;
}

glm::vec3 Ball::GetVelocity()
{
    return velocity;
}

bool Ball::GetIsExist()
{
    return isExist;
}

void Ball::SetIsExsist(bool _isExsist)
{
    isExist = _isExsist;
}

int Ball::GetID()
{
    return id;
}

float Ball::GetRadius()
{
    return radius;
}
