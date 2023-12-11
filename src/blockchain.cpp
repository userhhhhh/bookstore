#include "blockchain.h"
#include <bits/stdc++.h>
#include <streambuf>

const int MAX_SIZE=1000;
const int MIN_SIZE=400;
Element::Element(std::string str){
    std::copy(str.begin(),str.end(),index);
}
bool Element::operator<(const Element &e2)const{
    int result=std::strcmp(index,e2.index);
    if(result<0) return true;
    else if(result>0) return false;
    else if(result==0) return value<e2.value;
    return true;
}
bool Element::operator==(const Element& e2)const{
    return (!strcmp(index,e2.index) && value==e2.value);
}
bool Element::operator>(const Element& e2)const{
    int result=strcmp(index,e2.index);
    if(result<0) return false;
    else if(result>0) return true;
    else if(result==0) return value>e2.value;
    return true;
}
BlockChain::BlockChain(std::string file_name):file_name(file_name){
    file.open(file_name,std::ios::binary | std::ios::in | std::ios::out);
    if(!file.is_open()) {
        file.open(file_name, std::ios::binary | std::ios::out);
        file.close();
        file.open(file_name, std::ios::binary | std::ios::in | std::ios::out);
    }
}
BlockChain::~BlockChain(){
    if(file.is_open()) file.close();
}
void BlockChain::Insert(Element &e){
    //在一开始，先创建一个新的block块
    if(!flag){
        flag=true;
        NODE n;
        n.prev=-1;
        n.size=0;
        n.low=e;
        n.high=e;
        writeNode(n,0);
        tail=sizeof(NODE);
    }
    //下面是已经创建好block的情况
    Element e1;
    int next_pos=0;
    int cur_pos=0;
    while(true){
        file.seekg(next_pos,std::ios::beg);
        file.seekg(sizeof(Element),std::ios::cur);
        //将末尾标志元素读到e1里面
        file.read(reinterpret_cast<char*>(&e1),sizeof(Element));
        file.read(reinterpret_cast<char*>(&cur_pos),sizeof(int));
        if(e>e1){
            file.seekg(sizeof(int),std::ios::cur);
            file.read(reinterpret_cast<char*>(&next_pos),sizeof(int));
        }
        else break;
    }
    //现在已经找到了所在的block，现在seekg的位置是pos后面
    NODE n1;
    file.seekg(cur_pos,std::ios::beg);
    file.read(reinterpret_cast<char*>(&n1),sizeof(NODE));//将整个NODE读出来
    // 下面二分查找最后一个<=该元素的元素下标
    // 首先不可能有相等元素
    // while循环结束后，l的值就是我们要找的
    int l=0,r=n1.size-1;
    while(l<r){
        int mid=(l+r+1)/2;
        if(n1.a[mid]<e) l=mid;
        else r=mid-1;
    }
    for(int i=n1.size;i>=l+2;--i){
        n1.a[i]=n1.a[i-1];
    }
    n1.a[l+1]=e;
    n1.size++;
    //下面是裂块操作
    if(n1.size==MAX_SIZE){
        NODE n2;
        n2.prev=n1.pos;
        n2.next=n1.next;
        for(int i=(MAX_SIZE/2);i<=MAX_SIZE-1;++i){
            n2.a[i-MAX_SIZE/2]=n1.a[i];
        }
        n2.low=n2.a[0];
        n2.high=n1.a[MAX_SIZE-1];
        n2.size=MAX_SIZE/2;
        n2.pos=tail;
        writeNode(n2,tail);
        tail+=sizeof(NODE);
        //下面对n1操作
        n1.high=n1.a[MAX_SIZE/2-1];
        n1.size=MAX_SIZE/2;
        n1.next=n2.pos;
    }
    writeNode(n1,cur_pos);
}
void BlockChain::Delete(Element &e){
    Element e1;
    int next_pos=0;
    int cur_pos=0;
    while(true){
        file.seekg(next_pos,std::ios::beg);
        file.seekg(sizeof(Element),std::ios::cur);
        //将末尾标志元素读到e1里面
        file.read(reinterpret_cast<char*>(&e1),sizeof(Element));
        file.read(reinterpret_cast<char*>(&cur_pos),sizeof(int));
        if(e>e1){
            file.seekg(sizeof(int),std::ios::cur);
            file.read(reinterpret_cast<char*>(&next_pos),sizeof(int));
        }
        else break;
    }
    //现在已经找到了所在的block，现在seekg的位置是pos后面
    NODE n1;
    file.seekg(cur_pos,std::ios::beg);
    file.read(reinterpret_cast<char*>(&n1),sizeof(NODE));//将整个NODE读出来
    // 下面二分查找最后一个<=该元素的元素下标
    // 首先不可能有相等元素,所以只要查找到了，就只有这一个元素
    // while循环结束后，l的值就是我们要找的
    int l=0,r=n1.size-1;
    while(l<r){
        int mid=(l+r+1)/2;
        if(n1.a[mid]<e) l=mid;
        else r=mid-1;
    }
    if(n1.a[l]==e){
        for(int i=l;i<=n1.size-1;++i){
            n1.a[i]=n1.a[i+1];
        }
        n1.size--;
        writeNode(n1,cur_pos);
    }
    else std::cout<<"null"<<std::endl;
    //下面是并块或者借块操作
    if(n1.size>=MIN_SIZE) return;
    if(cur_pos==0){//头节点
        if(tail==0) return;//确保后面有节点
        NODE n2;
        file.seekg(sizeof(NODE),std::ios::beg);
        file.read(reinterpret_cast<char*>(&n2),sizeof(NODE));
        if(n2.size+n1.size<=MAX_SIZE) {
            merge(n1);
            if(tail==sizeof(NODE)) tail=0;
        }
        else borrow(n1,n2);
    }
    else if(cur_pos==tail){
        if(tail==0) return;
        NODE n2;
        file.seekg(-sizeof(NODE),std::ios::end);
        file.read(reinterpret_cast<char*>(&n2),sizeof(NODE));
        if(n2.size+n1.size<=MAX_SIZE) {
            merge(n2);
            tail=n1.pos;
        }
        else borrow(n1,n2);
    }
    else{
        NODE n2;
        file.seekg(sizeof(NODE),std::ios::beg);
        file.read(reinterpret_cast<char*>(&n2),sizeof(NODE));
        if(n2.size+n1.size<=MAX_SIZE) merge(n1);
        else borrow(n1,n2);
    }
}
void BlockChain::Find(std::string& str){
    //构造一个Element变量e2，它的value为int最小值
    Element e2(str);
    e2.value = std::numeric_limits<int>::min();
    Element e1;
    int next_pos=0;
    int cur_pos=0;
    while(true){
        file.seekg(next_pos,std::ios::beg);
        file.seekg(sizeof(Element),std::ios::cur);
        //将末尾标志元素读到e1里面
        file.read(reinterpret_cast<char*>(&e1),sizeof(Element));
        file.read(reinterpret_cast<char*>(&cur_pos),sizeof(int));
        if(e2>e1){
            file.seekg(sizeof(int),std::ios::cur);
            file.read(reinterpret_cast<char*>(&next_pos),sizeof(int));
        }
        else break;
    }
    //现在已经找到了所在的block，现在seekg的位置是pos后面
    NODE n1;
    file.seekg(cur_pos,std::ios::beg);
    file.read(reinterpret_cast<char*>(&n1),sizeof(NODE));//将整个NODE读出来
    // 下面二分查找最后一个<=该元素的元素下标
    // 首先不可能有相等元素,所以只要查找到了，就只有这一个元素
    // while循环结束后，l的值就是我们要找的
    int l=0,r=n1.size-1;
    while(l<r){
        int mid=(l+r+1)/2;
        if(n1.a[mid]<e2) l=mid;
        else r=mid-1;
    }
    bool exist=false;
    if(std::strcmp(n1.a[l].index,e2.index)==0) {
        std::cout<<e2.value<<' ';
        exist=true;
    }
    l++;
    //下面开始往后找，一直找到index不同为止
    while(true){
        while(std::strcmp(n1.a[l].index,e2.index)==0 && l<=n1.size-1){
            l++;
            std::cout<<e2.value<<' ';
            exist=true;
        }
        if(l==n1.size){
            l=0;
            int tmp=n1.next;//读出n1的下一块的位置
            file.seekp(tmp,std::ios::beg);
            file.read(reinterpret_cast<char*>(&n1),sizeof(NODE));
        }
        else break;
    }
    if(exist) std::cout<<std::endl;
    if(!exist) std::cout<<"null"<<std::endl;

}
void BlockChain::writeNode(NODE &n,int pos_in){
    file.seekp(pos_in,std::ios::beg);
    int index=file.tellp();
    n.pos=pos_in;
    file.write(reinterpret_cast<char*>(&n),sizeof(n));
}
//表示这一块跟它后面的一块合并
void BlockChain::merge(NODE &n){
    NODE n1;//后面的块
    file.seekp(n.pos+sizeof(NODE),std::ios::beg);
    file.read(reinterpret_cast<char*>(&n1),sizeof(NODE));
    int _size1=n.size;
    int _size2=n1.size;
    for(int i=_size1;i<_size1+_size2;++i){
        n.a[i]=n1.a[i-_size1];
    }
    n.size=_size1+_size2;
    n.high=n.a[n.size-1];
    n.next=n1.next;
    writeNode(n,n.pos);
    file.seekp(n.pos+2*sizeof(NODE)+2*sizeof(Element)+sizeof(int),std::ios::beg);
    int tmp=n.pos;
    file.write(reinterpret_cast<char*>(&tmp),sizeof(int));
}
void BlockChain::borrow(NODE &n1,NODE &n2){
    // n1的元素太少，跟n2借50个
    int size_n1=n1.size;
    int size_n2=n2.size;
    for(int i=0;i<50;++i){
        n1.a[size_n1+i]=n2.a[i];
    }
    for(int i=50;i<size_n2;++i){
        n2.a[i-50]=n2.a[i];
    }
    n1.size+=50;
    n2.size-=50;
    n1.high=n1.a[size_n1+49];
    n2.low=n2.a[0];
    writeNode(n1,n1.pos);
    writeNode(n2,n2.pos);
}
