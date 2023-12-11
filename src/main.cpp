//#include<bits/stdc++.h>
#include"blockchain.h"
//#include"blockchain.cpp"

int main(){
    BlockChain blockchain("b.txt");
    int command_count;
    std::cin>>command_count;
    for (int i = 0; i < command_count; ++i) {
        std::string command;
        std::cin >> command;

        if (command == "insert") {
            std::string index;
            int value;
            std::cin >> index >> value;
            Element element(index);
            element.value = value;
            blockchain.Insert(element);
        } else if (command == "delete") {
            std::string index;
            int value;
            std::cin >> index >> value;
            Element e(index);
            e.value=value;
            blockchain.Delete(e);
        } else if (command == "find") {
            std::string index;
            std::cin >> index;
            blockchain.Find(index);
        }
    }
    return 0;
}