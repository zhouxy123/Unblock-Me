#include <iostream>
#include <cstdlib>
#include <vector>

#ifndef _UNBLOCK_H
#define _UNBLOCK_H
using namespace std;

class Block
{
public:
    int id;
    int x;
    int y;
    int length;    // 2 or 3
    int direction; // 0:横  1:竖

    Block(int ID, int X, int Y, int len, int dir)
    {
        this->id = ID;
        this->x = X;
        this->y = Y;
        this->length = len;
        this->direction = dir;
    }
    void move(int step);
};

// 每一步包含以下信息
class MOVE_STEP
{
public:
    int id;         // 块编号
    int move_state; // 移动步数

    MOVE_STEP(int ID, int State)
    {
        this->id = ID;
        this->move_state = State;
    }
};

extern vector<Block> blocks;
extern vector<MOVE_STEP> steps;
extern char box_record[27][15];
extern vector<vector<int> > status_group;
extern vector<Block> temp_blocks;
extern int block_num;

void blocks_init(char *level_id);
int detect_exceed(Block b1);
int detect_overlap(Block b1, Block b2);
int if_valid();
int get_block_id(int x, int y);
int if_successful();

void clear_record();
void record_one_block(Block block, int draw_x_start, int draw_y_start);
void record_corner(Block block, int draw_x_start, int draw_y_start);
void record_blocks();
void set_position(int id, int set_x_start, int set_y_start);
int block_detect(int x, int y, int dir);

vector<int> get_status();
int cmp_status(vector<int> status1, vector<int> status2);
int if_new_status(vector<int> status);
int try_move(int id, int move_state);
void save_status();
void reset();
void auto_calculate();

#endif