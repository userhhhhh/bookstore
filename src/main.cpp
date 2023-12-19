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

//   freopen("/home/hqs123/bookstore/src/test/basic/testcase3.in","r",stdin);
//   freopen("0.out","w",stdout);

    Booksystem booksystem("isbn_data","isbn_index","isbn_tail",
                        "name_data","name_index","name_tail",
                        "author_data","author_index","author_tail",
                        "keyword_data","keyword_index","keyword_tail","book");
    Usersystem usersystem("user","user_data","user_index","user_tail");
    Finance finance("finance");

    std::string input_string;
    while(std::getline(std::cin,input_string)){
        bool result = std::all_of(input_string.begin(), input_string.end(), [](char c) { return c == ' '; });
        if(result) continue;//先处理一下空串
        std::istringstream iss(input_string);
        std::string command;
        iss>>command;
        // if(iss.fail()) {std::cout<<"Invalid\n";continue;}
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
                std::fstream inputFile("book");
                std::vector<Book> books;
                while(inputFile.peek() != EOF){
                    Book book;
                    inputFile.read(reinterpret_cast<char*>(&book),sizeof(Book));
                    books.push_back(book);
                }
                inputFile.close();
                std::sort(books.begin(), books.end());
                for(int i=0;i<books.size();++i){
                    std::cout<<books[i].isbn<<'\t'<<books[i].BookName<<'\t'<<books[i].Author
                    <<'\t'<<books[i].Keyword<<'\t'<<books[i].price<<'\t'<<books[i].rest<<'\n';
                }
            }
            else if(v.size()==1 && v[0]!="finance"){
                // 不确定：附加参数内容为空则操作失败？
                // 不确定：[Keyword] 中出现多个关键词则操作失败？

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
                if(!isValidInt(v[1])) {std::cout<<"Invalid\n";continue;}
                int tmp=std::stoi(v[1]);
                double result=booksystem.buy(v[0],tmp);
                if(result!=0.0){
                    Once once;
                    once.flag=true;
                    once.money=result;
                    finance.Insert(once);
                }
            }
            else {std::cout<<"Invalid\n";continue;}
        }
        else if(command=="select"){
            if(v.size()==1){booksystem.select(usersystem,v[0]);}
            else {std::cout<<"Invalid\n";continue;}
        }
        else if(command=="modify"){
            // 不确定：附加参数内容为空则操作失败
            // 不确定：[keyword] 包含重复信息段则操作失败

            std::string already_have[5]={"","","","",""};
            //上面的5个元素分别代表ISBN、name、author、keyword、price
            bool flag=true;
            for(int i=0;i<v.size();++i){           
                int pos=v[i].find("=");
                if(pos==std::string::npos) {std::cout<<"Invalid\n";flag=false;break;}
                std::string s1=v[i].substr(0,pos);
                std::string s2=v[i].substr(pos+1);
                if(s1=="-ISBN"){
                    if(already_have[0]=="") {already_have[0]=s2;}
                    else{std::cout<<"Invalid\n";flag=false;break;}
                }
                else if(s1=="-name"){
                    if(already_have[1]=="") {already_have[1]=s2;}
                    else{std::cout<<"Invalid\n";flag=false;break;}          
                }
                else if(s1=="-author"){
                    if(already_have[2]=="") {already_have[2]=s2;}
                    else{std::cout<<"Invalid\n";flag=false;break;}
                }
                else if(s1=="-keyword"){
                    if(already_have[3]=="") {already_have[3]=s2;}
                    else{std::cout<<"Invalid\n";flag=false;break;}
                }
                else if(s1=="-price"){
                    if(isValidFloat(s2)){
                        if(already_have[1]=="") {already_have[1]=s2;}
                        else{std::cout<<"Invalid\n";flag=false;break;}
                    }
                    else{std::cout<<"Invalid\n";flag=false;break;}
                }
                else {std::cout<<"Invalid\n";flag=false;break;}
            }
            if(!flag){continue;}
            int tmp = usersystem.login_now_select.back();//当前选中的书的索引
            if(tmp==-1) {std::cout<<"Invalid\n";continue;}
            Book tmp1;
            booksystem.file_book.seekg(tmp);
            booksystem.file_book.read(reinterpret_cast<char*>(&tmp1),sizeof(Book));
            if(already_have[0]!=""){
                char exchange[33]={'\0'};
                std::copy(already_have[0].begin(),already_have[0].end(),exchange);
                if(std::strcmp(tmp1.isbn,exchange)==0){std::cout<<"Invalid\n";continue;}
                std::copy(already_have[0].begin(),already_have[0].end(),tmp1.isbn);
            }
            if(already_have[1]!=""){std::copy(already_have[1].begin(),already_have[1].end(),tmp1.BookName);}
            if(already_have[2]!=""){std::copy(already_have[2].begin(),already_have[2].end(),tmp1.Author);}
            if(already_have[3]!=""){std::copy(already_have[3].begin(),already_have[3].end(),tmp1.Keyword);}
            if(already_have[4]!=""){double num=std::stod(already_have[4]);tmp1.price=num;}
            booksystem.file_book.seekp(tmp);
            booksystem.file_book.write(reinterpret_cast<char*>(&tmp1),sizeof(Book));
        }
        else if(command=="import"){
            if(v.size()==2 && isValidInt(v[0]) && isValidFloat(v[1])){
                int tmp1 = std::stoi(v[0]);
                double tmp2 = std::stod(v[1]);
                if(booksystem.import(usersystem,tmp1,tmp2)){
                    Once once;
                    once.flag=false;
                    once.money = tmp2;
                    finance.Insert(once);
                }
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