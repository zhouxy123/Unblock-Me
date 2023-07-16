#include "../cge/cge.h"


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

//字符
static wchar_t ascii[8][29] = {
    L" !\"#$%&'()*+,-./0123456789:;",
    L"<=>?@ABCDEFGHIJKLMNOPQRSTUVW",
    L"XYZ[\\]^_`abcdefghijklmnopqrs",
    L"tuvwxyz{|}~ÇüéâäàåçêëèïîìÄÅÉ",
    L"æÆôöòûùÿÖÜ¢£¥₧ƒáíóúñÑªº¿⌐¬½¼",
    L"¡«»░▒▓│┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚",
    L"╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀αßΓπΣ",
    L"σµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²■"};

//定时器ID
int g_moving_timer;

//事件发射器
int g_emitter;

// test box...
int g_box_test;
int g_box_ascii;
int g_box_color;
int line;
int draw_region;

typedef struct pen
{
    int color;
    wchar_t type;
} PEN;
PEN g_pen;

//更新数据
void cge_update(double dt)
{
    if (dt <= 0)
        return;
    //消费所有的用户输入事件
    for (int i = 0; i < cge_events_count; i++)
    {
        CGE_USER_EVENT *ue = &cge_events[i];

        if (g_model.state == NORMAL && ue->type == CGE_MOUSE)
        {
            if (ue->x < 28 && ue->y < 8)
            {
                if (ue->mouse_bstate == 2 || ue->mouse_bstate == 4)
                    g_pen.type = ascii[ue->y][ue->x];
            }
            if (ue->x < 16 && ue->y == 10)
            {
                if (ue->mouse_bstate == 2 || ue->mouse_bstate == 4)
                    g_pen.color = ue->x + 1;
            }
            else
            {
                if (ue->mouse_bstate == 2 || ue->mouse_bstate == 4 || ue->mouse_bstate == 0x80000)
                {
                    g_model.x = ue->x;
                    g_model.y = ue->y;
                    if (g_model.y < 25 && g_model.x < 80 && g_model.x >= 29)
                    {
                        cge_box_mvprintf(draw_region, g_model.x, g_model.y, g_pen.color, L"%lc", g_pen.type);
                    }
                }
            }
        }
    }
    cge_events_count = 0;
}

void cge_render(double dt)
{
}

//定时器走完后的回调函数
void timer_end_moving()
{
    g_model.state = NORMAL;
    int ed = (int)(size_t)cge_timer_getexdata(g_moving_timer);
    g_model.x = ed;
    cge_emitter_notify(g_emitter, (void *)L"BB");
}

//测试事件
void emit_handle(void *m)
{
    cge_box_mvprintf(g_box_test, 2, 2, 2, (const wchar_t *)m);
}

//初始化
void init()
{
    int i;
    g_model.state = NORMAL;
    cge_init_term();

    g_box_ascii = cge_box_register(28, 8, 0, 0);
    for (i = 0; i < 8; i++)
    {
        cge_box_mvprintf(g_box_ascii, 0, i, 1, ascii[i]);
    }

    draw_region = cge_box_register(80, 25, 0, 0);

    g_box_color = cge_box_register(16, 1, 0, 10);
    for (i = 0; i < 16; i++)
    {
        cge_box_mvprintf(g_box_color, i, 0, i + 1, L"a");
    }

    line = cge_box_register(1, 25, 28, 0);
    for (i = 0; i < 25; i++)
    {
        cge_box_mvprintf(line, 0, i, 1, L"|");
    }

    //测试事件触发
    g_emitter = cge_emitter_register();
    cge_emitter_addob(g_emitter, emit_handle);
}

int main()
{
    init();
    cge_main_loop();
    return 0;
}