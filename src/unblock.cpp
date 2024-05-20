#include <iostream>
#include <cstdlib>
#include <vector>
#include "../include/unblock.h"
#include "../include/hash_lookup3.h"
#include <fstream>
#include <cstring>
#include <unordered_set>
using namespace std;

vector<Block> blocks;
vector<MOVE_STEP> steps;
vector<vector<int> > status_group;
unordered_set<uint32_t> status_hash;
vector<Block> temp_blocks;
int block_num = 0;

// vector<MOVE_STEP> branch;
char box_record[27][15] = {};
// 全局变量：.cpp中定义，.h中声明(extern)

// 移动（范围：-4～+4）
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
    ifs.open("./data/leveldata.txt", ios::in);
    // 对于可执行文件的相对路径

    if (!ifs.is_open())
    {
        cout << "read fail." << endl;
    }

    char buf[5] = {0};

    int i = 0;
    //int block_num = 0;
    while (ifs >> buf)
    {
        if (strcmp(buf, level_id) == 0)
        {
            ifs >> buf;
            block_num = atoi(buf);
            while (i < block_num)
            {
                ifs >> buf;
                Block block(i, buf[0] - '0', buf[1] - '0', buf[2] - '0', buf[3] - '0');
                blocks.push_back(block);
                //temp_blocks.push_back(block);
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

// 获取块编号
int get_block_id(int x, int y)
{
    int block_id = -1;
    int block_x_start, block_x_end, block_y_start, block_y_end;
    for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); it++)
    {
        block_x_start = 4 * (*it).x + 1;
        block_y_start = 2 * (*it).y + 1;
        if ((*it).direction == 0)
        {
            block_x_end = block_x_start + 4 * (*it).length;
            block_y_end = block_y_start + 2;
        }
        else
        {
            block_y_end = block_y_start + 2 * (*it).length;
            block_x_end = block_x_start + 4;
        }

        if ((x > block_x_start) && (x < block_x_end) && (y > block_y_start) && (y < block_y_end))
        {
            block_id = (*it).id;
            break;
        }
    }
    return block_id;
}

int if_successful()
{
    if (blocks[0].x == 4)
        return 1;
    else
        return 0;
}

// 清空box_record
void clear_record()
{
    int i, j;
    for (i = 1; i <= 25; i++)
    {
        for (j = 1; j <= 13; j++)
        {
            box_record[i][j] = ' ';
        }
    }
}

// 在box_record里画一个block的边界+内部
void record_one_block(Block block, int draw_x_start, int draw_y_start)
{
    char type;
    if (block.id == 0)
        type = '&'; // 代表主块
    else
        type = '#'; // 代表其他块

    int draw_x_end, draw_y_end;
    int i;

    if (block.direction == 0)
    {
        draw_x_end = draw_x_start + 4 * block.length;
        draw_y_end = draw_y_start + 2;
        box_record[draw_x_start][draw_y_start + 1] = '|';
        box_record[draw_x_end][draw_y_start + 1] = '|';
        for (i = draw_x_start; i <= draw_x_end; i++)
        {
            box_record[i][draw_y_start] = '=';
            box_record[i][draw_y_end] = '=';
        }

        for (i = draw_x_start + 1; i < draw_x_end; i++)
        {
            box_record[i][draw_y_start + 1] = type;
        }
    }

    else
    {
        draw_x_end = draw_x_start + 4;
        draw_y_end = draw_y_start + 2 * block.length;
        for (i = draw_x_start; i <= draw_x_end; i++)
        {
            box_record[i][draw_y_start] = '=';
            box_record[i][draw_y_end] = '=';
        }
        for (i = draw_y_start; i <= draw_y_end; i++)
        {
            box_record[draw_x_start][i] = '|';
            box_record[draw_x_start + 4][i] = '|';
        }
        for (i = draw_y_start + 1; i < draw_y_end; i++)
        {
            box_record[draw_x_start + 1][i] = '#';
            box_record[draw_x_start + 2][i] = '#';
            box_record[draw_x_start + 3][i] = '#';
        }
    }
}

// 在box_record里画一个block的四个角
void record_corner(Block block, int draw_x_start, int draw_y_start)
{
    int draw_x_end, draw_y_end;
    if (block.direction == 0)
    {
        draw_x_end = draw_x_start + 4 * block.length;
        draw_y_end = draw_y_start + 2;
    }
    else
    {
        draw_x_end = draw_x_start + 4;
        draw_y_end = draw_y_start + 2 * block.length;
    }
    box_record[draw_x_start][draw_y_start] = '+';
    box_record[draw_x_end][draw_y_start] = '+';
    box_record[draw_x_start][draw_y_end] = '+';
    box_record[draw_x_end][draw_y_end] = '+';
}

// 在box_record里画所有块
void record_blocks()
{
    int draw_x_start, draw_y_start; // 实际画图的x,y

    // 先清空
    clear_record();

    for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); it++)
    {

        draw_x_start = 4 * (*it).x + 1;
        draw_y_start = 2 * (*it).y + 1;
        record_one_block((*it), draw_x_start, draw_y_start);
    }

    for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); it++)
    {

        draw_x_start = 4 * (*it).x + 1;
        draw_y_start = 2 * (*it).y + 1;
        record_corner((*it), draw_x_start, draw_y_start);
    }
}

