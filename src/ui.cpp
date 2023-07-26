#include "../cge/cge.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include "unblock.h"
#include <stdlib.h>

using namespace std;

#define LEVEL "L2"
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
    int x, y;
} GAME_MODEL;
GAME_MODEL g_model;

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

void draw_blocks(vector<Block> blocks)
{
    int i, j;
    int draw_x_start, draw_y_start; // 实际画图的x,y
    int draw_x_end, draw_y_end;
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
                //if ((i - draw_y_start) % 2 == 1)
                //{
                    cge_box_mvprintf(game_region, draw_x_start + 1, i, 13, L"|");
                    cge_box_mvprintf(game_region, draw_x_start + 2, i, 13, L"|");
                    cge_box_mvprintf(game_region, draw_x_start + 3, i, 13, L"|");
                //}
                /*else
                {
                    cge_box_mvprintf(game_region, draw_x_start + 1, i, 13, L"-");
                    cge_box_mvprintf(game_region, draw_x_start + 2, i, 13, L"-");
                    cge_box_mvprintf(game_region, draw_x_start + 3, i, 13, L"-");
                }*/
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
                // cge_box_mvprintf(game_region, 4 * i - 3, 2 * j - 1, 9, L"+");
                /*c = cge_boxes[game_region].img.content[y][x]; */
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

    /*for (i = 1; i <= 7; i++)
    {
        for (j = 1; j <= 7; j++)
        {
            cge_box_mvprintf(game_region, 4 * i - 3, 2 * j - 1, 9, L"+");
        }
    }*/

    cge_box_mvprintf(game_region, 13, 0, 1, L"%lc", LEVEL[1]);

    g_emitter = cge_emitter_register();
    cge_emitter_addob(g_emitter, emit_handle);

    blocks_init(LEVEL);
    draw_blocks(blocks);
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
    return 0;
}