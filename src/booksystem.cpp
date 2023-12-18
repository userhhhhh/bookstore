#include "booksystem.h"
#include "blockchain.h"
#include "usersystem.h"
#include <algorithm>

const int sizeofbook=sizeof(Book);

Booksystem::Booksystem(std::string str1,std::string str2,std::string str3,
                        std::string str4,std::string str5,std::string str6,
                        std::string str7,std::string str8,std::string str9,
                        std::string str10,std::string str11,std::string str12):
    isbn_chain(str1,str2,str3),name_chain(str4,str5,str6),
    author_chain(str7,str8,str9),keyword_chain(str10,str11,str12){
    file_book.open(file_book_name);
    if (!file_book.good()) {
        file_book.open(file_book_name, std::ios::out);
        file_book.close();
        file_book.open(file_book_name);
    }
}
Booksystem::Booksystem(){
    file_book.open(file_book_name);
    if (!file_book.good()) {
        file_book.open(file_book_name, std::ios::out);
        file_book.close();
        file_book.open(file_book_name);
    }
}
Booksystem::~Booksystem(){
    if (file_book.is_open()) file_book.close();
}

void Booksystem::print_book(int pos){
    file_book.seekg(pos);
    Book book;  //表示在v[0]处的book的全部信息
    file_book.read(reinterpret_cast<char*>(&book),sizeofbook);
    std::cout<<book.isbn<<'\t'<<book.BookName<<'\t'<<book.Author
        <<'\t'<<book.Keyword<<'\t'<<book.price<<'\t'<<book.rest<<'\n';
}
void Booksystem::show_isbn(std::string isbn_in){
    std::vector<int> v;
    v = isbn_chain.Find(isbn_in);
    if(v.empty()) {std::cout<<'\n'; return;}
    print_book(v[0]);
};
void Booksystem::show_name(std::string name_in){
    std::vector<int> v;
    v = isbn_chain.Find(name_in);
    if(v.empty()) {std::cout<<'\n'; return;}
    for(int i=0;i<v.size();++i){
        print_book(v[i]);
    }
}
void Booksystem::show_author(std::string author_in){
     std::vector<int> v;
    v = isbn_chain.Find(author_in);
    if(v.empty()) {std::cout<<'\n'; return;}
    for(int i=0;i<v.size();++i){
        print_book(v[i]);
    }
}
void Booksystem::show_keyword(std::string keyword_in){
    std::vector<int> v;
    v = isbn_chain.Find(keyword_in);
    if(v.empty()) {std::cout<<'\n'; return;}
    for(int i=0;i<v.size();++i){
        print_book(v[i]);
    }
}
//购买指的是从存货中减少
void Booksystem::buy(std::string isbn_in,int Quantity){
    if(Quantity<=0) {std::cout<<"Invalid\n"; return;}
    std::vector<int> v;
    v = isbn_chain.Find(isbn_in);//索引值
    if(v.size()==0) {std::cout<<"Invalid\n"; return;}
    file_book.seekg(v[0]);
    Book book;  //表示在v[0]处的book的全部信息
    file_book.read(reinterpret_cast<char*>(&book),sizeofbook);
    if(book.rest<Quantity) {std::cout<<"Invalid\n"; return;}
    book.rest-=Quantity;
    file_book.seekp(v[0]);
    file_book.write(reinterpret_cast<char*>(&book),sizeofbook);
    std::cout<< Quantity * book.price <<'\n';
}
void Booksystem::select(Usersystem& usersystem,std::string isbn_in){
    std::vector<int> v;
    v = isbn_chain.Find(isbn_in);//索引值
    if(v.size()==0) {
        //


        return;
    }
    usersystem.login_now_select.pop_back();
    usersystem.login_now_select.push_back(v[0]);
}
void Booksystem::modify_isbn(Usersystem& usersystem,std::string isbn_in){
    int tmp = usersystem.login_now_select.back();//当前选中的书的索引
    if(tmp==-1) {std::cout<<"Invalid\n"; return;}
    Book tmp1;
    file_book.seekg(tmp);
    file_book.read(reinterpret_cast<char*>(&tmp1),sizeofbook);
    std::copy(isbn_in.begin(),isbn_in.end(),tmp1.isbn);
    file_book.seekp(tmp);
    file_book.write(reinterpret_cast<char*>(&tmp1),sizeofbook);
}
void Booksystem::modify_name(Usersystem& usersystem,std::string name_in){
    int tmp = usersystem.login_now_select.back();//当前选中的书的索引
    if(tmp==-1) {std::cout<<"Invalid\n"; return;}
    Book tmp1;
    file_book.seekg(tmp);
    file_book.read(reinterpret_cast<char*>(&tmp1),sizeofbook);
    std::copy(name_in.begin(),name_in.end(),tmp1.isbn);
    file_book.seekp(tmp);
    file_book.write(reinterpret_cast<char*>(&tmp1),sizeofbook);
}
void Booksystem::modify_author(Usersystem& usersystem,std::string author_in){
    int tmp = usersystem.login_now_select.back();//当前选中的书的索引
    if(tmp==-1) {std::cout<<"Invalid\n"; return;}
    Book tmp1;
    file_book.seekg(tmp);
    file_book.read(reinterpret_cast<char*>(&tmp1),sizeofbook);
    std::copy(author_in.begin(),author_in.end(),tmp1.isbn);
    file_book.seekp(tmp);
    file_book.write(reinterpret_cast<char*>(&tmp1),sizeofbook);
}
void Booksystem::modify_keyword(Usersystem& usersystem,std::string keyword_in){
    int tmp = usersystem.login_now_select.back();//当前选中的书的索引
    if(tmp==-1) {std::cout<<"Invalid\n"; return;}
    Book tmp1;
    file_book.seekg(tmp);
    file_book.read(reinterpret_cast<char*>(&tmp1),sizeofbook);
    std::copy(keyword_in.begin(),keyword_in.end(),tmp1.isbn);
    file_book.seekp(tmp);
    file_book.write(reinterpret_cast<char*>(&tmp1),sizeofbook);
}
void Booksystem::import(Usersystem& usersystem,int Quantity,double TotalCost){
    int tmp = usersystem.login_now_select.back();//当前选中的书的索引
    if(tmp==-1 || Quantity<=0 || TotalCost<=0) {std::cout<<"Invalid\n"; return;}
    Book tmp1;
    file_book.seekg(tmp);
    file_book.read(reinterpret_cast<char*>(&tmp1),sizeofbook);
    tmp1.rest += Quantity;
    //totalcost没懂?
    file_book.seekp(tmp);
    file_book.write(reinterpret_cast<char*>(&tmp1),sizeofbook);
}