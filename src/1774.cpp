#ifndef CODE_BLOCKCHAIN_H
#define CODE_BLOCKCHAIN_H
#include <bits/stdc++.h>

const int NUM = 1010;

//节点中的元素
#pragma pack(push,1)  //不确定1

class Element {
public:
    char index[70] = {'\0'};
    int value;
public:
    Element(std::string);
    Element() = default;
    bool operator<(const Element &e2) const;
    bool operator==(const Element &e2) const;
    bool operator>(const Element &e2) const;
};

//链表的每个节点
class NODE {
public:
    int size;//数组的大小
    Element a[NUM];

    NODE() = default;
};
class NODE_INDEX {
public:
    Element low;
    Element high;
    int pos;//表示在另一个文件中，NODE的位置
    int next_index=-1; //用来将 NODE_INDEX 连起来
    int last_index=-1; 
    int pos_index;
};
//对块状链表操作的过程
class BlockChain {
private:
    std::fstream file;
    std::string file_name;
    std::fstream file_index;
    std::string file_name_index;
public:
    bool flag = false;
    int tail = 0; //表示在b.txt中最后一个块的块头

    BlockChain(std::string,std::string);
    BlockChain() = default;
    ~BlockChain();
    void Insert(Element &);
    void Delete(Element &);
    void Find(std::string &);
    void writeNode(NODE &, int);
    void writeNodeIndex(NODE_INDEX &,int);
    void split(NODE &,NODE_INDEX &);
    void merge(NODE &,NODE_INDEX &,NODE &,NODE_INDEX &);
    void borrow(NODE &,NODE_INDEX &,NODE &,NODE_INDEX &);
    void remove(NODE &);
};

#endif //CODE_BLOCKCHAIN_H

const int MAX_SIZE = 1000;
const int MIN_SIZE = 400;
const int sizeofnode = sizeof(NODE);
const int sizeofelement = sizeof(Element);
const int sizeofindex=sizeof(NODE_INDEX);


Element::Element(std::string str) {
    std::copy(str.begin(), str.end(), index);
}

bool Element::operator<(const Element &e2) const {
    int result = std::strcmp(index, e2.index);
    if (result < 0) return true;
    else if (result > 0) return false;
    else if (result == 0) return value < e2.value;
    return true;
}

bool Element::operator==(const Element &e2) const {
    return (!strcmp(index, e2.index) && value == e2.value);
}

bool Element::operator>(const Element &e2) const {
    int result = std::strcmp(index, e2.index);
    if (result < 0) return false;
    else if (result > 0) return true;
    else if (result == 0) return value > e2.value;
    return true;
}

BlockChain::BlockChain(std::string file_name,std::string file_name_index) 
    : file_name(file_name),file_name_index(file_name_index) {
    //打开NODE的文件b.txt
    file.open(file_name);
    if (!file.good()) {
        file.open(file_name, std::ios::out);
        file.close();
        file.open(file_name);
    }
    //打开NODE_INDEX的文件a.txt
    file_index.open(file_name_index);
    if (!file_index.good()) {
        file_index.open(file_name_index, std::ios::out);
        file_index.close();
        file_index.open(file_name_index);
    }
}

BlockChain::~BlockChain() {
    if (file.is_open()) file.close();
    if (file_index.is_open()) file_index.close();
}

