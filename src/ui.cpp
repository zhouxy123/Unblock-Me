#include "../cge/cge.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include "../include/unblock.h"
#include <stdlib.h>
#include <ncurses.h>

using namespace std;

#define LEVEL "L2"
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
    int right_max, left_max, down_max, up_max;
    int delta_x, delta_y;
    int right_lock, left_lock, down_lock, up_lock;
    int auto_mode; // 1-自动模式
    int step;
    int success; // 1-胜利
} GAME_MODEL;
GAME_MODEL g_model;

// 定时器ID
int g_moving_timer;

// 事件发射器
int g_emitter;

// box
int game_region;
int auto_play;
int next_step;

// 鼠标状态
int button1_down = 0;

// 测试事件
void emit_handle(void *m)
{
    cge_box_mvprintf(game_region, 2, 2, 2, (const wchar_t *)m);
}

// 清空显示
void clear_screen()
{
    int i, j;
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
    g_model.success = 1;
    cge_box_mvprintf(game_region, 10, 0, 12, L"VICTORY");
}

// 自动移动
void auto_move()
{
    g_model.auto_mode = 1; 
    auto_calculate();
    reset();
    // cge_box_mvprintf(game_region, 0, 0, 1, L"AUTO");
    cge_box_mvprintf(auto_play, 2, 1, 12, L"AUTO");
    cge_box_mvprintf(game_region, 10, 0, 1, L"═══════");
    cge_box_mvprintf(game_region, 13, 0, 1, L"%lc", LEVEL[1]);

    cge_box_mvprintf(next_step, 0, 0, 1, L"╔");
    cge_box_mvprintf(next_step, 12, 0, 1, L"╗");
    cge_box_mvprintf(next_step, 0, 2, 1, L"╚");
    cge_box_mvprintf(next_step, 12, 2, 1, L"╝");
    cge_box_mvprintf(next_step, 0, 1, 1, L"║");
    cge_box_mvprintf(next_step, 12, 1, 1, L"║");

    for (int i = 1; i <= 11; i++)
    {
        cge_box_mvprintf(next_step, i, 0, 1, L"═");
        cge_box_mvprintf(next_step, i, 2, 1, L"═");
    }
    cge_box_mvprintf(next_step, 2, 1, 1, L"NEXT STEP");
}

