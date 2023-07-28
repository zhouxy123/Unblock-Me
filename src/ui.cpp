#include "../cge/cge.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include "unblock.h"
#include <stdlib.h>

using namespace std;

#define LEVEL "L1"
#define EDGE_COLOR 9

// 游戏状态
typedef enum game_state
{
    NORMAL,
    MOVING
} GAME_STATE;

// 全局数据
typedef struct game_model
{
    GAME_STATE state;
    int x_press, y_press, x_release, y_release;
    int id;
} GAME_MODEL;
GAME_MODEL g_model;

// 定时器ID
int g_moving_timer;

// 事件发射器
int g_emitter;

// box
int game_region;

// 画块时，在此字符数组里同步记录
char box_record[27][15] = {};

// 测试事件
void emit_handle(void *m)
{
    cge_box_mvprintf(game_region, 2, 2, 2, (const wchar_t *)m);
}

void draw_blocks()
{
    int i, j;
    int draw_x_start, draw_y_start; // 实际画图的x,y
    int draw_x_end, draw_y_end;

    for(i = 1; i<=25; i++)
    {
        for(j=1; j<=13; j++)
        {
            cge_box_mvprintf(game_region, i, j, 1, L" ");
            box_record[i][j] = 0;
        }
    }
    for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); it++)
    {
        draw_x_start = 4 * (*it).x + 1;
        draw_y_start = 2 * (*it).y + 1;

        if ((*it).direction == 0)
        {
            draw_x_end = draw_x_start + 4 * (*it).length;
            cge_box_mvprintf(game_region, draw_x_start, draw_y_start + 1, EDGE_COLOR, L"║");
            cge_box_mvprintf(game_region, draw_x_end, draw_y_start + 1, EDGE_COLOR, L"║");
            box_record[draw_x_start][draw_y_start + 1] = '|';
            box_record[draw_x_end][draw_y_start + 1] = '|';
            for (i = draw_x_start + 1; i < draw_x_end; i++)
            {
                if ((i - draw_x_start) % 4 == 0)
                {
                    continue;
                }
                cge_box_mvprintf(game_region, i, draw_y_start, EDGE_COLOR, L"═");
                cge_box_mvprintf(game_region, i, draw_y_start + 2, EDGE_COLOR, L"═");
                box_record[i][draw_y_start] = '=';
                box_record[i][draw_y_start + 2] = '=';
            }

            if ((*it).id == 0)
            {
                for (i = draw_x_start + 1; i < draw_x_end; i++)
                {
                    cge_box_mvprintf(game_region, i, draw_y_start + 1, 10, L"|");
                }
            }

            else
            {
                for (i = draw_x_start + 1; i < draw_x_end; i++)
                {
                    cge_box_mvprintf(game_region, i, draw_y_start + 1, 13, L"|");
                }
            }
        }

        else
        {
            draw_y_end = draw_y_start + 2 * (*it).length;
            for (i = draw_x_start + 1; i < draw_x_start + 4; i++)
            {
                cge_box_mvprintf(game_region, i, draw_y_start, EDGE_COLOR, L"═");
                cge_box_mvprintf(game_region, i, draw_y_end, EDGE_COLOR, L"═");
                box_record[i][draw_y_start] = '=';
                box_record[i][draw_y_end] = '=';
            }
            for (i = draw_y_start + 1; i < draw_y_end; i++)
            {
                if ((i - draw_y_start) % 2 == 0)
                {
                    continue;
                }
                cge_box_mvprintf(game_region, draw_x_start, i, EDGE_COLOR, L"║");
                cge_box_mvprintf(game_region, draw_x_start + 4, i, EDGE_COLOR, L"║");
                box_record[draw_x_start][i] = '|';
                box_record[draw_x_start + 4][i] = '|';
            }
            for (i = draw_y_start + 1; i < draw_y_end; i++)
            {
                cge_box_mvprintf(game_region, draw_x_start + 1, i, 13, L"|");
                cge_box_mvprintf(game_region, draw_x_start + 2, i, 13, L"|");
                cge_box_mvprintf(game_region, draw_x_start + 3, i, 13, L"|");
            }
        }

        int x, y;
        char c_up, c_down, c_left, c_right;
        for (i = 1; i <= 7; i++)
        {
            for (j = 1; j <= 7; j++)
            {
                x = 4 * i - 3;
                y = 2 * j - 1;

                c_up = box_record[x][y - 1];
                c_down = box_record[x][y + 1];
                c_left = box_record[x - 1][y];
                c_right = box_record[x + 1][y];

                if ((c_up != '|') && (c_down != '|') && (c_left == '=') && (c_right == '='))
                    cge_box_mvprintf(game_region, x, y, EDGE_COLOR, L"═");

                else if ((c_up != '|') && (c_down == '|') && (c_left != '=') && (c_right == '='))
                    cge_box_mvprintf(game_region, x, y, EDGE_COLOR, L"╔");

                else if ((c_up != '|') && (c_down == '|') && (c_left == '=') && (c_right != '='))
                    cge_box_mvprintf(game_region, x, y, EDGE_COLOR, L"╗");

                else if ((c_up == '|') && (c_down != '|') && (c_left != '=') && (c_right == '='))
                    cge_box_mvprintf(game_region, x, y, EDGE_COLOR, L"╚");

                else if ((c_up == '|') && (c_down != '|') && (c_left == '=') && (c_right != '='))
                    cge_box_mvprintf(game_region, x, y, EDGE_COLOR, L"╝");

                else if ((c_up == '|') && (c_down == '|') && (c_left != '=') && (c_right != '='))
                    cge_box_mvprintf(game_region, x, y, EDGE_COLOR, L"║");

                else if ((c_up != '|') && (c_down == '|') && (c_left == '=') && (c_right == '='))
                    cge_box_mvprintf(game_region, x, y, EDGE_COLOR, L"╦");

                else if ((c_up == '|') && (c_down != '|') && (c_left == '=') && (c_right == '='))
                    cge_box_mvprintf(game_region, x, y, EDGE_COLOR, L"╩");

                else if ((c_up == '|') && (c_down == '|') && (c_left != '=') && (c_right == '='))
                    cge_box_mvprintf(game_region, x, y, EDGE_COLOR, L"╠");

                else if ((c_up == '|') && (c_down == '|') && (c_left == '=') && (c_right != '='))
                    cge_box_mvprintf(game_region, x, y, EDGE_COLOR, L"╣");

                else if ((c_up == '|') && (c_down == '|') && (c_left == '=') && (c_right == '='))
                    cge_box_mvprintf(game_region, x, y, EDGE_COLOR, L"╬");
            }
        }
    }
}

