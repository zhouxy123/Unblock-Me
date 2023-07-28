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

extern vector<Block> blocks;

void blocks_init(char *level_id);
int detect_exceed(Block b1);
int detect_overlap(Block b1, Block b2);
int if_valid();
int get_block_id(int x, int y);
int if_successful();

#endif