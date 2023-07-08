#include "cge.h"

CGE_ASCII_IMG cge_imgs[MAX_IMG];
int cge_img_count;

CGE_BOX cge_boxes[MAX_BOX];
int cge_box_count;

//为box加边框，单勾，双勾，+-几种风格
void cge_box_border(int box_id, int color, int border_type) {
    //TODO
}

//从文件读取一个ascii img, 放入cge_imgs缓冲区中
int cge_box_loadimg(const char *imgpath) {
    //TODO
    return -1;
}

//将box的内容设置成img
void cge_box_setimg(int box_id, int img_id) {
    CGE_BOX *cb = &cge_boxes[box_id];
    memcpy(&cb->img, &cge_imgs[img_id], sizeof(CGE_ASCII_IMG));
}

//注册一个box,返回boxid
int cge_box_register(int width, int height, int x, int y) {
    if(width>MAX_WIDTH || height>MAX_HEIGHT) 
        return -1;
    if(cge_box_count>MAX_BOX)
        return -1;
    CGE_BOX *cb = &cge_boxes[cge_box_count];
    cb->img.width = width;
    cb->img.height = height;
    cb->x = x;
    cb->y = y;
    return cge_box_count++;
}

//将box中的内容设置到主缓冲中
void _cge_box_render_buf(CGE_BOX *box) {
    for(int i=0; i<box->img.height; i++) {
        for(int j=0; j<box->img.width; j++) {
            int x = j + box->x;
            int y = i + box->y;
            cchar_t c = box->img.content[i][j];
            if(x<MAX_WIDTH && y<MAX_HEIGHT && c.chars[0]!=0) 
                cge_boxes[0].img.content[y][x] = c;
        }
    }
}

//循环设置boxes到主缓冲，然后实际绘制主缓冲
void _cge_box_render_main() {
    CGE_BOX *main_buf = &cge_boxes[0];
    memset(main_buf->img.content, 0, sizeof(cchar_t) * MAX_WIDTH * MAX_HEIGHT);
    for(int i=0; i<cge_box_count - 1; i++) 
        _cge_box_render_buf(&cge_boxes[i+1]);
    for(int i=0; i<MAX_HEIGHT; i++) {
        for(int j=0; j<MAX_WIDTH; j++) {
            cchar_t *c = &main_buf->img.content[i][j];
            if(c->chars[0]) mvadd_wch(i, j, c);
        }
    }
}

//在box内按颜色设置一个字符串
void cge_box_mvprintf(int box_id, int x, int y, int color, const wchar_t *fmt, ...) {
    wchar_t tmp[MAX_WIDTH];
    CGE_BOX *cb = &cge_boxes[box_id];

    va_list arg_ptr;

    va_start(arg_ptr, fmt);
    vswprintf(tmp, cb->img.width + 1, fmt, arg_ptr);
    va_end(arg_ptr);

    int len = wcslen(tmp);
    wchar_t tt[2];
    for(int i=0; i<len; i++) {
        tt[0] = tmp[i];
        tt[1] = L'\0';
        setcchar(&cb->img.content[y][x + i], (wchar_t *)&tt, 0, color, NULL);
    }
}