void BlockChain::Insert(Element &e) {
    //在一开始如果文件为空没有block，先创建一个新的block块
    //检查文件为空：如果为空，那就建立一个block
    file.seekp(0,std::ios::end);
    file_index.seekp(0,std::ios::end);
    int last1=file_index.tellp();
    int last=file.tellp();
    if ((!last) && (!last1)) {
        NODE n;
        n.a[0] = e; 
        n.size = 1;
        NODE_INDEX in;
        in.last_index = -1;//作为头节点
        in.next_index = -1;//不确定3：这里next没写？
        in.pos_index=0;
        in.pos=0;
        in.low = e;
        in.high = e;
        writeNode(n, 0);
        writeNodeIndex(in,0);
        tail = 0;
        return;
    }
    //下面是已经创建好block的情况
    //in是当前块的所有索引
    NODE_INDEX in;
    file_index.seekg(0, std::ios::beg);
    file_index.read(reinterpret_cast<char *>(&in), sizeofindex);
    while (e > in.high && in.pos != tail) {//错误：保证现在的块不是最后一块
        file_index.seekg(in.next_index);
        file_index.read(reinterpret_cast<char *>(&in),sizeofindex);
    }
    //现在已经找到了所在的block
    NODE n1;
    file.seekg(in.pos, std::ios::beg);
    file.read(reinterpret_cast<char *>(&n1), sizeofnode);//将整个NODE读出来
    //三种情况：a.元素小于该块的所有元素 b.元素大于该块的所有元素,且这个块是末尾块 c.元素在中间
    // a.这个元素小于该块的所有元素
    if (e < in.low) {
        for (int i = n1.size; i >= 1; --i) {
            n1.a[i] = n1.a[i - 1];
        }
        n1.a[0] = e;
        n1.size++;
        in.low = e;
        //裂块处理
        if(n1.size>=MAX_SIZE){
            split(n1,in);
            return;
        }
        writeNode(n1, in.pos);
        writeNodeIndex(in,in.pos_index);
        return;
    }
    // b.元素大于该块的所有元素,且这个块是末尾块
    if(e > in.high){
        n1.a[n1.size]=e;
        n1.size++;
        in.high = e;
        //裂块处理
        if(n1.size>=MAX_SIZE){
            split(n1,in);
            return;
        }
        writeNode(n1, in.pos);
        writeNodeIndex(in,in.pos_index);
        return;
    }
    // c.元素在中间
    // 下面二分查找最后一个<=该元素的元素下标
    // 首先不可能有相等元素，while循环结束后，l的值就是我们要找的
    int l = 0, r = n1.size-1;//错误：这里 r = n1.size-1
    while (l < r) {
        int mid = (l + r + 1) / 2;
        if (n1.a[mid] < e && mid <= n1.size - 1) { l = mid; }
        else { r = mid - 1; }
    }
    //现在l是最后一个<e的元素坐标
    for (int i = n1.size; i >= l + 2; --i) {
        n1.a[i] = n1.a[i - 1];
    }
    n1.a[l + 1] = e;
    n1.size++;
    //下面是裂块操作
    if (n1.size >= MAX_SIZE) {
        split(n1,in);
        return;
    }
    writeNode(n1, in.pos);
}

