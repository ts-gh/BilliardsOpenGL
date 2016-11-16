#include "Collision.h"
#include <math.h>

float _radius = 5.0;
float _ballBounce = 0.9;

void CollisionBallCheck(Ball *ball)
{
    for(int i=0; i<10; i++){
        for(int j=0; j<10; j++){
            
            if(j<=i){
                continue;
            }
            
            if(!ball[i].GetIsExist() || !ball[j].GetIsExist()){
                continue;
            }
            
            //２つのボールのの中心間距離
            float tr = 5.f + 5.f;
            
            //必要なボールの情報の取得
            glm::vec3 a_pos = ball[i].GetPosition();
            glm::vec3 b_pos = ball[j].GetPosition();
            glm::vec3 a_vel = ball[i].GetVelocity();
            glm::vec3 b_vel = ball[j].GetVelocity();
            
            //ABベクトルを算出
            float abx_vec = b_pos.x - a_pos.x;
            float abz_vec = b_pos.z - a_pos.z;
            
            //ボール同士の衝突判定
            if(tr * tr > (abx_vec * abx_vec) + (abz_vec * abz_vec)){
                
                //めり込んだ分を戻す
                float len = sqrt(abx_vec * abx_vec + abz_vec * abz_vec);
                float distance = (5.f + 5.f) - len;
                if(len > 0){
                    len = 1/ len;
                }
                abx_vec *= len;
                abz_vec *= len;
                
                distance /= 2.0;
                a_pos -= glm::vec3(abx_vec * distance, 0.f, abz_vec * distance);
                b_pos += glm::vec3(abx_vec * distance, 0.f, abz_vec * distance);
                ball[i].SetPosition(a_pos);
                ball[j].SetPosition(b_pos);
                
                //衝突後の速度計算
                float s1 = a_vel.x * abx_vec + a_vel.z * abz_vec;
                float s2 = b_vel.x * abx_vec + b_vel.z * abz_vec;
                
                float vn1_x = abx_vec * s1;
                float vn1_z = abz_vec * s1;
                
                float vn2_x = abx_vec * s2;
                float vn2_z = abz_vec * s2;
                
                float vt1_x = a_vel.x - vn1_x;
                float vt1_z = a_vel.z - vn1_z;
                
                float vt2_x = b_vel.x - vn2_x;
                float vt2_z = b_vel.z - vn2_z;
                
                float new_s1 = (s1 + s2 - 0.9*(s1-s2))/(1+1);
                float new_s2 = (s1 + s2 + 0.9*(s1-s2))/(1+1);
                
                float new_vn1_x = abx_vec * new_s1;
                float new_vn1_z = abz_vec * new_s1;
                
                float new_vn2_x = abx_vec * new_s2;
                float new_vn2_z = abz_vec * new_s2;
                
                ball[i].AddVelocity(glm::vec3(new_vn1_x + vt1_x, 0.f, new_vn1_z + vt1_z));
                ball[j].AddVelocity(glm::vec3(new_vn2_x + vt2_x, 0.f, new_vn2_z + vt2_z));
                
            }
        }
    }
}
