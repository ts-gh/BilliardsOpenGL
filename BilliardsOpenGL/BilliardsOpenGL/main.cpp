#include <GLUT/GLUT.h>
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include <math.h>
#include "Ball.h"
#include "Pocket.h"
#include "Collision.h"
#include <stdio.h>

#define WINDOW_POSITION_X 100   //生成するウィンドウ位置のX座標
#define WINDOW_POSITION_Y 100   //生成するウィンドウ位置のY座標
#define WINDOW_WIDTH 512        //生成するウィンドウの幅
#define WINDOW_HEIGHT 512       //生成するウィンドウの高さ

char WindowTitle[] = "BilliardOpenGL";  //生成するウィンドウのタイトル

int mouse_x, mouse_y;
unsigned char m_key;

float deltaTime;
float power;

//ナインボールなので・・・
Ball ball[10];

//UI用
Ball ballUI[9];

//Pocket
Pocket pocket[6];

//落とすべきボール
int targetBall;

//Scene管理
typedef enum {
    eScene_Menu,    //メニュー画面（というかスタート画面）
    eScene_Game,    //ゲーム画面
    eScene_Result,  //クリア画面
} eScene;

static int Scene = eScene_Menu; //defaltでメニュー画面

//GameScene内の　打つ時　動いてる時（ターン）　を管理
//typedef enum {
//    eTurn_Shoot,
//    eTurn_Move,
//} eTurn;
//static int Turn = eTurn_Shoot;

bool foulsDiffBallFlg;
bool foulsCueBallFlg;

bool clearFlg;

//プロトタイプ宣言
void display(void);
void idle(void);
void Initialize();
void RenderCube(float width, float height, float depth);
//void mouse_motion(int button, int state, int x, int y);
void mouse_on(int button, int state, int x, int y);
//void _CollisionBallCheck(Ball *ball);
void InitializeBallPosition(Ball *ball);
void InitializePocketPosition(Pocket *pocket);
void PowerGaugeUI(float deltaTime);
void InitializeBallOrderUI(Ball *ball);
void DrawString(const char *str, void *font, float Width, float x, float y, float z);
void keybord(unsigned char key, int x, int y);
void Game();
void Menu();
void Result();
void UpdateScene();
bool CanShootBall(Ball *ball);
void _FallPocketCheck(Pocket *pocket, Ball *ball);
bool GameClearCheck(Ball *ball);

//main関数
int main(int argc, char *argv[])
{
    glutInit(&argc, argv);                                          //環境の初期化
    glutInitWindowPosition(WINDOW_POSITION_X, WINDOW_POSITION_Y);   //ウィンドウの位置の指定
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);                //ウィンドウサイズの指定
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);                   //ディスプレイモードの指定
    glutCreateWindow(WindowTitle);                                  //ウィンドウの作成
    glutDisplayFunc(display);                                       //描画時に呼び出される関数を指定
    glutIdleFunc(idle);
    glutMouseFunc(mouse_on);
    glutKeyboardFunc(keybord);
    //glutMouseFunc(mouse_motion);
    Initialize();                                                   //初期化
    glutMainLoop();
    return 0;
}

//初期化関数
void Initialize()
{
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);   //背景色の設定
    glEnable(GL_DEPTH_TEST);                //デプスバッファを使用：glutInitDisplayMode() で GLUT_DEPTH を指定する
    
    InitializeBallPosition(ball);
    
    InitializePocketPosition(pocket);
    
    InitializeBallOrderUI(ball);
    
    targetBall = 1;
    foulsDiffBallFlg = false;
    foulsCueBallFlg = false;
    clearFlg = false;
    
    //    for(int i=0; i<10; i++){
    //        ball[i].Initialize(glm::vec3((float)i, 0.f, (float)i));
    //        ball[i].AddVelocity(glm::vec3((float)i*10, 0.f, (float)i*10));
    //    }
}

