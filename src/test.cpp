#include "../include/hash_lookup3.h"
#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;

int main() {
    
    vector<int> a;
    a.push_back(1);
    a.push_back(2);
    a.push_back(3);
    
    vector<int> b;
    b.push_back(4);
    b.push_back(5);
    b.push_back(6);
    //int b[3] = {1,2,3}; 
    // a.data()表示vector中数据部分的首地址（vector中数据也是连续的）
    uint32_t hash1 = hashlittle(a.data(), a.size(), 123456);
    uint32_t hash2 = hashlittle(b.data(), b.size(), 123456);
    cout<<hash1<<endl;
    cout<<hash2<<endl;
    cout<<"================="<<endl;
    int status[24] = {3,2,0,0,0,1,0,2,0,5,1,3,2,1,2,4,3,0,4,4,5,0,5,2};
    cout<<hashlittle(status, 24 * sizeof(int), 123456)<<endl;
    status[7] = 3;
    cout<<hashlittle(status, 24 * sizeof(int), 123456)<<endl;
    status[7] = 2;
    status[8] = 4;
    cout<<hashlittle(status, 24 * sizeof(int), 123456)<<endl;
    status[8] = 3;
    cout<<hashlittle(status, 24 * sizeof(int), 123456)<<endl;
    status[8] = 2;
    cout<<hashlittle(status, 24 * sizeof(int), 123456)<<endl;
    status[8] = 1;
    cout<<hashlittle(status, 24 * sizeof(int), 123456)<<endl;
    status[8] = 0;
    status[11] = 2;
    cout<<hashlittle(status, 24 * sizeof(int), 123456)<<endl;
    status[11] = 3;
    status[13] = 2;
    cout<<hashlittle(status, 24 * sizeof(int), 123456)<<endl;

    //unordered_set<uint32_t> s;
    //cout<<s.count(hash1)<<endl;

    /*
    32 00 01 02 05 13 21 24 30 44 50 52 (init)
    32 00 01 03 05 13 21 24 30 44 50 52
    32 00 01 02 45 13 21 24 30 44 50 52
    32 00 01 02 35 13 21 24 30 44 50 52
    32 00 01 02 25 13 21 24 30 44 50 52
    32 00 01 02 15 13 21 24 30 44 50 52
    32 00 01 02 05 12 21 24 30 44 50 52
    32 00 01 02 05 13 22 24 30 44 50 52
    */
    return 0;
}