#include <cstdio>
#include <cstdlib>
#include <thread>

int run_count = 10000000;

#ifndef countof
#define countof(x) sizeof(x)/sizeof(x[0])
#endif


auto thread_read_proc = [](char *buff, int size) {
    for (int i = 0; i < run_count; ++i) {
        for (int j = 0; j < size; j++) {
            char c = buff[j];
        }
    }
};

auto thread_write_proc = [](char *buff, int size) {
    for (int i = 0; i < run_count; ++i) {
        for (int j = 0; j < size; j++) {
            buff[j]++;
        }
    }
};

// 测试读取
void test1() {
    char __attribute__((aligned(64))) buff[64];
    thread_read_proc(buff, countof(buff));
}

// 测试写入
void test2() {
    char __attribute__((aligned(64))) buff[65];

    thread_write_proc(buff, countof(buff));
}

// 测试多线程读取
void test3() {
    char __attribute__((aligned(64))) buff[64];

    std::thread thread1(thread_read_proc, buff, countof(buff));
    std::thread thread2(thread_read_proc, buff, countof(buff));
    thread1.join();
    thread2.join();
}

// 测试多线程:1个读，1个写
void test4() {
    char __attribute__((aligned(64))) buff[64];

    std::thread thread1(thread_read_proc, buff, countof(buff));
    std::thread thread2(thread_write_proc, buff, countof(buff));
    thread1.join();
    thread2.join();
}

// 测试多线程:双写
void test5() {
    char __attribute__((aligned(64))) buff[64];

    std::thread thread1(thread_write_proc, buff, countof(buff));
    std::thread thread2(thread_write_proc, buff, countof(buff));
    thread1.join();
    thread2.join();
}

// 测试多线程:双写,不同的缓存行
void test6() {
    char __attribute__((aligned(64))) buff1[64];
    char __attribute__((aligned(64))) buff2[64];

    std::thread thread1(thread_write_proc, buff1, countof(buff1));
    std::thread thread2(thread_write_proc, buff2, countof(buff2));
    thread1.join();
    thread2.join();
}

//
int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: cache_line_test test_case run_count\n");
        printf("    cache_line_test 1 1000000");
        return 1;
    }

    int test_case = atoi(argv[1]);
    if (argc > 2) {
        run_count = atoi(argv[2]);
    }
    switch(test_case) {
        case 1:
            test1();
            break;
        case 2: 
            test2();
            break;
        case 3:
            test3();
            break;
        case 4:
            test4();
            break;
        case 5:
            test5();
            break;
        case 6:
            test6();
            break;
    }
    return 0;
}