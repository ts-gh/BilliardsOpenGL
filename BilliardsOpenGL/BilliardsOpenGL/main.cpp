#include <GLUT/GLUT.h>
#include "glm/glm.hpp"
#include "glm/vec3.hpp"
#include <math.h>
#include "Ball.h"
#include "Pocket.h"
#include "Collision.h"
#include <stdio.h>  // Debug用

#define WINDOW_POSITION_X 100   //生成するウィンドウ位置のX座標
#define WINDOW_POSITION_Y 100   //生成するウィンドウ位置のY座標
#define WINDOW_WIDTH 512        //生成するウィンドウの幅
#define WINDOW_HEIGHT 512       //生成するウィンドウの高さ

char WindowTitle[] = "BilliardOpenGL";  //生成するウィンドウのタイトル

int mouse_x, mouse_y;
unsigned char m_key;

float deltaTime;
float power;

//ナインボールなのでボールは10個
Ball ball[10];

//UI用のボール
Ball ballUI[9];

//プール台のポケット
Pocket pocket[6];

//落とすべきボール（ID）
int targetBall;

//Scene管理
typedef enum {
    eScene_Menu,    //メニュー画面（というかスタート画面）
    eScene_Game,    //ゲーム画面
    eScene_Result,  //クリア画面
} eScene;

static int Scene = eScene_Menu; //defaltでメニュー画面

//Foulsの種類を別々のフラグで管理
bool foulsDiffBallFlg;  //targetBall以外を落としたら
bool foulsCueBallFlg;   //白球を落としたら

bool gameClearFlg;

//プロトタイプ宣言
void display(void);
void idle(void);
void Initialize();
void mouse_on(int button, int state, int x, int y);
void InitializeBallPosition(Ball *ball);
void InitializeBallOrderUI(Ball *ball);
void InitializePocketPosition(Pocket *pocket);
void PowerGaugeUI(float deltaTime);
void DrawString(const char *str, void *font, float Width, float x, float y, float z);
void keybord(unsigned char key, int x, int y);
void Game();
void Menu();
void Result();
void UpdateScene();
bool CanShootBall(Ball *ball);
void FallPocketCheck(Pocket *pocket, Ball *ball);
bool GameClearCheck(Ball *ball);
void Pool();

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
    Initialize();                                                   //初期化
    glutMainLoop();
    return 0;
}

//初期化関数
void Initialize()
{
    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);   //背景色の設定
    glEnable(GL_DEPTH_TEST);                //デプスバッファを使用：glutInitDisplayMode() で GLUT_DEPTH を指定する
    
    InitializeBallPosition(ball);
    InitializeBallOrderUI(ball);
    InitializePocketPosition(pocket);
    
    targetBall = 1;
    foulsDiffBallFlg = false;
    foulsCueBallFlg = false;
    gameClearFlg = false;
}

