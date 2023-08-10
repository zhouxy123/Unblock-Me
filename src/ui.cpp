#include "../cge/cge.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include "unblock.h"
#include <stdlib.h>

using namespace std;

#define LEVEL "L1"
#define EDGE_COLOR 9

// 用于探测
#define RIGHT 0
#define LEFT 1
#define DOWN 2
#define UP 3

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
    int x_press, y_press;
    int x_temp, y_temp;
    int x_release, y_release;
    int id;
    int max_step;
    int delta_x, delta_y;
} GAME_MODEL;
GAME_MODEL g_model;

// 定时器ID
int g_moving_timer;

// 事件发射器
int g_emitter;

// box
int game_region;

// 测试事件
void emit_handle(void *m)
{
    cge_box_mvprintf(game_region, 2, 2, 2, (const wchar_t *)m);
}

// 清空显示
void clear_screen()
{
    int i,j;
    for (i = 1; i <= 25; i++)
    {
        for (j = 1; j <= 13; j++)
        {
            cge_box_mvprintf(game_region, i, j, 1, L" ");
        }
    }
}

// 实际画出所有块
void draw_blocks()
{
    int x, y;
    char c;

    clear_screen();

    for (x = 1; x <= 25; x++)
    {
        for (y = 1; y <= 13; y++)
        {
            c = box_record[x][y];

            if (c == '|')
                cge_box_mvprintf(game_region, x, y, EDGE_COLOR, L"║");
            if (c == '=')
                cge_box_mvprintf(game_region, x, y, EDGE_COLOR, L"═");
            if (c == '+')
                cge_box_mvprintf(game_region, x, y, EDGE_COLOR, L"+");
            if (c == '&')
                cge_box_mvprintf(game_region, x, y, 10, L"|");
            if (c == '#')
                cge_box_mvprintf(game_region, x, y, 13, L"|");
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

// 胜利
void victory()
{
    cge_box_mvprintf(game_region, 10, 0, 12, L"VICTORY");
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
    record_blocks();
    draw_blocks();
}

// 更新数据
void cge_update(double dt)
{
    if (dt <= 0)
        return;

    int move_state;
    int i;
    int x_start_origin, y_start_origin, x_end_origin, y_end_origin;
    for (i = 0; i < cge_events_count; i++)
    {
        CGE_USER_EVENT *ue = &cge_events[i];
        // if (g_model.state == NORMAL && ue->type == CGE_MOUSE)

        if (ue->type == CGE_MOUSE)
        {
            if (ue->mouse_bstate == BUTTON1_PRESSED)
            {
                g_model.x_press = ue->x - 5;
                g_model.y_press = ue->y - 2;
                g_model.id = get_block_id(g_model.x_press, g_model.y_press);
                record_blocks();
                draw_blocks();
            }

            // 0x80000: 按下左键并拖动
            if (ue->mouse_bstate == 0x80000 && g_model.id >= 0)
            {
                g_model.x_temp = ue->x - 5;
                g_model.y_temp = ue->y - 2;
                x_start_origin = 4 * blocks[g_model.id].x + 1;
                y_start_origin = 2 * blocks[g_model.id].y + 1;
        
                if (blocks[g_model.id].direction == 0)
                {
                    x_end_origin = x_start_origin + 4 * blocks[g_model.id].length;
                    g_model.delta_x = g_model.x_temp - g_model.x_press;

                    if (g_model.delta_x >= 0)//往右拖
                    {
                        g_model.max_step = block_detect(x_end_origin + g_model.delta_x, y_start_origin, RIGHT);

                        if(g_model.max_step >= 0 && g_model.delta_x <= 4)
                        {
                            set_position(g_model.id, x_start_origin + g_model.delta_x, y_start_origin);
                            draw_blocks();
                        }
                    }
                    else //往左拖
                    {
                        g_model.max_step = block_detect(x_start_origin + g_model.delta_x, y_start_origin, LEFT);

                        if(g_model.max_step <= 0 && g_model.delta_x >= -4)
                        {
                            set_position(g_model.id, x_start_origin + g_model.delta_x, y_start_origin);
                            draw_blocks();
                        }
                    }
                    
                }
                if (blocks[g_model.id].direction == 1)
                {
                    y_end_origin = y_start_origin + 2 * blocks[g_model.id].length;
                    g_model.delta_y = g_model.y_temp - g_model.y_press;

                    if (g_model.delta_y >= 0)//往下拖
                    {
                        g_model.max_step = block_detect(x_start_origin, y_end_origin + g_model.delta_y, DOWN);

                        if(g_model.max_step >= 0 && g_model.delta_y <= 2)
                        {
                            set_position(g_model.id, x_start_origin, y_start_origin + g_model.delta_y);
                            draw_blocks();
                        }
                    }
                    else //往上拖
                    {
                        g_model.max_step = block_detect(x_start_origin, y_start_origin + g_model.delta_y, UP);
                        if(g_model.max_step <= 0 && g_model.delta_y >= -2)
                        {
                            set_position(g_model.id, x_start_origin, y_start_origin + g_model.delta_y);
                            draw_blocks();
                        }
                    }
                }
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
                    if (if_valid() < 0)
                    {
                        blocks[g_model.id].move((-1) * move_state);
                    }
                }
                record_blocks();
                draw_blocks();
            }

            if (if_successful())
            {
                victory();
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