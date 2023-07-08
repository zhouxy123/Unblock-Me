#include "log.h"
#include "cge.h"

//游戏状态
typedef enum game_state {
    NORMAL,
    MOVING
} GAME_STATE;

//全局数据
typedef struct game_model {
    GAME_STATE state;
    int x, y;
} GAME_MODEL;
GAME_MODEL g_model;

//定时器ID
int g_moving_timer;

//事件发射器
int g_emitter;

//test box...
int g_box_test;
int g_box_test2;

//bool g_mouse_press;

//更新数据
void cge_update(double dt) {
    if(dt<=0) return;
    //消费所有的用户输入事件
    for(int i=0; i<cge_events_count; i++) {
        CGE_USER_EVENT *ue = &cge_events[i];
        if(ue->type == CGE_MOUSE) {
            log_error("%lx %lx %lx %lx log...%lx\n", 
                      BUTTON1_CLICKED, BUTTON1_PRESSED, 
                      BUTTON1_RELEASED, REPORT_MOUSE_POSITION, 
                      ue->mouse_bstate);
            //留下这三种事件就可以了
            if(ue->mouse_bstate == 2 || ue->mouse_bstate == 4 || ue->mouse_bstate == 0x80000) {
                g_model.x = ue->x;
                g_model.y = ue->y;
                cge_box_mvprintf(g_box_test2, g_model.x, g_model.y, 8, L"+");
            }
            /*
             * 以下代码是为了打印跟踪各种鼠标事件的bstate值
             * 迅速点击左键，触发CLICKED：4
             * 按下不放过一会就会触发，PRESSED：2
             * 松开会触发RELEASED: 1 (和PRESSED成对出现)
             * 鼠标不按键移动，触发REPORT_MOUSE_POSITION: 0x8000000
             * 按下左键移动，产生的是 0x80000 不知道是啥
             * 移动时释放左键，产生 0x40000 不知道是啥
             * 有时候还会产生 0x100000 不知道是啥
            //按住左键过一会儿就会触发一个PRESSED
            if(ue->mouse_bstate & BUTTON1_PRESSED) {
                g_mouse_press = true;
                log_error("press set gmp to true\n");
            }
            //释放左键
            if(ue->mouse_bstate & BUTTON1_RELEASED) {
                g_mouse_press = false;
                log_error("release set gmp to false\n");
            }
            //移动过程中释放
            if(ue->mouse_bstate == 0x40000) {
                g_mouse_press = false;
                log_error("release400 set gmp to false\n");
            }
            //点击一下左键马上释放
            if(ue->mouse_bstate & BUTTON1_CLICKED) {
                g_mouse_press = true;
                log_error("clicked set gmp to true\n");
            }
            if(g_mouse_press) {
                g_model.x = ue->x;
                g_model.y = ue->y;
                cge_box_mvprintf(g_box_test2, g_model.x, g_model.y, 8, L"+");
            }
            if(ue->mouse_bstate & BUTTON1_CLICKED) {
                g_mouse_press = false;
                log_error("clicked set gmp to false\n");
            }*/
        }
    }
    cge_events_count = 0;
}

//飞行绘制
void cge_render(double dt) {
    if(dt<=0) return;
    if(g_model.state != MOVING) return;
    double tp = cge_timer_getpercent(g_moving_timer);
    attron(COLOR_PAIR(10));
    mvprintw(g_model.y, g_model.x + 10 - ceil(10.0 * tp), "你好中国...");
    attroff(COLOR_PAIR(10));
    CGE_BOX *cb = &cge_boxes[g_box_test];
    cb->x = 10 - ceil(10.0 * tp);
}

//定时器走完后的回调函数
void timer_end_moving() {
    g_model.state = NORMAL;
    int ed = (int)(size_t)cge_timer_getexdata(g_moving_timer);
    g_model.x = ed;
    cge_emitter_notify(g_emitter, (void *)L"BB");
}

//测试事件
void emit_handle(void *m) {
    cge_box_mvprintf(g_box_test, 2, 2, 2, (const wchar_t *)m);
}

//初始化
void init() {
    FILE *fp = fopen("game.log", "w+");
    if(fp) log_add_fp(fp, LOG_DEBUG);
    log_set_quiet(true);

    g_model.state = NORMAL;
    cge_init_term();
    g_moving_timer = cge_timer_register(0.5, timer_end_moving);

    //g_mouse_press = false;

    //测试box
    g_box_test = cge_box_register(6, 5, 0, 0);
    cge_box_mvprintf(g_box_test, 0, 0, 8, L"+----+");
    cge_box_mvprintf(g_box_test, 0, 1, 8, L"|    |");
    cge_box_mvprintf(g_box_test, 0, 2, 8, L"| ╢╖ |");
    cge_box_mvprintf(g_box_test, 0, 3, 8, L"|    |");
    cge_box_mvprintf(g_box_test, 0, 4, 8, L"+----+");

    g_box_test2 = cge_box_register(80, 25, 0, 0);
    //cge_box_mvprintf(g_box_test2, 0, 0, 8, L"+----+");

    //测试事件触发
    g_emitter = cge_emitter_register();
    cge_emitter_addob(g_emitter, emit_handle);
}

int main() {
    init();
    cge_main_loop();
    return 0;
}