// 手动设置某个块的位置（用于鼠标拖动）
void set_position(int id, int set_x_start, int set_y_start)
{
    int draw_x_start, draw_y_start;
    // 先清空
    clear_record();

    for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); it++)
    {
        if ((*it).id != id)
        {
            draw_x_start = 4 * (*it).x + 1;
            draw_y_start = 2 * (*it).y + 1;
        }
        else
        {
            draw_x_start = set_x_start;
            draw_y_start = set_y_start;
        }
        record_one_block((*it), draw_x_start, draw_y_start);
    }
    for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); it++)
    {
        if ((*it).id != id)
        {
            draw_x_start = 4 * (*it).x + 1;
            draw_y_start = 2 * (*it).y + 1;
        }
        else
        {
            draw_x_start = set_x_start;
            draw_y_start = set_y_start;
        }
        record_corner((*it), draw_x_start, draw_y_start);
    }
}

// 检测上下左右有多少空间 （重新写）
int block_detect(int x, int y, int dir)
{
    int i;
    int max_step = 0;
    if (dir == 0) // 最多往右挪几个格
    {
        max_step = 25 - x;
        for (i = x + 1; i <= 25; i++)
        {
            if (box_record[i][y + 1] == '#' || box_record[i][y + 1] == '&')
            {
                max_step = i - x - 1;
                break;
            }
        }
    }
    if (dir == 1) // 最多往左挪几个格
    {
        max_step = 1 - x;
        for (i = x - 1; i >= 1; i--)
        {
            if (box_record[i][y + 1] == '#' || box_record[i][y + 1] == '&')
            {
                max_step = i - x + 1;
                break;
            }
        }
    }
    if (dir == 2) // 最多往下挪几个格
    {
        max_step = 13 - y;
        for (i = y + 1; i <= 13; i++)
        {
            if (box_record[x + 1][i] == '#' || box_record[x + 1][i] == '&')
            {
                max_step = i - y - 1;
                break;
            }
        }
    }
    if (dir == 3) // 最多往上挪几个格
    {
        max_step = 1 - y;
        for (i = y - 1; i >= 1; i--)
        {
            if (box_record[x + 1][i] == '#' || box_record[x + 1][i] == '&')
            {
                max_step = i - y + 1;
                break;
            }
        }
    }
    return max_step;
}

// 获取状态
vector<int> get_status()
{
    vector<int> status;
    for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); it++)
    {
        status.push_back((*it).x);
        status.push_back((*it).y);
    }
    return status;
}

// 判断重复状态 (1有重复，0无重复)
// 利用哈希表重写
int if_new_status(vector<int> status)
{
    // 利用hashlittle计算数组status的哈希值，判断一个状态是否出现过
    uint32_t hash = hashlittle(status.data(), status.size() * sizeof(int), 123456);
    if(status_hash.count(hash)) // 与之前某一状态相同
    {
        return 1;
    }
    status_hash.insert(hash);
    return 0;
}