// 初始化
void init()
{
    int i;
    g_model.state = NORMAL;
    g_model.auto_mode = 0;
    g_model.step = 0;
    g_model.success = 0;
    cge_init_term();

    game_region = cge_box_register(27, 15, 5, 2);
    auto_play = cge_box_register(8, 3, 5, 17);
    next_step = cge_box_register(13, 3, 19, 17);

    cge_box_mvprintf(auto_play, 0, 0, 1, L"╔");
    cge_box_mvprintf(auto_play, 7, 0, 1, L"╗");
    cge_box_mvprintf(auto_play, 0, 2, 1, L"╚");
    cge_box_mvprintf(auto_play, 7, 2, 1, L"╝");
    cge_box_mvprintf(auto_play, 0, 1, 1, L"║");
    cge_box_mvprintf(auto_play, 7, 1, 1, L"║");
    for (i = 1; i <= 6; i++)
    {
        cge_box_mvprintf(auto_play, i, 0, 1, L"═");
        cge_box_mvprintf(auto_play, i, 2, 1, L"═");
    }
    cge_box_mvprintf(auto_play, 2, 1, 1, L"AUTO");

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
    // int auto_mode = 0;
    for (i = 0; i < cge_events_count; i++)
    {
        CGE_USER_EVENT *ue = &cge_events[i];
        // if (g_model.state == NORMAL && ue->type == CGE_MOUSE)

        if (ue->type == CGE_MOUSE)
        {
            if (ue->mouse_bstate == BUTTON1_CLICKED)
            {
                if (ue->x > 5 && ue->x < 13 && ue->y > 17 && ue->y < 19 && g_model.auto_mode == 0)
                {
                    auto_move();
                }

                if (ue->x > 19 && ue->x < 32 && ue->y > 17 && ue->y < 19 && g_model.auto_mode == 1 && g_model.success == 0)
                {
                    blocks[steps[g_model.step].id].move(steps[g_model.step].move_state);
                    g_model.step++;
                    record_blocks();
                    draw_blocks();
                }
            }

            /*if (ue->x > 19 && ue->x < 32 && ue->y > 17 && ue->y < 19)
            {
                blocks[3].move(1);
                //cge_box_mvprintf(game_region, 0, 0, 1, L"AUTO");
                record_blocks();
                draw_blocks();
            }*/


            if (ue->mouse_bstate == BUTTON1_PRESSED && g_model.auto_mode == 0 && g_model.success == 0)
            {
                //cout<<blocks[4].x<<" "<<blocks[4].y<<endl;
                //cout<<block_detect(4 * blocks[4].x + 1 + 4 * blocks[4].length, 4 * blocks[4].y + 1, RIGHT)<<endl;
                button1_down = 1;
                g_model.x_press = ue->x - 5;
                g_model.y_press = ue->y - 2;
                g_model.id = get_block_id(g_model.x_press, g_model.y_press);

                x_start_origin = 4 * blocks[g_model.id].x + 1;
                y_start_origin = 2 * blocks[g_model.id].y + 1;
                if(g_model.id != -1)
                {
                    if (blocks[g_model.id].direction == 0)
                    {
                        x_end_origin = x_start_origin + 4 * blocks[g_model.id].length;
                        g_model.right_max = block_detect(x_end_origin, y_start_origin, RIGHT);
                        g_model.left_max = block_detect(x_start_origin, y_start_origin, LEFT);
                        if (g_model.right_max == 0)
                        {
                            g_model.right_lock = 1;
                        }
                        if (g_model.right_max > 0)
                        {
                            g_model.right_lock = 0;
                        }
                        if (g_model.left_max == 0)
                        {
                            g_model.left_lock = 1;
                        }
                        if (g_model.left_max < 0)
                        {
                            g_model.left_lock = 0;
                        }
                    }
                    if (blocks[g_model.id].direction == 1)
                    {
                        y_end_origin = y_start_origin + 2 * blocks[g_model.id].length;
                        g_model.down_max = block_detect(x_start_origin, y_end_origin, DOWN);
                        g_model.up_max = block_detect(x_start_origin, y_start_origin, UP);
                        if (g_model.down_max == 0)
                        {
                            g_model.down_lock = 1;
                        }
                        if (g_model.down_max > 0)
                        {
                            g_model.down_lock = 0;
                        }
                        if (g_model.up_max == 0)
                        {
                            g_model.up_lock = 1;
                        }
                        if (g_model.up_max < 0)
                        {
                            g_model.up_lock = 0;
                        }
                    }
                }
                record_blocks();
                draw_blocks();                
            }

            // 0x80000: 按下左键并拖动
            //if (ue->mouse_bstate == 0x80000 && g_model.id >= 0)
            if (button1_down == 1 && g_model.id >= 0 && g_model.auto_mode == 0 && g_model.success == 0)
            {
                //cge_box_mvprintf(game_region, 0, 0, 1, L"A");
                g_model.x_temp = ue->x - 5;
                g_model.y_temp = ue->y - 2;
                x_start_origin = 4 * blocks[g_model.id].x + 1;
                y_start_origin = 2 * blocks[g_model.id].y + 1;

                if (blocks[g_model.id].direction == 0) // 横向，只能左右滑
                {
                    x_end_origin = x_start_origin + 4 * blocks[g_model.id].length;
                    g_model.delta_x = g_model.x_temp - g_model.x_press;

                    if (g_model.delta_x >= 0 && g_model.delta_x <= g_model.right_max) // 往右拖
                    {
                        g_model.max_step = block_detect(x_end_origin + g_model.delta_x, y_start_origin, RIGHT);

                        if (g_model.max_step > 0 && g_model.right_lock == 0)
                        {
                            set_position(g_model.id, x_start_origin + g_model.delta_x, y_start_origin);
                            draw_blocks();
                        }
                    }
                    else if (g_model.delta_x <= 0 && g_model.delta_x >= g_model.left_max) // 往左拖
                    {
                        g_model.max_step = block_detect(x_start_origin + g_model.delta_x, y_start_origin, LEFT);

                        if (g_model.max_step < 0 && g_model.left_lock == 0)
                        {
                            set_position(g_model.id, x_start_origin + g_model.delta_x, y_start_origin);
                            draw_blocks();
                        }
                    }
                }
                if (blocks[g_model.id].direction == 1) // 纵向，只能上下滑
                {
                    y_end_origin = y_start_origin + 2 * blocks[g_model.id].length;
                    g_model.delta_y = g_model.y_temp - g_model.y_press;

                    if (g_model.delta_y >= 0 && g_model.delta_y <= g_model.down_max) // 往下拖
                    {
                        g_model.max_step = block_detect(x_start_origin, y_end_origin + g_model.delta_y, DOWN);

                        if (g_model.max_step > 0 && g_model.down_lock == 0)
                        {
                            set_position(g_model.id, x_start_origin, y_start_origin + g_model.delta_y);
                            draw_blocks();
                        }
                    }
                    else if (g_model.delta_y <= 0 && g_model.delta_y >= g_model.up_max) // 往上拖
                    {
                        g_model.max_step = block_detect(x_start_origin, y_start_origin + g_model.delta_y, UP);
                        if (g_model.max_step < 0 && g_model.up_lock == 0)
                        {
                            set_position(g_model.id, x_start_origin, y_start_origin + g_model.delta_y);
                            draw_blocks();
                        }
                    }
                }
            }

            // 0x40000: 按下过程中释放
            //if (ue->mouse_bstate == 0x40000)
            if (ue->mouse_bstate == BUTTON1_RELEASED && g_model.auto_mode == 0 && g_model.success == 0)
            {
                button1_down = 0;
                //cge_box_mvprintf(game_region, 0, 0, 1, L"A");
                g_model.x_release = ue->x - 5;
                g_model.y_release = ue->y - 2;

                if (g_model.id >= 0)
                {
                    int dx = g_model.x_release - g_model.x_press;
                    int dy = g_model.y_release - g_model.y_press;
                    move_state = 0;
                    if (blocks[g_model.id].direction == 0) // 横向
                    {
                        if(dx >= 2 && dx < 6 && dx <= g_model.right_max)
                        {
                            move_state = 1;
                        }
                        else if(dx >= 6 && dx < 10 && dx <= g_model.right_max)
                        {
                            move_state = 2;
                        }
                        else if(dx >= 10 && dx < 14 && dx <= g_model.right_max)
                        {
                            move_state = 3;
                        }
                        else if(dx >= 14 && dx < 18 && dx <= g_model.right_max)
                        {
                            move_state = 4;
                        }
                        else if(dx > g_model.right_max)
                        {
                            move_state = g_model.right_max / 4;
                        }

                        else if(dx <= -2 && dx > -6 && dx >= g_model.left_max)
                        {
                            move_state = -1;
                        }
                        else if(dx <= -6 && dx > -10 && dx >= g_model.left_max)
                        {
                            move_state = -2;
                        }
                        else if(dx <= -10 && dx > -14 && dx >= g_model.left_max)
                        {
                            move_state = -3;
                        }
                        else if(dx <= -14 && dx > -18 && dx >= g_model.left_max)
                        {
                            move_state = -4;
                        }
                        else if(dx < g_model.left_max)
                        {
                            move_state = g_model.left_max / 4;
                        }
                    }
                    
                    else if (blocks[g_model.id].direction == 1) // 纵向
                    {
                        if(dy >= 2 && dy < 4 && dy <= g_model.down_max)
                        {
                            move_state = 1;
                        }
                        else if(dy >= 4 && dy < 6 && dy <= g_model.down_max)
                        {
                            move_state = 2;
                        }
                        else if(dy >= 6 && dy < 8 && dy <= g_model.down_max)
                        {
                            move_state = 3;
                        }
                        else if(dy >= 8 && dy < 10 && dy <= g_model.down_max)
                        {
                            move_state = 4;
                        }
                        else if(dy > g_model.down_max)
                        {
                            move_state = g_model.down_max / 2;
                        }

                        if(dy <= -2 && dy > -4 && dy >= g_model.up_max)
                        {
                            move_state = -1;
                        }
                        else if(dy <= -4 && dy > -6 && dy >= g_model.up_max)
                        {
                            move_state = -2;
                        }
                        else if(dy <= -6 && dy > -8 && dy >= g_model.up_max)
                        {
                            move_state = -3;
                        }
                        else if(dy <= -8 && dy > -10 && dy >= g_model.up_max)
                        {
                            move_state = -4;
                        }
                        else if(dy < g_model.up_max)
                        {
                            move_state = g_model.up_max / 2;
                        }
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