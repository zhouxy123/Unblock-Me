#include <iostream>
#include <cstdlib>
#include <vector>
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

//移动（范围：-4～+4）
void Block::move(int step)
{
    // 横
    if (direction == 0)
        x += step;

    // 竖
    else
        y += step;
}

/*
成功：主块(编号为0)x=4，且处于valid状态（块之间无冲突）
*/
vector<Block> blocks;
void blocks_init();
int detect_exceed(Block b1);
int detect_overlap(Block b1, Block b2);
int if_valid(vector<Block> blocks);

int main()
{
    blocks_init();

    // 用迭代器遍历
    for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); it++)
    {
        cout << "block" << (*it).id << ": x=" << (*it).x << " y=" << (*it).y << " length=" << (*it).length << " direction=" << (*it).direction << endl;
    }

    cout << "block0: exceed:" << detect_exceed(blocks[0]) << endl;
    cout << "block1: exceed:" << detect_exceed(blocks[1]) << endl;
    cout << "block2: exceed:" << detect_exceed(blocks[2]) << endl;

    cout << "block1 and block2: overlap:" << detect_overlap(blocks[1], blocks[2]) << endl;

    cout << "global status: " << if_valid(blocks) << endl;
    /*
    Block block1(0, 0, 2, 0);
    Block block2(0, 1, 3, 0);
    Block block3(0, 3, 2, 1);
    Block block4(0, 5, 2, 0);
    Block block5(2, 3, 2, 0);
    */
    // block1.set(0, 0, 3, 1);
    // cout << "x:" << block1.x << " y:" << block1.y << " length:" << block1.length << " direction:" << block1.direction << endl;

    // block1.move(2);
    // cout << "x:" << block1.x << " y:" << block1.y << " length:" << block1.length << " direction:" << block1.direction << endl;
}

// 初始化所有块
void blocks_init()
{
    Block block0(0, 0, 2, 2, 0); // 主块：编号为0，初始位置(0,2)，长度为2，横向
    blocks.push_back(block0);
    int block_num;
    cout << "输入块数(除主块外):" << endl;
    cin >> block_num;
    int x, y, length, direction;
    for (int i = 1; i <= block_num; i++)
    {
        cout << "初始化block" << i << ": (x, y, length, direction)" << endl;
        cin >> x >> y >> length >> direction;
        Block block(i, x, y, length, direction);
        blocks.push_back(block);
    }
}

// 判断一个块是否超边界
int detect_exceed(Block b1)
{
    // 横
    if (b1.direction == 0)
    {
        if ((b1.x >= 0) && ((b1.x + b1.length - 1) <= 5) && (b1.y >= 0) && (b1.y <= 5))
            return 1;
        else
            return -1;
    }

    // 竖
    else
    {
        if ((b1.x >= 0) && ((b1.x) <= 5) && (b1.y >= 0) && ((b1.y + b1.length - 1) <= 5))
            return 1;
        else
            return -1;
    }
}

// 判断两个块之间是否重叠
int detect_overlap(Block b1, Block b2)
{
    int status = 1;
    // 全横
    if ((b1.direction == 0) && (b2.direction == 0))
    {
        // 不在同一水平线上
        if (b1.y != b2.y)
            status = 1;

        // 在同一水平线上，无重叠
        else if (((b1.x + b1.length - 1) < b2.x) || ((b2.x + b2.length - 1) < b1.x))
            status = 1;

        // 在同一水平线上，有重叠
        else
            status = -1;
    }

    // 全竖
    if ((b1.direction == 1) && (b2.direction == 1))
    {
        // 不在同一竖直线上
        if (b1.x != b2.x)
            status = 1;

        // 在同一竖直线上，无重叠
        else if (((b1.y + b1.length - 1) < b2.y) || ((b2.y + b2.length - 1) < b1.y))
            status = 1;

        // 在同一竖直线上，有重叠
        else
            status = -1;
    }

    int i, j;
    // 1横2竖
    if ((b1.direction == 0) && (b2.direction == 1))
    {
        for (i = 0; i < b1.length; i++)
        {
            for (j = 0; j < b2.length; j++)
            {
                if ((b1.x + i == b2.x) && (b1.y == b2.y + j))
                {
                    status = -1;
                }
            }
        }
    }

    // 1竖2横
    if ((b1.direction == 1) && (b2.direction == 0))
    {
        for (i = 0; i < b1.length; i++)
        {
            for (j = 0; j < b2.length; j++)
            {
                if ((b1.y + i == b2.y) && (b1.x == b2.x + j))
                {
                    status = -1;
                }
            }
        }
    }

    return status;
}

// 检验整体状态是否正确
int if_valid(vector<Block> blocks)
{
    int i, j;
    int status = 1;
    for (i = 0; i < blocks.size(); i++)
    {
        if (detect_exceed(blocks[i]) == -1)
            status = -1;
    }

    for (i = 0; i < blocks.size(); i++)
    {
        for (j = 0; j < blocks.size(); j++)
        {
            if (i == j)
            {
                j++;
            }

            if (detect_overlap(blocks[i], blocks[j]) == -1)
            {
                status = -1;
            }
        }
    }

    return status;
}

//熊胆至金方