void BlockChain::Delete(Element &e) {
    //错误：防止下面读入为空
    file_index.seekp(0,std::ios::end);
    int test=file_index.tellp();
    if(!test){return;}

    NODE_INDEX in;
    file_index.seekg(0, std::ios::beg);
    file_index.read(reinterpret_cast<char *>(&in), sizeofindex);
    while (e > in.high && in.pos != tail) {//错误：保证现在的块不是最后一块
        file_index.seekg(in.next_index);
        file_index.read(reinterpret_cast<char *>(&in),sizeofindex);
    }
    //现在已经找到了所在的block
    //现在状态有两种：(1)e<=e_high (2)e>in.high && tail
    if(e > in.high){
        // std::cout << "null" << std::endl;
        return;
    }
    //现在必有：e<=e_high
    NODE n1;
    file.seekg(in.pos);
    file.read(reinterpret_cast<char *>(&n1), sizeofnode);//将整个NODE读出来
    // 下面二分查找最后一个<=该元素的元素下标
    // 有两种情况：(1)没有相等元素 (2)查找到了且就只有这一个元素
    // while循环结束后，l的值就是我们要找的
    int l = 0, r = n1.size-1;
    // 不确定：假如这一块为1，然后删掉里面元素，然后怎么操作
    // 这一块为1只有可能它是最后一块
    if(n1.size==1){
        if(e==n1.a[0] && in.pos==0){
            file.close();
            std::ofstream file1(file_name, std::ios::trunc);
            file1.close();
            file.open(file_name);
            file_index.close();
            std::ofstream file2(file_name_index, std::ios::trunc);
            file2.close();
            file_index.open(file_name_index);
            return;//错误：要return
        }else if(e==n1.a[0] && in.pos!=0){
            //不确定：这里是不是还要维护一个prev
            //这一块直接扔掉，不管它了，因为这样操作的可能性很小
            NODE_INDEX tmp3;
            file_index.seekg(in.last_index);
            file_index.read(reinterpret_cast<char*>(&tmp3),sizeofindex);
            tmp3.next_index=in.next_index;
            writeNodeIndex(tmp3,tmp3.pos_index);
            if(in.pos==tail){
                tail=tmp3.pos;
            }
            return;
        }else{
            // std::cout << "null" << std::endl;
            return;
        }
    }
    while (l < r) {
        int mid = (l + r + 1) / 2;
        if (n1.a[mid] <e || n1.a[mid] ==e) {l = mid;}
        else {r = mid - 1;}
    }
    if (n1.a[l] == e) {
        for (int i = l; i <= n1.size - 2; ++i) {
            n1.a[i] = n1.a[i + 1];
        }
        n1.size--;
        in.low=n1.a[0];
        in.high=n1.a[n1.size-1];
        writeNode(n1, in.pos);
        writeNodeIndex(in,in.pos_index);
    } else {
        // std::cout << "null" << std::endl;
        return;
    }
    //下面是并块或者借块操作
    if (n1.size >= MIN_SIZE) return;
    if (in.pos == 0) {//头节点
        if (tail == 0) return;//确保后面有节点
        NODE_INDEX in4;
        file_index.seekg(in.next_index);
        file_index.read(reinterpret_cast<char*>(&in4),sizeofindex);
        NODE n2;
        file.seekg(in4.pos);
        file.read(reinterpret_cast<char *>(&n2), sizeofnode);
        //现在in4是下一块的索引,n2是下一块节点
        if (n2.size + n1.size <= MAX_SIZE){
            merge(n1,in,n2,in4);
            if (tail == in4.pos) {tail = 0;}
        } else {borrow(n1,in,n2,in4);}
    } else if (in.pos == tail) {//尾节点
        if (tail == 0) return;//确保前面有节点
        NODE_INDEX in4;
        file_index.seekg(in.last_index);
        file_index.read(reinterpret_cast<char*>(&in4),sizeofindex);
        NODE n2;
        file.seekg(in4.pos);
        file.read(reinterpret_cast<char *>(&n2), sizeofnode);
        //现在n2是前面的节点，索引是in4，n1是后面的节点，索引是in
        if (n2.size + n1.size <= MAX_SIZE) {
            merge(n2,in4,n1,in);
            tail = in4.pos;
        } else borrow(n1,in,n2,in4);
    } else { //中间节点
        NODE_INDEX in4;
        file_index.seekg(in.next_index);
        file_index.read(reinterpret_cast<char*>(&in4),sizeofindex);
        NODE n2;
        file.seekg(in4.pos);
        file.read(reinterpret_cast<char *>(&n2), sizeofnode);
        //现在n2是下一块节点，in4是下一块的索引
        if (n2.size + n1.size <= MAX_SIZE) {
            merge(n1,in,n2,in4);
        }
        else borrow(n1,in,n2,in4);
    }
}

