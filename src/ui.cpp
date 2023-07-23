#include "../cge/cge.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include "unblock.h"

using namespace std;

//游戏状态
typedef enum game_state
{
    NORMAL,
    MOVING
} GAME_STATE;

//全局数据
typedef struct game_model
{
    GAME_STATE state;
    int x, y;
} GAME_MODEL;
GAME_MODEL g_model;

//事件发射器
int g_emitter;

//box
int game_region;

//测试事件
void emit_handle(void *m)
{
    cge_box_mvprintf(game_region, 2, 2, 2, (const wchar_t *)m);
}

//初始化
void init()
{
    int i,j;
    g_model.state = NORMAL;
    cge_init_term();

    game_region = cge_box_register(27, 15, 5, 2);
    cge_box_mvprintf(game_region, 0, 0, 1, L"╔");
    for (i = 1; i <= 25; i++)
    {
        cge_box_mvprintf(game_region, i, 0, 1, L"═");
    }
    cge_box_mvprintf(game_region, 26, 0, 1, L"╗");
    for (i = 1; i <= 13; i++)
    {
        cge_box_mvprintf(game_region, 0, i, 1, L"║");
        if(i < 5 || i > 7)
        cge_box_mvprintf(game_region, 26, i, 1, L"║");
    }
    cge_box_mvprintf(game_region, 0, 14, 1, L"╚");
    for (i = 1; i <= 25; i++)
    {
        cge_box_mvprintf(game_region, i, 14, 1, L"═");
    }
    cge_box_mvprintf(game_region, 26, 14, 1, L"╝");

    for(i=1;i<=7;i++)
    {
        for(j=1;j<=7;j++)
        {
            cge_box_mvprintf(game_region, 4*i-3, 2*j-1, 1, L"+");
        }
    }

    g_emitter = cge_emitter_register();
    cge_emitter_addob(g_emitter, emit_handle);

}



void cge_update(double dt)
{

}

void cge_render(double dt)
{

}

int main()
{
    init();
    cge_main_loop();
    blocks_init("L2");
    for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); it++)
    {
        cout << "block" << (*it).id << ": x=" << (*it).x << " y=" << (*it).y << " length=" << (*it).length << " direction=" << (*it).direction << endl;
    }
}