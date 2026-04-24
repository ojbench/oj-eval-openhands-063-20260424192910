#ifndef BPT_MEMORYRIVER_HPP
#define BPT_MEMORYRIVER_HPP

#include <fstream>
#include <stack>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

template<class T, int info_len = 2>
class MemoryRiver {
private:
    fstream file;
    string file_name;
    int sizeofT = sizeof(T);
    // Use a stack to keep track of deleted positions for space reclamation
    std::stack<int> free_positions;
    
    // Open file for reading and writing
    void open_file() {
        file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
    }
    
    // Close file
    void close_file() {
        file.close();
    }
    
public:
    MemoryRiver() = default;

    MemoryRiver(const string& file_name) : file_name(file_name) {}

    void initialise(string FN = "") {
        if (FN != "") file_name = FN;
        file.open(file_name, std::ios::out | std::ios::binary);
        int tmp = 0;
        for (int i = 0; i < info_len; ++i)
            file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        file.close();
        // Clear the free positions stack
        std::stack<int> empty;
        free_positions.swap(empty);
    }

    //读出第n个int的值赋给tmp，1_base
    void get_info(int &tmp, int n) {
        if (n > info_len) return;
        open_file();
        file.seekg((n-1) * sizeof(int));
        file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
        close_file();
    }

    //将tmp写入第n个int的位置，1_base
    void write_info(int tmp, int n) {
        if (n > info_len) return;
        open_file();
        file.seekp((n-1) * sizeof(int));
        file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
        close_file();
    }
    
    //在文件合适位置写入类对象t，并返回写入的位置索引index
    //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
    //位置索引index可以取为对象写入的起始位置
    int write(T &t) {
        int pos;
        if (!free_positions.empty()) {
            // Reuse a previously deleted position
            pos = free_positions.top();
            free_positions.pop();
            open_file();
            file.seekp(pos);
        } else {
            // Append to the end of the file
            open_file();
            file.seekp(0, std::ios::end);
            pos = file.tellp();
        }
        file.write(reinterpret_cast<char *>(&t), sizeof(T));
        close_file();
        return pos;
    }
    
    //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
    void update(T &t, const int index) {
        open_file();
        file.seekp(index);
        file.write(reinterpret_cast<char *>(&t), sizeof(T));
        close_file();
    }
    
    //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
    void read(T &t, const int index) {
        open_file();
        file.seekg(index);
        file.read(reinterpret_cast<char *>(&t), sizeof(T));
        close_file();
    }
    
    //删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
    void Delete(int index) {
        // Add the position to the free positions stack for reuse
        free_positions.push(index);
        // We don't actually overwrite the data, just mark the space as available
        // This is sufficient for space reclamation
    }
};


#endif //BPT_MEMORYRIVER_HPP