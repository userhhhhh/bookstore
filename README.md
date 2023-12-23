# 开发文档

主体分成三个部分：blockchain、booksystem、usersystem、log

## blockchain

### 成员变量

- 块状链表中的元素是Element类
- Element类：char*表示索引，value表示这个块在真正存储它的文件的索引
- NODE类：Element合在一起构成了NODE，另外有size标注这个NODE里面有多少Element
- NODE_INDEX类：标注NODE的特征信息，就是最大元素、最小元素，pos表示在文件中的位置，这样可以与NODE关联起来
- 创建NODE_INDEX类的原因：因为查找时如果将索引与实际数据放在一起，即使读取只读取一小部分，仍然会使指针大范围地移动，导致速度变慢，所以索引类就是要将这些特征信息压缩聚集到一起
- tail：另外存了一个文件，用来指示最后一块的索引，因为不保存的话会出bug

### 成员函数

- Insert：具体找到插入那个块，然后将块内比该元素大的元素后移
- Delete：具体找到插入那个块，然后将块内比该元素大的元素前移
- split：裂块，防止块太大，超过限度就裂成两块
- merge：并块，防止块太小，超过限度就并成一块
- borrow：借块，防止无法并块，就是并块以后超出限制，那么就借块，从前面或者后面借一点元素

'''cpp
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
'''

## booksystem

- std::string file_book_name：存储booksystem的文件名
- std::fstream file_book：存储booksystem的文件相关的流
- BlockChain isbn_chain;
- BlockChain name_chain;
- BlockChain author_chain;
- BlockChain keyword_chain;
- 上面分别建立了从isbn、name、author、keyword的块状链表
- 实际存储图书的所有信息的是file_book对应的文件

'''cpp
#ifndef CODE_BOOKSYSTEM_H
#define CODE_BOOKSYSTEM_H
#include"blockchain.h"
#include"usersystem.h"

class Book{
public:
    char isbn[33]={'\0'};//赋初值？
    char BookName[63]={'\0'};
    char Author[63]={'\0'};
    char Keyword[63]={'\0'};
    int quntity=0;
    double price=0.0;//错误：注意是double类型，且要有初始值
    double totalcost=0.0;//交易总额
    int rest=0;
public:
    Book(std::string);
    Book()=default;
    bool operator<(Book &other);
};

//booksystem用来存图书的真实信息，而块状链表存了图书的索引，就是value值
class Booksystem{
public:
    std::string file_book_name;
    std::fstream file_book;
    BlockChain isbn_chain;
    BlockChain name_chain;
    BlockChain author_chain;
    BlockChain keyword_chain;//不确定
public:
    Booksystem(std::string,std::string,std::string,std::string,
                std::string,std::string,std::string,std::string,
                std::string,std::string,std::string,std::string,std::string);
    Booksystem();
    ~Booksystem();
    void show_isbn(std::string);
    void show_name(std::string);
    void show_author(std::string);
    void show_keyword(std::string);
    void print_book(int pos);
    double buy(std::string,int);
    void select(Usersystem&,std::string);
    bool import(Usersystem&,int,double);
};    


#endif //CODE_BOOKSYSTEM_H
'''
  

## usersyetem

- std::string file_user_name：存储用户的文件名
- std::fstream file_user：存储用户的文件相关的流
- user_chain：由ID找到索引
- std::vector login_now：现在登录的用户，维护登录栈
- std::vector login_now_select：保存选中的书的索引，与上面的vector一一对应
- User login_user：现在登录的user

'''cpp
#ifndef CODE_USERSYSTEM_H
#define CODE_USERSYSTEM_H
#include "blockchain.h"
#include <string>

class User{
public:
    char UserID[33]={'\0'};//错误：这里要赋初值，不然所得的值会有问题
    char password[33]={'\0'};
    char username[33]={'\0'};
    int privilege = 0;
public:
    User(std::string,std::string,std::string,int);
    User()=default;
};

class Usersystem{
public:
    std::string file_user_name;
    std::fstream file_user;
    BlockChain user_chain; //由ID找到索引
    std::vector<std::string> login_now;
    std::vector<int> login_now_select;//保存选中的书的索引
    User login_user;//现在登录的user
    bool flag_log=false;
public:
    Usersystem(std::string,std::string,std::string,std::string);
    ~Usersystem();
    void su(std::string,std::string);
    void logout();
    void register_(std::string,std::string,std::string);
    void modify(std::string,std::string,std::string);
    void useradd(std::string,std::string,std::string,int);
    void delete_(std::string);
};

#endif
'''

## log

- std::string file_finance_name：存储日志的文件名
- std::fstream file_finance：存储日志的文件相关的流
- Insert：插入元素
- Show：展示元素

'''cpp
#ifndef CODE_LOG_H
#define CODE_LOG_H

#include<bits/stdc++.h>

class Once{
public:
    bool flag;//表示收入还是支出
    double money;
public:
    Once(bool,double);
    Once()=default;
};

class Finance{
public:
    std::string file_finance_name;
    std::fstream file_finance;
public:
    Finance(std::string);
    ~Finance();
    void Insert(Once);
    void Show(int);
};

#endif
'''
