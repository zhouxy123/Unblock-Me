#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <wchar.h>
#include <ncurses.h>
#include <locale.h>

#ifndef _CGE_H
#define _CGE_H

//每秒帧数
#define FRAME 60
//用户输入事件缓冲区长度
#define MAX_USER_EVENT 32
//定时器最多个数
#define MAX_TIMER 128
//事件发射器最多个数
#define MAX_EMITTER 64
//每个事件最多的订阅者个数
#define MAX_OBSERVER 128

//用户输入事件类型
typedef enum {
    CGE_MOUSE = 1,
    CGE_KEY = 2
} CGE_USER_EVENT_TYPE;

//用户输入事件结构
typedef struct cge_user_event {
    CGE_USER_EVENT_TYPE type;
    mmask_t mouse_bstate;
    int key;
    int x;
    int y;
} CGE_USER_EVENT;

//全局用户输入事件
extern CGE_USER_EVENT cge_events[MAX_USER_EVENT];
extern int cge_events_count;

//定时器回调函数
typedef void (*timer_handle)();
//定时器结构
typedef struct cge_timer {
    int stage;
    int count;
    timer_handle endcall;
    void *exdata;
} CGE_TIMER;

//全局定时器
extern CGE_TIMER cge_timers[MAX_TIMER];
extern int cge_timer_count;

//事件回调函数
typedef void (*observer_handle)(void* msg);
//事件发射器结构
typedef struct cge_emitter {
    observer_handle observers[MAX_OBSERVER];
    int observer_count;
} CGE_EMITTER;

//全局事件发射器
extern CGE_EMITTER cge_emitters[MAX_EMITTER];
extern int cge_emitter_count;

//文本图像boxes...
//为了简化代码，采用静态全局变量
//最多支持MAX_BOX个box对象
#define MAX_WIDTH 80
#define MAX_HEIGHT 25
#define MAX_BOX 1024
#define MAX_IMG 512

typedef struct cge_ascii_img {
    int width, height;
    int x, y;
    cchar_t content[MAX_HEIGHT][MAX_WIDTH];
} CGE_ASCII_IMG;

typedef struct cge_box {
    //采用静态结构，没有采用指针，也是为了更安全
    CGE_ASCII_IMG img;
    int x, y;
    //负数表示隐藏不绘制,正数越大表示越在上层
    //同样权重按数组中的次序绘制
    int render_weight;
} CGE_BOX;

extern CGE_ASCII_IMG cge_imgs[MAX_IMG];
extern int cge_img_count;
extern CGE_BOX cge_boxes[MAX_BOX];
extern int cge_box_count;

//cge函数
void cge_init_term();
void cge_main_loop();
void cge_init();

//游戏的更新和绘制逻辑在这两个函数中实现
void cge_update(double dt);
void cge_render(double dt);

//下划线开头表示内部函数，使用者不应该调用
//内部update，由main_loop调用
void _cge_schedule_update(double dt);

//定时器相关函数
void _cge_timer_update();
int  cge_timer_register(double t, timer_handle func);
void cge_timer_fire(int timer_id, void *exdata);
double cge_timer_getpercent(int timer_id);
void *cge_timer_getexdata(int timer_id);

//事件发射器相关函数
int cge_emitter_register();
int cge_emitter_addob(int emitter_id, observer_handle func);
void cge_emitter_rmob(int emitter_id, observer_handle func);
void cge_emitter_notify(int emitter_id, void *msg);

//文本图相关函数
int cge_box_register(int width, int height, int x, int y);
void cge_box_mvprintf(int box_id, int x, int y, int color, const wchar_t *fmt, ...); 
int cge_box_loadimg(const char *imgpath);
void cge_box_setimg(int box_id, int img_id);
void _cge_box_render_buf(CGE_BOX *box);
void _cge_box_render_main();

#endif
