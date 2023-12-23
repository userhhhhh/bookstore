# 开发文档

主体分成三个部分：blockchain、booksystem、usersystem、log

## blockchain

### 成员变量

- 块状链表中的元素是Element类
- Element类：char*表示索引，value表示这个块在真正存储它的文件的索引
- NODE类：Element合在一起构成了NODE，另外有size标注这个NODE里面有多少Element
- NODE_INDEX类：标注NODE的特征信息，就是最大元素、最小元素，pos表示在文件中的位置，这样可以与NODE关联起来
- 创建NODE_INDEX类的原因：因为查找时如果将索引与实际数据放在一起，即使读取只读取一小部分，仍然会使指针大范围地移动，导致速度变慢，所以索引类就是要将这些特征信息压缩聚集到一起
- tail：另外存了一个文件，用来指示最后一块的索引，因为不保存的话会出bug

### 成员函数

- Insert：具体找到插入那个块，然后将块内比该元素大的元素后移
- Delete：具体找到插入那个块，然后将块内比该元素大的元素前移
- split：裂块，防止块太大，超过限度就裂成两块
- merge：并块，防止块太小，超过限度就并成一块
- borrow：借块，防止无法并块，就是并块以后超出限制，那么就借块，从前面或者后面借一点元素

## booksystem

- std::string file_book_name：存储booksystem的文件名
- std::fstream file_book：存储booksystem的文件相关的流
- BlockChain isbn_chain;
- BlockChain name_chain;
- BlockChain author_chain;
- BlockChain keyword_chain;
- 上面分别建立了从isbn、name、author、keyword的块状链表
- 实际存储图书的所有信息的是file_book对应的文件

## usersyetem

- std::string file_user_name：存储用户的文件名
- std::fstream file_user：存储用户的文件相关的流
- user_chain：由ID找到索引
- std::vector login_now：现在登录的用户，维护登录栈
- std::vector login_now_select：保存选中的书的索引，与上面的vector一一对应
- User login_user：现在登录的user

## log

- std::string file_finance_name：存储日志的文件名
- std::fstream file_finance：存储日志的文件相关的流
- Insert：插入元素
- Show：展示元素
