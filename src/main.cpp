#include"blockchain.h"
#include"booksystem.h"
#include"usersystem.h"
#include"log.h"
#include <vector>

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
std::vector<std::string> get_keyword(std::string str){
    std::vector<std::string> substrings;
    std::istringstream iss1(str);
    std::string sub;
    while(std::getline(iss1, sub,'|')){
        substrings.push_back(sub);
    }
    if(substrings.empty()){substrings.push_back(str);}
    return substrings;
}
int main(){

//   freopen("/home/hqs123/bookstore/src/test/complex/testcase3/1.in","r",stdin);
//   freopen("0.out","w",stdout);

    Booksystem booksystem("isbn_data","isbn_index","isbn_tail",
                        "name_data","name_index","name_tail",
                        "author_data","author_index","author_tail",
                        "keyword_data","keyword_index","keyword_tail","book");
    Usersystem usersystem("user","user_data","user_index","user_tail");
    Finance finance("finance");

    std::string input_string;
    int num_count=1;
    while(std::getline(std::cin,input_string)){
//        std::cout<<num_count;
//        if(num_count==259){
//            int aaa=19;
//        }
//        num_count++;
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
            else {std::cout<<"Invalid"<<std::endl;continue;}
        }
        else if(command=="logout"){
            if(usersystem.login_now.empty()){std::cout<<"Invalid"<<std::endl;continue;}
            if(v.empty()) {usersystem.logout();}
            else {std::cout<<"Invalid"<<std::endl;continue;}
        }
        else if(command=="register"){
            if(v.size()==3) {usersystem.register_(v[0],v[1],v[2]);}
            else {std::cout<<"Invalid"<<std::endl;continue;}
        }
        else if(command=="passwd"){
            //错误：要求登录状态下才能进行这个操作
            if(usersystem.login_now.empty()){std::cout<<"Invalid"<<std::endl;continue;}
            if(v.size()==3) {usersystem.modify(v[0],v[2],v[1]);}
            else if(v.size()==2) {usersystem.modify(v[0],v[1],"");}
            else {std::cout<<"Invalid"<<std::endl;continue;}
        }
        else if(command=="useradd"){
            if(usersystem.login_now.empty()){std::cout<<"Invalid"<<std::endl;continue;}
            if(usersystem.login_user.privilege<3){std::cout<<"Invalid"<<std::endl;continue;}
            if(v.size()==4 && (v[2]=="0"||v[2]=="1"||v[2]=="3"||v[2]=="7")){
                int tmp=std::stoi(v[2]);
                usersystem.useradd(v[0],v[1],v[3],tmp);
            }
            else {std::cout<<"Invalid"<<std::endl;continue;}
        }
        else if(command=="delete"){
            if(usersystem.login_now.empty()){std::cout<<"Invalid"<<std::endl;continue;}
            if(usersystem.login_user.privilege<7){std::cout<<"Invalid"<<std::endl;continue;}
            if(v.size()==1){usersystem.delete_(v[0]);}
            else {std::cout<<"Invalid"<<std::endl;continue;}
        }
        else if(command=="show"){//两种情况
            if(usersystem.login_now.empty()){std::cout<<"Invalid"<<std::endl;continue;}
            if(v.size()==2 && v[0]=="finance" && isValidInt(v[1])==true){
                if(usersystem.login_user.privilege<7){std::cout<<"Invalid"<<std::endl;continue;}
                int tmp=std::stoi(v[1]);
                finance.Show(tmp);
            }
            else if(v.size()==1 && v[0]=="finance"){
                if(usersystem.login_user.privilege<7){std::cout<<"Invalid"<<std::endl;continue;}
                finance.file_finance.seekp(0,std::ios::end);
                int index=finance.file_finance.tellp();
                int count=index/sizeof(Once);
                finance.Show(count);
            }
            else if(v.size()==0){
                std::vector<Book> books;
                // std::fstream inputFile("book");
                // 错误：这里不能再开一个流，不然它的更新会延迟，直接用原来的 file_book即可
                booksystem.file_book.seekg(0);
                while(booksystem.file_book.peek()!=EOF){
                    Book book;
                    booksystem.file_book.read(reinterpret_cast<char*>(&book),sizeof(Book));
                    books.push_back(book);
                }
                std::sort(books.begin(), books.end());
                for(int i=0;i<books.size();++i){
                    std::cout<<books[i].isbn<<'\t'<<books[i].BookName<<'\t'<<books[i].Author
                    <<'\t'<<books[i].Keyword<<'\t'<<std::fixed << std::setprecision(2)<<books[i].price<<'\t'<<books[i].rest<<std::endl;
                }
            }
            else if(v.size()==1 && v[0]!="finance"){
                // 不确定：附加参数内容为空则操作失败？
                // 不确定：[Keyword] 中出现多个关键词则操作失败？

                int pos=v[0].find("=");
                if(pos==std::string::npos) {std::cout<<"Invalid"<<std::endl;continue;}
                if(pos==v[0].size()-1) {std::cout<<"Invalid\n";break;}
                std::string s1=v[0].substr(0,pos);
                //不确定：这里引号没有怎么处理？
                if(s1=="-ISBN"){
                    std::string s2=v[0].substr(pos+1);
                    booksystem.show_isbn(s2);
                }
                else if(s1=="-name"){
                    std::string s2;
                    int startPos = v[0].find("\"");       // 查找第一个引号的位置
                    int endPos = v[0].find_last_of("\""); // 查找最后一个引号的位置
                    if (startPos != std::string::npos && endPos != std::string::npos && startPos < endPos) {
                        s2=v[0].substr(startPos + 1, endPos - startPos - 1); // 提取引号中的内容
                    }
                    else{std::cout<<"Invalid"<<std::endl;continue;}
                    booksystem.show_name(s2);
                }
                else if(s1=="-author"){
                    std::string s2;
                    int startPos = v[0].find("\"");       // 查找第一个引号的位置
                    int endPos = v[0].find_last_of("\""); // 查找最后一个引号的位置
                    if (startPos != std::string::npos && endPos != std::string::npos && startPos < endPos) {
                        s2=v[0].substr(startPos + 1, endPos - startPos - 1); // 提取引号中的内容
                    }
                    else{std::cout<<"Invalid"<<std::endl;continue;}
                    booksystem.show_author(s2);
                }
                else if(s1=="-keyword"){
                    std::string s2;
                    int startPos = v[0].find("\"");       // 查找第一个引号的位置
                    int endPos = v[0].find_last_of("\""); // 查找最后一个引号的位置
                    if (startPos != std::string::npos && endPos != std::string::npos && startPos < endPos) {
                        s2=v[0].substr(startPos + 1, endPos - startPos - 1); // 提取引号中的内容
                    }
                    else{std::cout<<"Invalid"<<std::endl;continue;}
                    booksystem.show_keyword(s2);
                }
                else {std::cout<<"Invalid"<<std::endl;continue;}
            }
            else {std::cout<<"Invalid"<<std::endl;continue;}
        }
        else if(command=="buy"){
            if(usersystem.login_now.empty()){std::cout<<"Invalid"<<std::endl;continue;}
            if(v.size()==2){
                if(!isValidInt(v[1])) {std::cout<<"Invalid"<<std::endl;continue;}
                int tmp=std::stoi(v[1]);
                double result=booksystem.buy(v[0],tmp);
                if(result!=0.0){
                    Once once;
                    once.flag=true;
                    once.money=result;
                    finance.Insert(once);
                }
            }
            else {std::cout<<"Invalid"<<std::endl;continue;}
        }
        else if(command=="select"){
            if(usersystem.login_now.empty()){std::cout<<"Invalid"<<std::endl;continue;}
            if(usersystem.login_user.privilege<3){std::cout<<"Invalid"<<std::endl;continue;}
            if(v.size()==1){booksystem.select(usersystem,v[0]);}
            else {std::cout<<"Invalid"<<std::endl;continue;}
        }
        else if(command=="modify"){
            if(usersystem.login_now.empty()){std::cout<<"Invalid"<<std::endl;continue;}
            if(usersystem.login_user.privilege<3){std::cout<<"Invalid"<<std::endl;continue;}
            // 不确定：附加参数内容为空则操作失败
            // 不确定：[keyword] 包含重复信息段则操作失败

            std::string already_have[5]={"","","","",""};
            //上面的5个元素分别代表ISBN、name、author、keyword、price
            bool flag=true;
            for(int i=0;i<v.size();++i){           
                int pos=v[i].find("=");
                if(pos==std::string::npos) {std::cout<<"Invalid"<<std::endl;flag=false;break;}
                if(pos==v[i].size()-1) {std::cout<<"Invalid"<<std::endl;flag=false;break;}
                std::string s1=v[i].substr(0,pos);
                if(s1=="-ISBN"){
                    std::string s2=v[i].substr(pos+1);
                    if(already_have[0]=="") {already_have[0]=s2;}
                    else{std::cout<<"Invalid\n";flag=false;break;}
                }
                else if(s1=="-name"){
                    //错误：要去掉引号
                    //错误：引号在string里面的存储还连带着\符号，因为要说明这是一个普通的字符引号
                    std::string s2;
                    int startPos = v[i].find("\"");       // 查找第一个引号的位置
                    int endPos = v[i].find_last_of("\""); // 查找最后一个引号的位置
                    if (startPos != std::string::npos && endPos != std::string::npos && startPos < endPos) {
                        s2=v[i].substr(startPos + 1, endPos - startPos - 1); // 提取引号中的内容
                    }
                    else{std::cout<<"Invalid"<<std::endl;flag=false;break;}
                    if(already_have[1]=="") {already_have[1]=s2;}
                    else{std::cout<<"Invalid"<<std::endl;flag=false;break;}
                }
                else if(s1=="-author"){
                    std::string s2;
                    int startPos = v[i].find("\"");       // 查找第一个引号的位置
                    int endPos = v[i].find_last_of("\""); // 查找最后一个引号的位置
                    if (startPos != std::string::npos && endPos != std::string::npos && startPos < endPos) {
                        s2=v[i].substr(startPos + 1, endPos - startPos - 1); // 提取引号中的内容
                    }
                    else{std::cout<<"Invalid"<<std::endl;flag=false;break;}
                    if(already_have[2]=="") {already_have[2]=s2;}
                    else{std::cout<<"Invalid"<<std::endl;flag=false;break;}
                }
                else if(s1=="-keyword"){
                    std::string s2;
                    int startPos = v[i].find("\"");       // 查找第一个引号的位置
                    int endPos = v[i].find_last_of("\""); // 查找最后一个引号的位置
                    if (startPos != std::string::npos && endPos != std::string::npos && startPos < endPos) {
                        s2=v[i].substr(startPos + 1, endPos - startPos - 1); // 提取引号中的内容
                    }
                    else{std::cout<<"Invalid"<<std::endl;flag=false;break;}
                    if(already_have[3]=="") {already_have[3]=s2;}
                    else{std::cout<<"Invalid"<<std::endl;flag=false;break;}
                }
                else if(s1=="-price"){
                    std::string s2=v[i].substr(pos+1);
                    if(isValidFloat(s2)){
                        if(already_have[4]=="") {already_have[4]=s2;}
                        else{std::cout<<"Invalid"<<std::endl;flag=false;break;}
                    }
                    else{std::cout<<"Invalid"<<std::endl;flag=false;break;}
                }
                else {std::cout<<"Invalid"<<std::endl;flag=false;break;}
            }
            if(!flag){continue;}
            int tmp = usersystem.login_now_select.back();//当前选中的书的索引
            if(tmp==-1) {std::cout<<"Invalid"<<std::endl;continue;}
            Book tmp1;
            booksystem.file_book.seekg(tmp);
            booksystem.file_book.read(reinterpret_cast<char*>(&tmp1),sizeof(Book));
            if(already_have[0]!=""){
                //错误：将原来的isbn改成exchange，但exchange不能与其他isbn重复
                char exchange[33]={'\0'};
                std::copy(already_have[0].begin(),already_have[0].end(),exchange);
                std::vector<int> v1=booksystem.isbn_chain.Find(already_have[0]);
                if(!v1.empty()) {std::cout<<"Invalid"<<std::endl;continue;}
                if(std::strcmp(tmp1.isbn,exchange)==0){std::cout<<"Invalid"<<std::endl;continue;}
                std::string str(tmp1.isbn,std::strlen(tmp1.isbn));
                Element e(str);
                e.value=tmp;
                booksystem.isbn_chain.Delete(e);
                //错误：顺序不能放反了
                for(int i=0;i<33;i++) {tmp1.isbn[i]='\0';}
                std::copy(already_have[0].begin(),already_have[0].end(),tmp1.isbn);
                for(int i=0;i<70;++i) {e.index[i]='\0';}
                std::copy(already_have[0].begin(),already_have[0].end(),e.index);
                booksystem.isbn_chain.Insert(e);
            }
            if(already_have[1]!=""){
                std::string str(tmp1.BookName,std::strlen(tmp1.BookName));
                std::vector<int> v1;
                v1=booksystem.name_chain.Find(str);
                Element e(str);
                e.value=tmp;
                if(!v1.empty()){//如果有，先删除
                    booksystem.name_chain.Delete(e);
                }
                for(int i=0;i<70;++i) {e.index[i]='\0';}
                std::copy(already_have[1].begin(),already_have[1].end(),e.index);
                booksystem.name_chain.Insert(e);
                for(int i=0;i<63;++i) {tmp1.BookName[i]='\0';}
                std::copy(already_have[1].begin(),already_have[1].end(),tmp1.BookName);
            }
            if(already_have[2]!=""){
                std::string str(tmp1.Author,std::strlen(tmp1.Author));
                std::vector<int> v1;
                v1=booksystem.author_chain.Find(str);
                Element e(str);
                e.value=tmp;
                if(!v1.empty()){//如果有，先删除
                    booksystem.author_chain.Delete(e);
                }
                for(int i=0;i<70;++i) {e.index[i]='\0';}
                std::copy(already_have[2].begin(),already_have[2].end(),e.index);
                booksystem.author_chain.Insert(e);
                for(int i=0;i<63;++i) {tmp1.Author[i]='\0';}
                std::copy(already_have[2].begin(),already_have[2].end(),tmp1.Author);
            }
            if(already_have[3]!=""){
                //不确定：keyword没实现(已写)
                //错误：改之后是 v1，是由 already_have[3]得到的
                //错误：改之前是 v3，是从 tmp1.Keyword里面读出来的
                std::vector<std::string> v1;
                v1=get_keyword(already_have[3]);
                std::string str(tmp1.Keyword,std::strlen(tmp1.Keyword));
                std::vector<std::string> v3;
                v3=get_keyword(str);
                //先删除元素
                for(int i=0;i<v3.size();++i){
                    std::vector<int> v2;
                    v2=booksystem.keyword_chain.Find(v3[i]);
                    Element e(v3[i]);
                    e.value=tmp;
                    if(!v2.empty()) {//如果有，先删除
                        booksystem.keyword_chain.Delete(e);
                    }
                }
                //现在插入元素
                for(int i=0;i<v1.size();++i){
                    Element e;
                    e.value=tmp;
                    std::copy(v1[i].begin(),v1[i].end(),e.index);
                    booksystem.keyword_chain.Insert(e);
                }
                for(int i=0;i<63;++i) {tmp1.Keyword[i]='\0';}
                std::copy(already_have[3].begin(),already_have[3].end(),tmp1.Keyword);
            }
            if(already_have[4]!=""){
                //错误：保留两位小数
                double num=std::stod(already_have[4]);
                std::stringstream ss;
                ss << std::fixed << std::setprecision(2) << num;
                ss >> tmp1.price;
            }
            booksystem.file_book.seekp(tmp);
            booksystem.file_book.write(reinterpret_cast<char*>(&tmp1),sizeof(Book));
        }
        else if(command=="import"){
            if(usersystem.login_now.empty()){std::cout<<"Invalid"<<std::endl;continue;}
            if(usersystem.login_user.privilege<3){std::cout<<"Invalid"<<std::endl;continue;}
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
            else {std::cout<<"Invalid"<<std::endl;continue;}
        }
        else if(command=="log"){
            
        }
        else if(command=="report"){
            
        }
        else if(command=="quit" || command=="exit"){
            if(v.size()==0){
                //错误：这里一定要把文件关掉，不然会出问题
                if(booksystem.file_book.is_open()) booksystem.file_book.close();
                if(usersystem.file_user.is_open()) usersystem.file_user.close();
                exit(0);
            }  //不确定
            else {std::cout<<"Invalid"<<std::endl;continue;}
        }
        else{
            std::cout<<"Invalid"<<std::endl;continue;
        }
    }
    return 0;
}