//描画関数 (1Fに1回呼ばれる)
void display(void)
{
    deltaTime = glutGet(GLUT_ELAPSED_TIME)/1000.f;
    //printf("%f\n",deltaTime);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //バッファの削除
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    //視野角,アスペクト比(ウィンドウの幅/高さ),描画する範囲(最も近い距離,最も遠い距離)
    gluPerspective(30.0, (double)WINDOW_WIDTH/(double)WINDOW_HEIGHT, 0.1, 1000.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    //視点の設定
    /*gluLookAt(
              0.0, 550.0, 0.0,    //視点の位置x,y,z;
              0.0, 0.0, 0.0,        //視界の中心位置の参照点座標x,y,z
              0.0, 0.0, -1.0);       //視界の上方向のベクトルx,y,z
    */

    switch(Scene){
        case eScene_Menu:
            Menu();
            Initialize();
            break;
        case eScene_Game:
            Game();
            break;
        case eScene_Result:
            Result();
            break;
    }
    
    UpdateScene();
    
    /*
    //カラー指定
    glColor3d(1.0, 1.0, 1.0);
    //線描画スタイルの指定
    glBegin(GL_LINE_LOOP);
    
    //頂点指定
    glVertex3d(-80.0, 0.0, -145.0);
    glVertex3d(80.0, 0.0, -145.0);
    glVertex3d(80.0, 0.0, 145.0);
    glVertex3d(-80.0, 0.0, 145.0);
    
    //線描画スタイルの指定終了
    glEnd();
    
    PowerGaugeUI(deltaTime);
    
    //BallUIのRender
    for(int i=0; i<10; i++){
        //本家のボールが存在してたら
        if(ball[i+1].GetIsExist()){
            ballUI[i].Render();
        }
    }
    
    // BallのUpdate
    for(int i=0; i<10; i++){
        ball[i].Update();
    }
    
    // BallのCollisionWallCheck
    for(int i=0; i<10; i++){
        ball[i].CollisionWallCheck();
    }
    
    //Ballのポケットに落ちたかのチェック
    for(int i=0; i<10; i++){
        ball[i].FallPocketCheck();
    }
    
    // BallのCollisionBallCheck
    CollisionBallCheck(ball);
    
    // BallのRender
    for(int i=0; i<10; i++){
        if(ball[i].GetIsExist()){
            ball[i].Render();
        }
    }
    
    //pocketのrender
    for(int i=0; i<6; i++){
        pocket[i].Render();
    }
    
    //pocketのfallcheck
    for(int i=0; i<6; i++){
        pocket[i].FallPocketCheck(ball);
    }
    */
    
    glutSwapBuffers();  //glutInitDisplayMode(GLUT_DOUBLE)でダブルバッファリングを利用可
}

void idle(void)
{
    glutPostRedisplay();
}

void RenderCube(float width,float height,float depth)
{
    glBegin(GL_QUADS);
    
    //前
    glNormal3f(0.0,0.0,-1.0);
    glVertex3f(width/2,height/2,depth/2);
    glVertex3f(-width/2,height/2,depth/2);
    glVertex3f(-width/2,-height/2,depth/2);
    glVertex3f(width/2,-height/2,depth/2);
    
    //左
    glNormal3f(1.0,0.0,0.0);
    glVertex3f(width/2,height/2,depth/2);
    glVertex3f(width/2,height/2,-depth/2);
    glVertex3f(width/2,-height/2,-depth/2);
    glVertex3f(width/2,-height/2,depth/2);
    
    //右
    glNormal3f(-1.0,0.0,0.0);
    glVertex3f(-width/2,height/2,-depth/2);
    glVertex3f(-width/2,height/2,depth/2);
    glVertex3f(-width/2,-height/2,depth/2);
    glVertex3f(-width/2,-height/2,-depth/2);
    
    //後
    glNormal3f(0.0,0.0,1.0);
    glVertex3f(width/2,height/2,-depth/2);
    glVertex3f(-width/2,height/2,-depth/2);
    glVertex3f(-width/2,-height/2,-depth/2);
    glVertex3f(width/2,-height/2,-depth/2);
    
    //上
    glNormal3f(0.0,1.0,0.0);
    glVertex3f(width/2,height/2,depth/2);
    glVertex3f(-width/2,height/2,depth/2);
    glVertex3f(-width/2,height/2,-depth/2);
    glVertex3f(width/2,height/2,-depth/2);
    
    //下
    glNormal3f(0.0,-1.0,0.0);
    glVertex3f(width/2,-height/2,depth/2);
    glVertex3f(-width/2,-height/2,depth/2);
    glVertex3f(-width/2,-height/2,-depth/2);
    glVertex3f(width/2,-height/2,-depth/2);
    glEnd();
}

void mouse_on(int button, int state, int x, int y)
{
    //printf("呼ばれたよ");
    
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        if(CanShootBall(ball)){
            //マウスの位置を取得
            mouse_x = x;
            mouse_y = y;
        
            //ワールド座標に変換 y->z
            glm::vec3 mouse_wpos = glm::vec3(300.f/512.f * (mouse_x-256), 0.f, 300.f/512.f * (mouse_y-256));
            //float mouse_wx = 290/512 * mouse_x;
            //float mouse_wz = 290/512 * mouse_y;
        
            //printf("%.1f,%.1f,%.1f\n",mouse_wpos.x,mouse_wpos.y,mouse_wpos.z);
                
            if(!foulsDiffBallFlg){
                //ボールのポジション取得
                glm::vec3 ball_pos = ball[0].GetPosition();
        
                //距離からベクトルを求める
                glm::vec3 v = mouse_wpos - ball_pos;
        
                //単位ベクトル化
                glm::vec3 _v = glm::normalize(v);
            
                //printf("%.1f,%.1f,%.1f\n",v.x,v.y,v.z);
            
                //速度に設定
                ball[0].AddVelocity(_v * power);
            } else {
                ball[0].AddVelocity(glm::vec3(0.f));
                ball[0].SetPosition(mouse_wpos);
                if(foulsCueBallFlg){
                    ball[0].SetIsExsist(true);
                    foulsCueBallFlg = false;
                }
                foulsDiffBallFlg = false;
            }
        }
    }
}