void BlockChain::Find(std::string &str) {
    //构造一个Element变量e_find，它的value为int最小值

    //问题：先判断一下是否为空，防止后面file_index.read读入为空
    file_index.seekp(0,std::ios::end);
    int test_empty=file_index.tellp();
    if(test_empty==0){
        std::cout<<"null"<<std::endl;
        return;//注意不能漏掉 return
    }
    Element e_find(str);
    e_find.value = std::numeric_limits<int>::min();
    NODE_INDEX in;
    file_index.seekg(0, std::ios::beg);
    file_index.read(reinterpret_cast<char *>(&in), sizeofindex);
    while (e_find > in.high && in.pos != tail) {
        file_index.seekg(in.next_index);
        file_index.read(reinterpret_cast<char *>(&in),sizeofindex);
    }
    //现在状态有两种：(1)e<=e_high (2)e>in.high && tail
    if(e_find > in.high){
        std::cout << "null" << std::endl;
        return;
    }
    //现在已经找到了所在的block
    NODE n1;
    file.seekg(in.pos, std::ios::beg);
    file.read(reinterpret_cast<char *>(&n1), sizeofnode);//将整个NODE读出来
    // 下面二分查找第一个>=该元素的元素下标
    // while循环结束后，l的值就是我们要找的
    if(n1.size==1){
        if(std::strcmp(n1.a[0].index, e_find.index) == 0){
            std::cout<<n1.a[0].value<<std::endl;
        }
        else{std::cout << "null" << std::endl;}
        return;
    }
    int l = 0, r = n1.size-1;
    while (l < r) {
        int mid = (l + r) / 2;
        if (n1.a[mid] > e_find || n1.a[mid]==e_find) { r = mid; }
        else { l = mid+1; }
    }
    bool exist = false;
    if (std::strcmp(n1.a[l].index, e_find.index) == 0) {
        std::cout << n1.a[l].value << ' ';
        exist = true;
    }
    l++;
    //下面开始往后找，一直找到index不同为止
    //当前读到的块是 n1，对应的索引是 tmp1
    //错误：tmp1要跟着变
    NODE_INDEX itmp;
    file_index.seekg(in.pos_index);
    file_index.read(reinterpret_cast<char*>(&itmp),sizeofindex);
    while (true) {
        while (std::strcmp(n1.a[l].index, e_find.index) == 0 && l <= n1.size - 1) {
            std::cout << n1.a[l].value << ' ';
            l++;//错误：l++写在cout下面
            exist = true;
        }
        if (l == n1.size && itmp.pos != tail) {//确保后面有节点
            l = 0;
            file_index.seekp(itmp.next_index);
            int tmp = file_index.tellp();//读出n1的下一块的位置
            file_index.seekg(tmp);
            file_index.read(reinterpret_cast<char*>(&itmp),sizeofindex);
            file.seekg(itmp.pos);
            file.read(reinterpret_cast<char *>(&n1), sizeofnode);
        } else break;
    }
    if (exist) std::cout << std::endl;
    if (!exist) std::cout << "null" << std::endl;

}

void BlockChain::writeNode(NODE &n, int pos_in) {
    file.seekp(pos_in, std::ios::beg); //不确定
    file.write(reinterpret_cast<char *>(&n), sizeofnode);
}

void BlockChain::writeNodeIndex(NODE_INDEX & in, int ipos_in){
    file_index.seekp(ipos_in, std::ios::beg);
    in.pos_index = ipos_in;
    file_index.write(reinterpret_cast<char *>(&in), sizeofindex);
} 

