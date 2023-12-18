#include"blockchain.h"
#include"booksystem.h"
#include"usersystem.h"
#include"log.h"

bool isValidFloat(std::string& str){
    std::istringstream iss1(str);
    double value;
    if(iss1>>value){
        char remainingChar;// 检查是否整个字符串都被解析
        if(iss1 >> remainingChar) return false;    
        else return true; 
    }else{return false;}
}
bool isValidInt(std::string& str){
    size_t pos=0;
    if(str[pos]=='+' || str[pos]=='-') ++pos;
    if(!std::isdigit(str[pos])) return false;
    for(; pos<str.length();++pos){
        if (!std::isdigit(str[pos])) return false;
    }
    return true;
}

int main(){
    Booksystem booksystem("isbn_data","isbn_index","isbn_tail",
                        "name_data","name_index","name_tail",
                        "author_data","author_index","author_tail",
                        "keyword_data","keyword_index","keyword_tail");
    Usersystem usersystem;
    Finance finance("finance");
    
    std::string input_string;
    while(std::getline(std::cin,input_string)){
        bool result = std::all_of(input_string.begin(), input_string.end(), [](char c) { return c == ' '; });
        if(result) continue;//先处理一下空串
        std::istringstream iss(input_string);
        std::string command;
        iss>>command;
        if(iss.fail()) {std::cout<<"Invalid\n";continue;}
        std::vector<std::string> v;
        std::string word;
        while (iss >> word){
            v.push_back(word);
        }
        //上面的操作将除了最前面的指令都读出来了，存放在v里面
        if(command=="su"){
            if(v.size()==2) {usersystem.su(v[0],v[1]);}
            else if(v.size()==1) {usersystem.su(v[0],"");}
            else {std::cout<<"Invalid\n";continue;}
        }
        else if(command=="logout"){
            if(v.empty()) {usersystem.logout();}
            else {std::cout<<"Invalid\n";continue;}
        }
        else if(command=="register"){
            if(v.size()==3) {usersystem.register_(v[0],v[1],v[2]);}
            else {std::cout<<"Invalid\n";continue;}
        }
        else if(command=="passwd"){
            if(v.size()==3) {usersystem.modify(v[0],v[2],v[1]);}
            else if(v.size()==2) {usersystem.modify(v[0],v[1],"");}
            else {std::cout<<"Invalid\n";continue;}
        }
        else if(command=="useradd"){
            if(v.size()==4 && (v[2]=="0"||v[2]=="1"||v[2]=="3"||v[2]=="7")){
                int tmp=std::stoi(v[2]);
                usersystem.useradd(v[0],v[1],v[3],tmp);
            }
            else {std::cout<<"Invalid\n";continue;}
        }
        else if(command=="delete"){
            if(v.size()==1){usersystem.delete_(v[0]);}
            else {std::cout<<"Invalid\n";continue;}
        }
        else if(command=="show"){//两种情况
            if(v.size()==2 && v[0]=="finance" && isValidInt(v[1])==true){
                int tmp=std::stoi(v[1]);
                finance.Show(tmp);
            }
            else if(v.size()==1 && v[0]=="finance"){
                finance.file_finance.seekp(0,std::ios::end);
                int index=finance.file_finance.tellp();
                int count=index/sizeof(Once);
                finance.Show(count);
            }
            else if(v.size()==0){
                //不会？
            }
            else if(v.size()==1 && v[0]!="finance"){
                int pos=v[0].find("=");
                if(pos==std::string::npos) {std::cout<<"Invalid\n";continue;}
                std::string s1=v[0].substr(0,pos);
                std::string s2=v[0].substr(pos+1);
                if(s1=="-ISBN"){booksystem.show_isbn(s2);}
                else if(s1=="-name"){booksystem.show_name(s2);}
                else if(s1=="-author"){booksystem.show_author(s2);}
                else if(s1=="-keyword"){booksystem.show_keyword(s2);}
                else {std::cout<<"Invalid\n";continue;}   
            }
            else {std::cout<<"Invalid\n";continue;}
        }
        else if(command=="buy"){
            if(v.size()==2){
                int tmp=std::stoi(v[1]);
                booksystem.buy(v[0],tmp);
            }
            else {std::cout<<"Invalid\n";continue;}
        }
        else if(command=="select"){
            if(v.size()==1){booksystem.select(usersystem,v[0]);}
            else {std::cout<<"Invalid\n";continue;}
        }
        else if(command=="modify"){
            //问题：这里失败怎么输出？
            for(int i=0;i<v.size();++i){           
                int pos=v[i].find("=");
                if(pos==std::string::npos) {std::cout<<"Invalid\n";continue;}
                std::string s1=v[i].substr(0,pos);
                std::string s2=v[i].substr(pos+1);
                if(s1=="-ISBN"){booksystem.modify_isbn(usersystem,s2);}
                else if(s1=="-name"){booksystem.modify_name(usersystem,s2);}
                else if(s1=="-author"){booksystem.modify_author(usersystem,s2);}
                else if(s1=="-keyword"){booksystem.modify_keyword(usersystem,s2);}
                else {std::cout<<"Invalid\n";continue;}
            }
        }
        else if(command=="import"){
            if(v.size()==2 && isValidInt(v[0]) && isValidFloat(v[1])){
                int tmp1 = std::stoi(v[0]);
                double tmp2 = std::stod(v[1]);
                booksystem.import(usersystem,tmp1,tmp2);
            }
            else {std::cout<<"Invalid\n";continue;}
        }
        else if(command=="log"){
            
        }
        else if(command=="report"){
            
        }
        else if(command=="quit" || command=="exit"){
            if(v.size()==0){exit(0);}  //不确定
            else {std::cout<<"Invalid\n";continue;}
        }
        else{
            std::cout<<"Invalid\n";continue;
        }
    }
    return 0;
}