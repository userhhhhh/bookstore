#ifndef CODE_BLOCKCHAIN_H
#define CODE_BLOCKCHAIN_H

// 这个文件的目的是实现一个文件存储类。
// 块状链表中，每个节点是一个NODE类,下面是NODE里面的东西
// NODE1：里面存储一个数组，数组元素是两个东西，一个string类型的index，一个int类型的value，还有排序方式
// NODE2：节点之间的连接不是通过指针，而是通过在文件中的位置，是一个整型数
// NODE3：块状链表的节点应该有两个特征值，一个是下界，一个是上界，便于在查找时快速锁定所在区间

#include <bits/stdc++.h>
const int NUM=1010;
//节点中的元素
class Element{
public:
    char index[70]={'\0'};
    int value;
public:
    Element(std::string);
    Element()=default;
    bool operator<(const Element& e2)const;
    bool operator==(const Element& e2)const;
    bool operator>(const Element& e2)const;
};
//链表的每个节点
class NODE{
public:
    Element low;
    Element high;
    int pos; //记录当前节点的初始位置
    int prev;//记录前一个节点的位置
    int next;//记录下一个节点的位置
    int size;//数组的大小
    Element a[NUM];
    NODE()=default;
};
//对块状链表操作的过程
class BlockChain{
private:
    std::fstream file;
    std::string file_name;
public:
    bool flag=false;
    int tail=0;
    BlockChain(std::string);  
    BlockChain()=default;
    ~BlockChain();
    void Insert(Element&);
    void Delete(Element&);
    void Find(std::string&);
    void writeNode(NODE&,int);
    void merge(NODE&);
    void borrow(NODE&,NODE&);
};

#endif //CODE_BLOCKCHAIN_H