// 尝试移动
/*返回值：
0-产生新状态，可开始操作下一个块
1-成功
2-不产生新状态，需要考虑对这个块的其他操作
3-操作不合理，需要考虑对这个块的其他操作
*/
int try_move(int id, int move_state)
{
    // blocks[id].move(move_state);
    vector<int> status;
    int result = 0;
    int if_valid_move = 1;
    int i;

    if (move_state > 1)
    {
        for (i = 1; i <= move_state; i++)
        {
            blocks[id].move(i);
            if (if_valid() != 1)
            {
                if_valid_move = 0;
                blocks[id].move(-1 * i);
                break;
            }
            blocks[id].move(-1 * i);
        }
    }

    if (move_state < -1)
    {
        for (i = -1; i >= move_state; i--)
        {
            blocks[id].move(i);
            if (if_valid() != 1)
            {
                if_valid_move = 0;
                blocks[id].move(-1 * i);
                break;
            }
            blocks[id].move(-1 * i);
        }
    }

    if (move_state == -1 || move_state == 1)
    {
        blocks[id].move(move_state);
        // cout<< "state: (" << blocks[id].x << "," << blocks[id].y << ")" << endl;
        if (if_valid() != 1)
        {
            if_valid_move = 0;
        }
        blocks[id].move(-1 * move_state);
    }

    if (if_valid_move == 1)
    {
        blocks[id].move(move_state);

        status = get_status();
        
        // cout<<"if new status "<<if_new_status(status)<<endl;
        if (if_new_status(status) == 0) // 新状态
        {
            //uint32_t hash = hashlittle(status.data(), status.size(), 123456);
            //status_hash.insert(hash);
            status_group.push_back(status);
            MOVE_STEP step(id, move_state);
            steps.push_back(step);
            // cout << "(" << id << "," << move_state << ")" << endl;
            if (if_successful())
            {
                result = 1;
                return 1;
            }

            return 0;
        }
        else
        {
            blocks[id].move(-1 * move_state);
            result = 2;
        }
    }
    else
    {
        // blocks[id].move(-1 * move_state);
        result = 3;
    }
    return result;
}

void save_status()
{
    for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); it++)
    {
        temp_blocks.push_back(*it);
    }
}

void reset()
{
    int i = 0;
    for (vector<Block>::iterator it = temp_blocks.begin(); it != temp_blocks.end(); it++)
    {
        blocks[i].id = (*it).id;
        blocks[i].x = (*it).x;
        blocks[i].y = (*it).y;
        blocks[i].direction = (*it).direction;
        blocks[i].length = (*it).length;
        i++;
    }
}

// 自动计算
void auto_calculate()
{
    vector<int> status;
    status = get_status();          // 获取初始状态
    status_group.push_back(status); // 存入状态组中
    uint32_t hash = hashlittle(status.data(), status.size() * sizeof(int), 123456);
    status_hash.insert(hash);       // 储存初始状态哈希值
    
    int end = 0;
    int result;
    int i;
    int count = 0;
    int fail_count = 0;
    int error_count = 0;

    save_status(); // 开始自动计算之前先储存当前状态
    while (end == 0)
    {
        /*if (end == 1)
        {
            break;
        }*/

        if (fail_count >= block_num * 8)
        {
            blocks[steps.back().id].move(steps.back().move_state * (-1));
            steps.pop_back();
            fail_count = 0;
            error_count ++;
        }


        for (vector<Block>::iterator it = blocks.begin(); it != blocks.end(); it++)
        {
            if (end == 1)
            {
                break;
            }
            int a[8] = {4, -4, 3, -3, 2, -2, 1, -1};
            for (i = 0; i <= 7; i++)
            {
                result = try_move((*it).id, a[i]);
                //cout<<result<<endl;
                count++;

                if(try_move(0, 4-blocks[0].x) != 3)
                {
                    end = 1;
                    fail_count = 0;
                    reset();
                    break;
                }

                if (result == 0) // 0-产生新状态，可开始操作下一个块
                {
                    fail_count = 0;
                    break;
                }

                if (result == 1) // 1-成功
                {
                    end = 1;
                    fail_count = 0;
                    reset();
                    break;
                }

                if (result == 2) // 2-不产生新状态
                {
                    fail_count++;
                    continue;
                }

                if (result == 3) // 3-操作不合理，需要考虑对这个块的其他操作
                {
                    fail_count++;
                    continue;
                }
            }
            if (fail_count >= block_num * 8)
            {
                //死循环
                //cout<<"fail"<<endl;
                break;
            }
        }
    }
}

