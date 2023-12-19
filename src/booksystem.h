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

    // void modify_isbn(Usersystem&,std::string);
    // void modify_name(Usersystem&,std::string);
    // void modify_author(Usersystem&,std::string);
    // void modify_keyword(Usersystem&,std::string);
    // void modify_price(Usersystem&,std::string);
};    


#endif //CODE_BOOKSYSTEM_H
