#include"log.h"

const int sizeofonce = sizeof(Once);

Finance::Finance(std::string str):file_finance_name(str){
    file_finance.open(file_finance_name);
    if (!file_finance.good()) {
        file_finance.open(file_finance_name, std::ios::out);
        file_finance.close();
        file_finance.open(file_finance_name);
    }
}
Finance::~Finance(){
    if (file_finance.is_open()) file_finance.close();
}
void Finance::Insert(Once once){
    file_finance.seekp(0,std::ios::end);
    file_finance.write(reinterpret_cast<char*>(&once),sizeofonce);
}
void Finance::Show(int count){
    if(count==0){std::cout<<'\n';return;}
    double income=0.00;
    double outcome=0.00;
    Once once;
    file_finance.seekg(0,std::ios::end);
    int index=file_finance.tellg();
    if(index < count*sizeofonce){std::cout<<"Invalid\n";return;}
    for(int i=1;i<=count;++i){
        file_finance.seekg(index-i*sizeofonce);
        file_finance.read(reinterpret_cast<char*>(&once),sizeofonce);
        if(once.flag==true) income+=once.money;
        if(once.flag==false) outcome+=once.money;
    }
    std::cout<<"+ "<<income<<" - "<<outcome<<'\n';
}