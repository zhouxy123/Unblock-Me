#include <iostream>
#include <cstdlib>
#include <vector>
#include "unblock.h"
#include <fstream>
#include <cstring>
using namespace std;

vector<Block> blocks;
//全局变量：.cpp中定义，.h中声明(extern)

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

// 初始化所有块
void blocks_init(char *level_id)
{
    ifstream ifs;
    ifs.open("./data/leveldata.txt",ios::in);
    //对于可执行文件的相对路径
 
    if (!ifs.is_open())
    {
        cout << "read fail." << endl;
    }
  
    char buf[5] = {0};
    
    int i=0;
    int block_num;
    while (ifs >> buf)
    {
        if(strcmp(buf, level_id)==0)
        {
            ifs >> buf;
            block_num = atoi(buf);
            while(i < block_num)
            {
                ifs >> buf;
                Block block(i, buf[0]-'0', buf[1]-'0', buf[2]-'0', buf[3]-'0');
                blocks.push_back(block);
                //cout << buf << endl;
                i++;
            }
            break;
        }
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
int if_valid()
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