// 定时器走完后的回调函数
void timer_end_moving()
{
    g_model.state = NORMAL;
    int ed = (int)(size_t)cge_timer_getexdata(g_moving_timer);
    g_model.x_press = ed;
    cge_emitter_notify(g_emitter, (void *)L"BB");
}

// 初始化
void init()
{
    int i;
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
        if (i < 5 || i > 7)
            cge_box_mvprintf(game_region, 26, i, 1, L"║");
    }
    cge_box_mvprintf(game_region, 0, 14, 1, L"╚");
    for (i = 1; i <= 25; i++)
    {
        cge_box_mvprintf(game_region, i, 14, 1, L"═");
    }
    cge_box_mvprintf(game_region, 26, 14, 1, L"╝");

    cge_box_mvprintf(game_region, 13, 0, 1, L"%lc", LEVEL[1]);

    // 测试事件触发
    g_emitter = cge_emitter_register();
    cge_emitter_addob(g_emitter, emit_handle);

    blocks_init(LEVEL);
    draw_blocks();
}

// 更新数据
void cge_update(double dt)
{
    if (dt <= 0)
        return;

    int move_state;
    for (int i = 0; i < cge_events_count; i++)
    {
        CGE_USER_EVENT *ue = &cge_events[i];
        if (g_model.state == NORMAL && ue->type == CGE_MOUSE)
        {
            if (ue->mouse_bstate == BUTTON1_PRESSED)
            {
                g_model.x_press = ue->x - 5;
                g_model.y_press = ue->y - 2;
                g_model.id = get_block_id(g_model.x_press, g_model.y_press); 
            }

            // 0x40000: 按下过程中释放
            if (ue->mouse_bstate == 0x40000)
            {
                g_model.x_release = ue->x - 5;
                g_model.y_release = ue->y - 2;

                if (g_model.id >= 0)
                {
                    if ((blocks[g_model.id].direction == 0) && (g_model.x_release > g_model.x_press))
                    {
                        move_state = 1;
                    }

                    else if ((blocks[g_model.id].direction == 0) && (g_model.x_release < g_model.x_press))
                    {
                        move_state = -1;
                    }

                    else if ((blocks[g_model.id].direction == 1) && (g_model.y_release > g_model.y_press))
                    {
                        move_state = 1;
                    }

                    else if ((blocks[g_model.id].direction == 1) && (g_model.y_release < g_model.y_press))
                    {
                        move_state = -1;
                    }
                    
                    else
                    {
                        move_state = 0;
                    }

                    blocks[g_model.id].move(move_state);
                    if(if_valid()<0)
                    {
                        blocks[g_model.id].move((-1)*move_state);
                    }
                    
                }
            }
            draw_blocks();

            if(if_successful())
            {
                cge_box_mvprintf(game_region, 10, 0, 12, L"VICTORY");
            }
        }
    }
    cge_events_count = 0;
}

void cge_render(double dt)
{
}

int main()
{
    init();
    cge_main_loop();
    return 0;
}