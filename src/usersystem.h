#ifndef CODE_USERSYSTEM_H
#define CODE_USERSYSTEM_H
#include "blockchain.h"
#include <string>

class User{
public:
    char UserID[33];
    char password[33];
    char username[33];
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
    Usersystem();
    ~Usersystem();
    void su(std::string,std::string);
    void logout();
    void register_(std::string,std::string,std::string);
    void modify(std::string,std::string,std::string);
    void useradd(std::string,std::string,std::string,int);
    void delete_(std::string);
};








#endif