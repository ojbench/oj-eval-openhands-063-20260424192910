
#include <iostream>
#include <string>
#include "MemoryRiver.hpp"

// Test structure
struct TestStruct {
    int a;
    double b;
    char c;
    
    TestStruct() : a(0), b(0.0), c('0') {}
    TestStruct(int a_, double b_, char c_) : a(a_), b(b_), c(c_) {}
    
    bool operator==(const TestStruct& other) const {
        return a == other.a && b == other.b && c == other.c;
    }
};

int main() {
    // Create a MemoryRiver instance for TestStruct with info_len = 2
    MemoryRiver<TestStruct, 2> river("test_file.bin");
    
    // Initialize the file
    river.initialise();
    
    // Test write_info and get_info
    river.write_info(42, 1);
    river.write_info(123, 2);
    
    int val1, val2;
    river.get_info(val1, 1);
    river.get_info(val2, 2);
    
    if (val1 != 42 || val2 != 123) {
        std::cerr << "Error in write_info/get_info" << std::endl;
        return 1;
    }
    
    // Test writing objects
    TestStruct ts1(1, 1.1, 'a');
    TestStruct ts2(2, 2.2, 'b');
    TestStruct ts3(3, 3.3, 'c');
    
    int pos1 = river.write(ts1);
    int pos2 = river.write(ts2);
    int pos3 = river.write(ts3);
    
    // Test reading objects
    TestStruct read_ts1, read_ts2, read_ts3;
    river.read(read_ts1, pos1);
    river.read(read_ts2, pos2);
    river.read(read_ts3, pos3);
    
    if (!(read_ts1 == ts1) || !(read_ts2 == ts2) || !(read_ts3 == ts3)) {
        std::cerr << "Error in write/read" << std::endl;
        return 1;
    }
    
    // Test update
    TestStruct updated_ts(99, 99.9, 'z');
    river.update(updated_ts, pos2);
    
    TestStruct check_updated;
    river.read(check_updated, pos2);
    
    if (!(check_updated == updated_ts)) {
        std::cerr << "Error in update" << std::endl;
        return 1;
    }
    
    // Test Delete and space reclamation
    river.Delete(pos1);
    river.Delete(pos3);
    
    // Write new objects - should reuse deleted positions
    TestStruct ts4(4, 4.4, 'd');
    TestStruct ts5(5, 5.5, 'e');
    
    int pos4 = river.write(ts4);
    int pos5 = river.write(ts5);
    
    // The positions should be the same as the deleted ones if space reclamation works
    // This isn't guaranteed but likely with our stack-based approach
    // We'll just verify the data is correct
    
    TestStruct read_ts4, read_ts5;
    river.read(read_ts4, pos4);
    river.read(read_ts5, pos5);
    
    if (!(read_ts4 == ts4) || !(read_ts5 == ts5)) {
        std::cerr << "Error in write after Delete" << std::endl;
        return 1;
    }
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
