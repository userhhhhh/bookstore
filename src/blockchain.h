#ifndef CODE_BLOCKCHAIN_H
#define CODE_BLOCKCHAIN_H
#include <bits/stdc++.h>

const int NUM = 6010;

class Element {
public:
    char index[70] = {'\0'};
    int value;
public:
    Element(std::string);
    Element() = default;
    bool operator<(const Element &e2) const;
    bool operator==(const Element &e2) const;
    bool operator>(const Element &e2) const;
};

//链表的每个节点
class NODE {
public:
    int size;//数组的大小
    Element a[NUM];

    NODE() = default;
};
class NODE_INDEX {
public:
    Element low;
    Element high;
    int pos;//表示在另一个文件中，NODE的位置
    int next_index=-1; //用来将 NODE_INDEX 连起来
    int last_index=-1; 
    int pos_index;
};
//对块状链表操作的过程
class BlockChain {
private:
    std::fstream file;
    std::string file_name;
    std::fstream file_index;
    std::string file_name_index;
    std::fstream file_tail;
    std::string file_name_tail;
public:
    bool flag = false;
    int tail = 0; //表示在b.txt中最后一个块的块头

    BlockChain(std::string,std::string,std::string );
    BlockChain() = default;
    ~BlockChain();
    void Insert(Element &);
    void Delete(Element &);
    std::vector<int> Find(std::string &);
    void writeNode(NODE &, int);
    void writeNodeIndex(NODE_INDEX &,int);
    void split(NODE &,NODE_INDEX &);
    void merge(NODE &,NODE_INDEX &,NODE &,NODE_INDEX &);
    void borrow(NODE &,NODE_INDEX &,NODE &,NODE_INDEX &);
    void remove(NODE &);
};

#endif //CODE_BLOCKCHAIN_H