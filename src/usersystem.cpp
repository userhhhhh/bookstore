#include "usersystem.h"
#include "blockchain.h"
#include <algorithm>
#include <cstring>

const int sizeofuser = sizeof(User);

User::User(std::string str1,std::string str2,std::string str3,int num){
    std::copy(str1.begin(),str1.end(),UserID);
    std::copy(str2.begin(),str2.end(),password);
    std::copy(str3.begin(),str3.end(),username);
    privilege=num;
}
Usersystem::Usersystem(){
    file_user.open(file_user_name);
    if (!file_user.good()) {
        file_user.open(file_user_name, std::ios::out);
        file_user.close();
        file_user.open(file_user_name);
    }
}
Usersystem::~Usersystem(){
    if (file_user.is_open()) file_user.close();
}

void Usersystem::su(std::string UserID_in,std::string passwd_in_){
    char passwd_in[33];
    std::copy(passwd_in_.begin(),passwd_in_.end(),passwd_in);
    //下面v只有一个元素，并且该元素为user的索引
    std::vector<int> v;
    v = user_chain.Find(UserID_in);
    if(v.empty()) {std::cout<<"Invalid\n"; return;}
    //先把这个userID对应的User找出，再看是否符合要求
    file_user.seekg(v[0]);
    User tmp;
    file_user.read(reinterpret_cast<char*>(&tmp),sizeofuser);
    if(std::strcmp(tmp.password,passwd_in)==0 ||
      (passwd_in_=="" && login_user.privilege>tmp.privilege)){
        login_now.push_back(UserID_in);
        login_now_select.push_back(-1);
        login_user=tmp;
        flag_log=true;
    }
    else{std::cout<<"Invalid\n";}
}
void Usersystem::logout(){
    if(login_now.empty()) {std::cout<<"Invalid\n";return;}
    login_now.pop_back();
    login_now_select.pop_back();
    if(login_now.empty()) {flag_log=false;}
    else {
        std::vector<int> v;
        v = user_chain.Find(login_now.back());
        if(v.empty()) {std::cout<<"Invalid\n"; return;}
        file_user.seekg(v[0]);
        file_user.read(reinterpret_cast<char*>(&login_user),sizeofuser);
    }
    
}
void Usersystem::register_(std::string UserID_in,std::string passwd_in,std::string username_in){
    std::vector<int> v;
    v = user_chain.Find(UserID_in);
    if(!v.empty()){std::cout<<"Invalid\n";return;}
    User user(UserID_in,passwd_in,username_in,1);
    file_user.seekp(0,std::ios::end);
    int index=file_user.tellp();
    file_user.write(reinterpret_cast<char*>(&user),sizeofuser);
    Element element(UserID_in);
    element.value=index;
    user_chain.Insert(element);
}
void Usersystem::modify(std::string UserID_in,std::string NewPassword,std::string CurrentPassword=""){
    if(CurrentPassword=="" && login_user.privilege!=7){std::cout<<"Invalid\n";return;}
    std::vector<int> v;
    v = user_chain.Find(UserID_in);
    if(v.empty()){std::cout<<"Invalid\n";return;}
    file_user.seekg(v[0]);
    User tmp;
    file_user.read(reinterpret_cast<char*>(&tmp),sizeofuser);
    char passwd_in[33];
    std::copy(CurrentPassword.begin(),CurrentPassword.end(),passwd_in);
    if(std::strcmp(tmp.password,passwd_in)==0){
        std::copy(CurrentPassword.begin(),CurrentPassword.end(),tmp.password);
        file_user.seekp(v[0]);
        file_user.write(reinterpret_cast<char*>(&tmp),sizeofuser);
    }
    else{std::cout<<"Invalid\n";}
}
void Usersystem::useradd(std::string UserID_in,std::string passwd_in,std::string username_in,int num){
    std::vector<int> v;
    v = user_chain.Find(UserID_in);
    if(!v.empty()){std::cout<<"Invalid\n";return;}
    //上面说明没有这个元素
    User user(UserID_in,passwd_in,username_in,num);
    file_user.seekp(0,std::ios::end);
    int index=file_user.tellp();
    file_user.write(reinterpret_cast<char*>(&user),sizeofuser);
    Element element(UserID_in);
    element.value=index;
    user_chain.Insert(element);
}
void Usersystem::delete_(std::string UserID_in){
    std::vector<int> v;
    v = user_chain.Find(UserID_in);
    if(v.empty()){std::cout<<"Invalid\n";return;}
    Element element(UserID_in);
    element.value=v[0];
    user_chain.Delete(element);
}