//描画関数 (1Fに1回呼ばれる)
void display(void)
{
    deltaTime = glutGet(GLUT_ELAPSED_TIME)/1000.f;  //ミリ秒(ms) ->　秒(s)　に変換
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //バッファの削除
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    //視野角,アスペクト比(ウィンドウの幅/高さ),描画する範囲(最も近い距離,最も遠い距離)
    gluPerspective(30.0, (double)WINDOW_WIDTH/(double)WINDOW_HEIGHT, 0.1, 1000.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

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
    
    glutSwapBuffers();  //glutInitDisplayMode(GLUT_DOUBLE)でダブルバッファリングを利用可
}

void idle(void)
{
    glutPostRedisplay();
}

void mouse_on(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        if(CanShootBall(ball)){
            //マウスの位置を取得
            mouse_x = x;
            mouse_y = y;
        
            //ワールド座標に変換（ウィンドウサイズとマウス位置から比率で算出　このとき、y -> z　に）
            glm::vec3 mouse_wpos = glm::vec3(300.f/512.f * (mouse_x-256), 0.f, 300.f/512.f * (mouse_y-256));
                
            if(!foulsDiffBallFlg){
                //ボールのポジション取得
                glm::vec3 ball_pos = ball[0].GetPosition();
        
                //位置からベクトルを求める
                glm::vec3 v = mouse_wpos - ball_pos;
        
                //単位ベクトル化
                glm::vec3 _v = glm::normalize(v);
            
                //速度に設定
                ball[0].AddVelocity(_v * power);
            } else {
                //速度を0に設定
                ball[0].AddVelocity(glm::vec3(0.f));
                
                //クリック位置に移動
                ball[0].SetPosition(mouse_wpos);
                
                //白球が落ちてたら
                if(foulsCueBallFlg){
                    //可視化する
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
    //キューで打たれる白球
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
    pocket[2].Initialize(2, glm::vec3(81.f, 0.f, 0.f));
    pocket[3].Initialize(3, glm::vec3(80.f, 0.f, 145.f));
    pocket[4].Initialize(4, glm::vec3(-80.f, 0.f, 145.f));
    pocket[5].Initialize(5, glm::vec3(-81.f, 0.f, 0.f));
}

void PowerGaugeUI(float deltaTime)
{
    //小数点以下をパワーゲージに使用
    float tmp = deltaTime;
    float tmp2 = tmp - (int)tmp;
    
    //パワーゲージに応じたパワーの設定（15.fは任意）
    power = 15.f * tmp2;
    
    glColor3d(1, 1, 1);
    
    //外身（左上から時計回りに）
    glBegin(GL_LINE_LOOP);
    glVertex3i(100, 0, 30);
    glVertex3i(120, 0, 30);
    glVertex3i(120, 0, 110);
    glVertex3i(100, 0, 110);
    glEnd();
    
    //中身の背景[グラデーション]（左上から時計回りに）
    glBegin(GL_QUADS);
    glColor3d(1.f, 0.f, 0.f);
    glVertex3i(100, 0, 30);
    glVertex3i(120, 0, 30);
    glColor3d(1.f, 1.f, 0.f);
    glVertex3i(120, 0, 110);
    glVertex3i(100, 0, 110);
    glEnd();
    
    //中身（左上から時計回りに）
    glColor3d(1.f, 1.f, 1.f);
    glBegin(GL_QUADS);
    glVertex3i(100, 0, 110 - tmp2*80);
    glVertex3i(120, 0, 110 - tmp2*80);
    glVertex3i(120, 0, 30);
    glVertex3i(100, 0, 30);
    glEnd();
}

void InitializeBallOrderUI(Ball *ball)
{
    //UI用ボールに情報を渡す
    for(int i=0; i < 9; i++){
        ballUI[i] = ball[i+1];
    }
    
    //画面左端に縦に並べる
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
        Initialize();
    }
    if(GameClearCheck(ball) || m_key == 'r'){   //Debug用に'r'キーでも遷移するように
        Scene = eScene_Result;
        Initialize();
    }
    
    //m_keyのクリア
    m_key = 0;
}

void Menu()
{
    //視点の設定
    gluLookAt(
              0.0, 0.0, 1000.0,     //視点の位置x,y,z;
              0.0, 0.0, 0.0,        //視界の中心位置の参照点座標x,y,z
              0.0, 1.0, 0.0);       //視界の上方向のベクトルx,y,z
    
    DrawString("Billiards", GLUT_STROKE_ROMAN, 5.f, -210.f, -50.f, 0.f);
    //DrawString("Press Enter", GLUT_STROKE_ROMAN, 5.f, -600.f, -70.f, 0.f);    //のちに実装予定
}

void Game()
{
    //視点の設定
    gluLookAt(
              0.0, 550.0, 0.0,      //視点の位置x,y,z;
              0.0, 0.0, 0.0,        //視界の中心位置の参照点座標x,y,z
              0.0, 0.0, -1.0);      //視界の上方向のベクトルx,y,z
    
    Pool();
    
    //球が打てる状況ならパワーゲージを表示
    if(CanShootBall(ball) && !foulsCueBallFlg && !foulsDiffBallFlg){
        PowerGaugeUI(deltaTime);
    }
    
    //PocketのRender
    for(int i=0; i<6; i++){
        pocket[i].Render();
    }
    
    for(int i=0; i<10; i++){
        
        //BallUIのRender
        if(ball[i+1].GetIsExist()){
            ballUI[i].Render();
        }
        
        if(ball[i].GetIsExist()){
            
            //BallのUpdate
            ball[i].Update();
            
            //BallのRender
            ball[i].Render();
            
            //Ballと壁とのCollisionチェック
            ball[i].CollisionWallCheck();
            
        }
    }
    
    //Ball同士のCollisionチェック
    CollisionBallCheck(ball);
    
    //PocketとBallのCollisionチェック
    FallPocketCheck(pocket, ball);
    
}

void Result()
{
    //視点の設定
    gluLookAt(
              0.0, 0.0, 1000.0,     //視点の位置x,y,z;
              0.0, 0.0, 0.0,        //視界の中心位置の参照点座標x,y,z
              0.0, 1.0, 0.0);       //視界の上方向のベクトルx,y,z
    
    DrawString("Clear", GLUT_STROKE_ROMAN, 5.f, -150.f, -50.f, 0.f);
}

void Pool(){
    
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
    
    
    glColor3d(71.f/255.f, 107.f/255.f, 85.f/255.f);

    glBegin(GL_QUADS);
    glVertex3d(-80.0, 0.0, -145.0);
    glVertex3d(80.0, 0.0, -145.0);
    glVertex3d(80.0, 0.0, 145.0);
    glVertex3d(-80.0, 0.0, 145.0);
    glEnd();

}

void keybord(unsigned char key, int x, int y)
{
    m_key = key;
}

void DrawString(const char *str, void *font, float Width, float x, float y, float z)
{
    glPushAttrib(GL_LINE_BIT);
    glLineWidth(Width);
    glColor3d(1.f, 1.f, 1.f);
    glTranslatef(x, y, z);
    //glScalef(0.7, 0.7, 0.7);  //のちに使うかも
    while(*str){
        glutStrokeCharacter(font, *str);
        ++str;
    }
    glPopAttrib();
}

bool CanShootBall(Ball *ball)
{
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

void FallPocketCheck(Pocket *pocket, Ball *ball)
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
                    
                    //それが白球なら
                    if(ball[j].GetID() == 0){
                        foulsCueBallFlg = true;
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