void InitializeBallPosition(Ball *ball)
{
    //キューで打たれる白い球
    ball[0].Initialize(0, glm::vec3(0.f, 0.f, 100.f));
    
    //他の球（本来は1,9以外は自由におけるがご勘弁を）
    ball[1].Initialize(1, glm::vec3(0.f, 0.f, -90.f));
    ball[2].Initialize(2, glm::vec3(-5.f, 0.f, -100.f));
    ball[3].Initialize(3, glm::vec3(5.f, 0.f, -100.f));
    ball[4].Initialize(4, glm::vec3(-10.f, 0.f, -110.f));
    ball[5].Initialize(5, glm::vec3(0.f, 0.f, -110.f));
    ball[6].Initialize(6, glm::vec3(10.f, 0.f, -110.f));
    ball[7].Initialize(7, glm::vec3(-5.f, 0.f, -120.f));
    ball[8].Initialize(8, glm::vec3(5.f, 0.f, -120.f));
    ball[9].Initialize(9, glm::vec3(0.f, 0.f, -130.f));
}

void InitializePocketPosition(Pocket *pocket)
{
    //左上から時計回りに
    pocket[0].Initialize(0, glm::vec3(-80.f, 0.f, -145.f));
    pocket[1].Initialize(1, glm::vec3(80.f, 0.f, -145.f));
    pocket[2].Initialize(2, glm::vec3(80.f, 0.f, 0.f));
    pocket[3].Initialize(3, glm::vec3(80.f, 0.f, 145.f));
    pocket[4].Initialize(4, glm::vec3(-80.f, 0.f, 145.f));
    pocket[5].Initialize(5, glm::vec3(-80.f, 0.f, 0.f));
}

void PowerGaugeUI(float deltaTime)
{
    //10分割
    float tmp = deltaTime;
    float tmp2 = tmp - (int)tmp;
    
    //パワーゲージに応じたパワーの設定
    power = 15.f * tmp2;
    //printf("%f\n",tmp2);
    
    //外身
    glBegin(GL_LINE_LOOP);
    glVertex3i(100, 0, 30);
    glVertex3i(120, 0, 30);
    glVertex3i(120, 0, 110);
    glVertex3i(100, 0, 110);
    glEnd();
    
    //中身
    glBegin(GL_QUADS);
    //左上から時計回りに
    glVertex3i(100, 0, 110 - tmp2*80);
    glVertex3i(120, 0, 110 - tmp2*80);
    glVertex3i(120, 0, 110);
    glVertex3i(100, 0, 110);
    glEnd();
}

void InitializeBallOrderUI(Ball *ball)
{
    //UI用ボールに情報を渡す
    for(int i=0; i < 9; i++){
        ballUI[i] = ball[i+1];
    }
    
    for(int i=0; i<9; i++){
        ballUI[i].SetPosition(glm::vec3(-110.f, 0.f, -120.f+30*i));
    }
}

void UpdateScene()
{
    if(m_key == 'g'){
        Scene = eScene_Game;
    }
    if(m_key == 'm'){
        Scene = eScene_Menu;
    }
    if(GameClearCheck(ball) || m_key == 'r'){
        Scene = eScene_Result;
        Initialize();
    }
}

void Menu()
{
    //視点の設定
    gluLookAt(
              0.0, 0.0, 1000.0,    //視点の位置x,y,z;
              0.0, 0.0, 0.0,        //視界の中心位置の参照点座標x,y,z
              0.0, 1.0, 0.0);       //視界の上方向のベクトルx,y,z
    
    DrawString("Billiards", GLUT_STROKE_ROMAN, 5.f, -210.f, -50.f, 0.f);
    //DrawString("Press Enter", GLUT_STROKE_ROMAN, 5.f, -600.f, -70.f, 0.f);
}

