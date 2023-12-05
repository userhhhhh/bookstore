#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <fstream>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

//可以直接使用 string 来定义字符串变量 myString，而不需要写成 std::string myString = "Hello, World!";

template<class T, int info_len = 2>
class MemoryRiver {
private:
    fstream file;
    string file_name;
    int sizeofT = sizeof(T);
public:
    MemoryRiver() = default;

    MemoryRiver(const string& file_name) : file_name(file_name) {}
    // ": file_name(file_name)" 是在成员初始化列表

    void initialise(string FN = "") {
        if (FN != "") file_name = FN;
        file.open(file_name, std::ios::out);
        //将流对象与文件关联，std::ios::out 说明是在向里面写东西
        int tmp = 0;
        for (int i = 0; i < info_len; ++i)
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
    }

    //读出第n个int的值赋给tmp，1_base
    void get_info(int &tmp, int n) {
        if (n > info_len) return;
        file.open(file_name,std::ios::in);
        file.seekg((n-1)*4,std::ios::beg);
        file.read(reinterpret_cast<char*>(&tmp),sizeof(int));
        file.close();
    }

    //将tmp写入第n个int的位置，1_base
    void write_info(int tmp, int n) {
        if (n > info_len) return;
        file.open(file_name,std::ios::out | std::ios::in);
        //错误：这里一定要标注std::ios::in,不能只填 std::ios::out
        file.seekp((n-1)*4,std::ios::beg);
        file.write(reinterpret_cast<char*>(&tmp),sizeof(int));
        file.close();
    }

    //在文件合适位置写入类对象t，并返回写入的位置索引index
    //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
    //位置索引index可以取为对象写入的起始位置
    int write(T &t) {
        file.open(file_name,std::ios::in | std::ios::out | std::ios::binary);
        file.seekp(0,std::ios::end);//这里指针指向的是最后一个字符的下一个
        int index=file.tellp();
        file.write(reinterpret_cast<char*>(&t),sizeof(T));
        file.close();
        return index;
    }

    //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
    void update(T &t, const int index) {
        file.open(file_name,std::ios::in | std::ios::out | std::ios::binary);
        file.seekp(index,std::ios::beg);
        file.write(reinterpret_cast<char*>(&t),sizeof(T));
        file.close();
    }

    //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
    void read(T &t, const int index) {
        file.open(file_name,std::ios::in | std::ios::out | std::ios::binary);
        file.seekg(index,std::ios::beg);
        file.read(reinterpret_cast<char*>(&t),sizeof(T));
        file.close();
    }

    //删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
    void Delete(int index) {

    }
};


#endif //BPT_MEMORYRIVER_HPP
//备注：参考了codemate


int main() {
    ofstream os("a.txt");
    return 0;
}