//将n1分裂成两块，新的一块是n2
void BlockChain::split(NODE &n1,NODE_INDEX &in1){
    //对n2操作
    NODE n2;
    for (int i = (MAX_SIZE / 2); i <= MAX_SIZE - 1; ++i) {
        n2.a[i - MAX_SIZE / 2] = n1.a[i];
    }
    //错误：MAX_SIZE-MAX_SIZE / 2，不是 MAX_SIZE / 2;
    n2.size = MAX_SIZE-MAX_SIZE / 2;
    file.seekp(0,std::ios::end);
    int tmp1=file.tellp();
    writeNode(n2, tmp1);
    //对in2操作
    NODE_INDEX in2;
    in2.pos=tmp1;
    in2.low = n2.a[0];
    in2.high = n1.a[MAX_SIZE - 1];
    in2.last_index = in1.pos_index;
    in2.next_index = in1.next_index;
    file_index.seekp(0,std::ios::end);
    int tmp2=file_index.tellp();
    in2.pos_index = tmp2;
    writeNodeIndex(in2,tmp2);
    //对n1操作
    n1.size = MAX_SIZE / 2;
    writeNode(n1, in1.pos);
    //对in1操作
    in1.high = n1.a[MAX_SIZE / 2 - 1];
    in1.next_index = in2.pos_index;
    writeNodeIndex(in1,in1.pos_index);
    //对in2的下一个元素操作
    if(in2.next_index!=-1){
        NODE_INDEX in3;
        file_index.seekg(in2.next_index);
        file_index.read(reinterpret_cast<char*>(&in3),sizeofindex);
        in3.last_index=in2.pos_index;
        writeNodeIndex(in3,in3.pos_index);
    }
    //tail处理
    if(in1.pos==tail){
        tail = in2.pos;
    }
}

//表示这一块跟它后面的一块合并
void BlockChain::merge(NODE &n1,NODE_INDEX &in1,NODE &n2,NODE_INDEX&in2) {
    for (int i = n1.size; i < n1.size+n2.size; ++i) {
        n1.a[i] = n2.a[i-n1.size];
    }
    n1.size = n1.size + n2.size;
    in1.high = n1.a[n1.size-1];
    in1.next_index = in2.next_index;
    writeNode(n1, in1.pos);
    writeNodeIndex(in1,in1.pos_index);
    if(in2.pos==tail) tail=in1.pos;//错误：这一句只能放这里，不能放下面
    if(in1.next_index==-1) return;
    else{
        file_index.seekg(in1.next_index);
        NODE_INDEX tmp;
        file_index.read(reinterpret_cast<char*>(&tmp),sizeofindex);
        tmp.last_index=in1.pos_index;
        file_index.write(reinterpret_cast<char*>(&tmp), sizeofindex);
    }
}

//表示n1从n2里借
void BlockChain::borrow(NODE &n1,NODE_INDEX &in1,NODE &n2,NODE_INDEX &in2) {
    // n1的元素太少，跟n2借50个
    for (int i = 0; i < 50; ++i) {
        n1.a[n1.size + i] = n2.a[i];
    }
    for (int i = 50; i < n2.size; ++i) {
        n2.a[i - 50] = n2.a[i];
    }
    n1.size += 50;
    n2.size -= 50;
    in1.high = n1.a[n1.size + 49];
    in2.low = n2.a[0];
    writeNode(n1, in1.pos);
    writeNode(n2, in2.pos);
    writeNodeIndex(in1,in1.pos_index);
    writeNodeIndex(in2,in2.pos_index);
}

int main() {
    // freopen("0.in", "r", stdin);
    // freopen("wrong.out","w",stdout);
    // b.txt用来存所有数据，a.txt用来存每个节点的最大元素
    BlockChain blockchain("b.txt","a.txt");
    int command_count;
    std::cin >> command_count;
    for (int i = 0; i < command_count; ++i) {
        std::string command;
        std::cin >> command;
        if (command == "insert") {
            std::string index;
            int value;
            std::cin >> index >> value;
            if(index=="a" && value==100){
                Element element(index);
                element.value = value;
                blockchain.Insert(element);
                continue;
            }
            Element element(index);
            element.value = value;
            blockchain.Insert(element);
        } else if (command == "delete") {
            std::string index;
            int value;
            std::cin >> index >> value;
            Element e(index);
            e.value = value;
            blockchain.Delete(e);
        } else if (command == "find") {
            std::string index;
            std::cin >> index;
            blockchain.Find(index);
        }
    }
    return 0;
}