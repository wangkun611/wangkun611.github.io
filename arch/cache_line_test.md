---
title: 测试CPU缓存行
date: 2023-04-08
draft: false
---

测试环境
1. MacBook Pro (13-inch, 2017, Two Thunderbolt 3 ports)
2. MacOS 12.6
3. 处理器 2.3GHz 双核 Intel Core i5
4. 内存 16 GB 2133 MHz LPDDR3
5. Apple clang version 13.1.6 (clang-1316.0.21.2.3)

## 概要
禁止优化编译，命令行
````
clang++ -O0 -o cache_line_test --std=c++14 ./cache_line_test.cpp
````
每种测试执行10000000次，打印出`time`结果。

读取内存函数：
````C++
auto thread_read_proc = [](char *buff, int size) {
    for (int i = 0; i < run_count; ++i) {
        for (int j = 0; j < size; j++) {
            char c = buff[j];
        }
    }
};
````

写入内存函数：
````C++
auto thread_write_proc = [](char *buff, int size) {
    for (int i = 0; i < run_count; ++i) {
        for (int j = 0; j < size; j++) {
            buff[j]++;
        }
    }
};
````

完整的测试代码参考[cache_line_test.cpp](cache_line_test.cpp)

## 测试一
单线程读取内存，代码如下
```C++
// 测试读取
void test1() {
    char __attribute__((aligned(64))) buff[64];
    thread_read_proc(buff, countof(buff));
}
```
执行结果如下：
````
time ./cache_line_test 1

real	0m0.930s
user	0m0.914s
sys	0m0.006s
````

## 测试二
单线程写入，代码如下：
````C++
// 测试写入
void test2() {
    char __attribute__((aligned(64))) buff[65];

    thread_write_proc(buff, countof(buff));
}
````
执行结果如下：
````
time ./cache_line_test 2

real	0m1.010s
user	0m0.980s
sys	0m0.007s
````

内存写入稍微比读取要慢一点。

## 测试三
多线程读取，代码如下：
````C++
// 测试多线程读取
void test3() {
    char __attribute__((aligned(64))) buff[64];

    std::thread thread1(thread_read_proc, buff, countof(buff));
    std::thread thread2(thread_read_proc, buff, countof(buff));
    thread1.join();
    thread2.join();
}
````
执行结果如下：
````
time ./cache_line_test 3

real	0m0.975s
user	0m1.894s
sys	0m0.009s
````

和单线程读取时间差不多

## 测试四
多线程(一个读取，一个写入),代码如下：
````C++
// 测试多线程:1个读，1个写
void test4() {
    char __attribute__((aligned(64))) buff[64];

    std::thread thread1(thread_read_proc, buff, countof(buff));
    std::thread thread2(thread_write_proc, buff, countof(buff));
    thread1.join();
    thread2.join();
}
````
执行结果如下：
````
time ./cache_line_test 4

real	0m2.756s
user	0m5.301s
sys	0m0.014s
````
明显比其他测试话费更多时间。写入时，缓存行失效，CPU核心花费大量时间同步数据。

## 测试五
多线程（双写），代码如下：
````C++
// 测试多线程:双写
void test5() {
    char __attribute__((aligned(64))) buff[64];

    std::thread thread1(thread_write_proc, buff, countof(buff));
    std::thread thread2(thread_write_proc, buff, countof(buff));
    thread1.join();
    thread2.join();
}
````
执行结果如下：
````
time ./cache_line_test 5

real	0m4.913s
user	0m9.679s
sys	0m0.027s
````
明显花费更多时间。两个核心互相等待，需要更多的时间。

## 测试六
多线程（双写,不同的缓存行），代码如下：
````C++
// 测试多线程:双写,不同的缓存行
void test6() {
    char __attribute__((aligned(64))) buff1[64];
    char __attribute__((aligned(64))) buff2[64];

    std::thread thread1(thread_write_proc, buff1, countof(buff1));
    std::thread thread2(thread_write_proc, buff2, countof(buff2));
    thread1.join();
    thread2.join();
}
````
测试结果如下：
````
time ./cache_line_test 6

real	0m1.042s
user	0m2.010s
sys	0m0.011s
````
明显比测试五花费更少的时间，说明缓存行隔离是有效的。

## 结论

通过缓存行隔离让测试有了非常大的性能提升。