void Game()
{
    //視点の設定
    gluLookAt(
              0.0, 550.0, 0.0,    //視点の位置x,y,z;
              0.0, 0.0, 0.0,        //視界の中心位置の参照点座標x,y,z
              0.0, 0.0, -1.0);       //視界の上方向のベクトルx,y,z
    
    //カラー指定
    glColor3d(1.0, 1.0, 1.0);
    //線描画スタイルの指定
    glBegin(GL_LINE_LOOP);
    
    //頂点指定
    glVertex3d(-80.0, 0.0, -145.0);
    glVertex3d(80.0, 0.0, -145.0);
    glVertex3d(80.0, 0.0, 145.0);
    glVertex3d(-80.0, 0.0, 145.0);
    
    //線描画スタイルの指定終了
    glEnd();
    
    //球が打てる状況なら
    if(CanShootBall(ball) && !foulsCueBallFlg && !foulsDiffBallFlg){
        PowerGaugeUI(deltaTime);
    }
    
    //BallUIのRender
    for(int i=0; i<10; i++){
        //本家のボールが存在してたら
        if(ball[i+1].GetIsExist()){
            ballUI[i].Render();
        }
    }
    
    // BallのUpdate
    for(int i=0; i<10; i++){
        ball[i].Update();
    }
    
    // BallのCollisionWallCheck
    for(int i=0; i<10; i++){
        ball[i].CollisionWallCheck();
    }
    
    //Ballのポケットに落ちたかのチェック
    for(int i=0; i<10; i++){
        ball[i].FallPocketCheck();
    }
    
    // BallのCollisionBallCheck
    CollisionBallCheck(ball);
    
    // BallのRender
    for(int i=0; i<10; i++){
        if(ball[i].GetIsExist()){
            ball[i].Render();
        }
    }
    
    //pocketのrender
    for(int i=0; i<6; i++){
        pocket[i].Render();
    }
    
    //pocketのfallcheck
    //for(int i=0; i<6; i++){
    //    pocket[i].FallPocketCheck(ball, targetBall);
    //}
    
    //pocketとballのcheck
    _FallPocketCheck(pocket, ball);
}

void Result()
{
    //視点の設定
    gluLookAt(
              0.0, 0.0, 1000.0,    //視点の位置x,y,z;
              0.0, 0.0, 0.0,        //視界の中心位置の参照点座標x,y,z
              0.0, 1.0, 0.0);       //視界の上方向のベクトルx,y,z
    
    DrawString("Clear", GLUT_STROKE_ROMAN, 5.f, -150.f, -50.f, 0.f);
}

void keybord(unsigned char key, int x, int y)
{
    m_key = key;
    //printf("%c", m_key);
}

void DrawString(const char *str, void *font, float Width, float x, float y, float z)
{
    glPushAttrib(GL_LINE_BIT);
    glLineWidth(Width);
    glColor3d(1.f, 1.f, 1.f);
    glTranslatef(x, y, z);
    //glScalef(0.7, 0.7, 0.7);
    while(*str){
        glutStrokeCharacter(font, *str);
        ++str;
    }
    glPopAttrib();
}

bool CanShootBall(Ball *ball)
{
    //if(foulsDiffBallFlg || foulsCueBallFlg){
    //    return false;
    //}
    for(int i=0; i<10; i++){
        if(!ball[i].GetIsExist()){
            continue;
        }
        if(ball[i].GetVelocity() != glm::vec3(0.f)){
            return false;
        }
    }
    return true;
}

void _FallPocketCheck(Pocket *pocket, Ball *ball)
{
    for(int i=0; i<6; i++){
        for(int j=0; j<10; j++){
            if(!ball[j].GetIsExist()){
                continue;
            }
        
            glm::vec3 a_pos = pocket[i].GetPosition();
            glm::vec3 b_pos = ball[j].GetPosition();
        
            float abx_v = b_pos.x - a_pos.x;
            float abz_v = b_pos.z - a_pos.z;
        
            float tr = pocket[i].GetRadius() + ball[j].GetRadius();
        
            if(tr * tr > (abx_v * abx_v) + (abz_v * abz_v)){
                ball[j].SetIsExsist(false);
                
                //ファウル行為（ターゲット以外を落としちゃった）
                if(ball[j].GetID() != targetBall){
                    foulsDiffBallFlg = true;
                    printf("foulsDiff:%d\n", (int)foulsDiffBallFlg);
                    
                    //それが手玉なら
                    if(ball[j].GetID() == 0){
                        foulsCueBallFlg = true;
                        printf("foulsCue:%d\n", (int)foulsCueBallFlg);
                    }
                } else {    //ちゃんとターゲットを落としたらターゲットを進める
                    for(int k=1; k<10; k++){
                        if(ball[k].GetIsExist()){
                            targetBall = ball[k].GetID();
                            break;
                        }
                    }
                }
            }
        }
    }
}

bool GameClearCheck(Ball *ball){
    for(int i=1; i<10; i++){
        if(ball[i].GetIsExist()){
            return false;
        }
    }
    return true;
}
