#include "cge.h"

CGE_USER_EVENT cge_events[32];
int cge_events_count;
unsigned long cge_stage;

//调色板可以在这里修改
int cge_color_pattern[16][2] = {
    {COLOR_WHITE, COLOR_BLACK},
    {COLOR_WHITE, COLOR_BLUE},
    {COLOR_WHITE, COLOR_RED},
    {COLOR_WHITE, COLOR_CYAN},
    {COLOR_WHITE, COLOR_GREEN},
    {COLOR_WHITE, COLOR_YELLOW},
    {COLOR_WHITE, COLOR_MAGENTA},
    {COLOR_BLACK, COLOR_WHITE},
    {COLOR_BLUE, COLOR_BLACK},
    {COLOR_RED, COLOR_BLACK},
    {COLOR_CYAN, COLOR_BLACK},
    {COLOR_GREEN, COLOR_BLACK},
    {COLOR_YELLOW, COLOR_BLACK},
    {COLOR_MAGENTA, COLOR_BLACK},
    {COLOR_MAGENTA, COLOR_BLACK},
    {COLOR_MAGENTA, COLOR_BLACK}
};

//调用用户的render方法，然后统一绘制主缓冲
void _cge_render(double dt) {
    if(dt<=0) return;
    //清屏
    erase();
    //调用用户的render...
    cge_render(dt);
    //绘制主缓冲，参考cge_box.c
    _cge_box_render_main();
}

//每帧执行
void _cge_schedule_update(double dt) {
    //更新定时器，参考cge_timer.c
    _cge_timer_update();
    cge_update(dt);
    _cge_render(dt);
    cge_stage++;
}

//执行终端UI相关初始化
void cge_init_term() {
    setlocale(LC_ALL,"");
    initscr();
    cbreak();
    nonl();
    noecho();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    curs_set(0);
    //初始化调色板
    if(has_colors()) {
        start_color();
        for(int i=0; i<16; i++) 
            init_pair(i+1,
                      cge_color_pattern[i][0],
                      cge_color_pattern[i][1]);
    }
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, 0);

    //这个是记录鼠标移动事件的指令,ncurses里没有对应的函数
    //直接调用控制指令
    printf("\033[?1003h\n");

    //注册cge_boxes[0]，用于绘制缓冲
    cge_box_register(MAX_WIDTH, MAX_HEIGHT, 0, 0);
}

//主循环
void cge_main_loop() {
    int ch;
    struct timeval tpstart,tpend;
    double tmpdt=0.0;

    cge_stage = 0;

    //开始时间
    gettimeofday(&tpstart,NULL);
    MEVENT event;
    //每秒FRAME次调用_cge_schedule_update
    do {
        //getch()不能设置成block模式，因为要同时驱动游戏1s60次Schedule循环
        //如果用timeout(0)把getch设置成nonblock,那cpu占用会过高
        //所以用timeout(1)控制getch的调用频率, 并结合gettimeofday来发起Schedule
        //getch内部会调用refresh，所以有了这个render里可以不用refresh()
        timeout(1);
        ch=getch();
        if(ch>0) {
            if(cge_events_count<MAX_USER_EVENT) {
                CGE_USER_EVENT *ge = &cge_events[cge_events_count];
                if(ch == KEY_MOUSE) {
                    getmouse(&event);
                    ge->type = CGE_MOUSE;
                    ge->mouse_bstate = event.bstate;
                    ge->x = event.x;
                    ge->y = event.y;
                } else {
                    ge->type = CGE_KEY;
                    ge->key = ch;
                }
                cge_events_count++;
            }
        }
        double dt;
        gettimeofday(&tpend,NULL);
        dt=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
        dt/=1000000;
        tpstart=tpend;
        tmpdt+=dt;
        if(tmpdt>=(1.0/FRAME)) {
            _cge_schedule_update(tmpdt);
            tmpdt=0.0;
        }
    } while(1);
}
