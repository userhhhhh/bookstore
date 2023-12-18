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