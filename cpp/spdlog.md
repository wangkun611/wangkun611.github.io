---
title: 介绍 spdlog 日志库
date: 2023-04-03
draft: true
---

之前有个项目使用的log4cpp记录日志，项目目前已经完全采用C++11了。是时候更新下日志库了。

# spdlog的简介
spdlog是一个“非常快的”，仅使用头文件的，C++的日志库。有一下几个特点：
 - 使用 C++11
 - 跨平台
 - 高效
 - 易用
 - 支持多种目标和级别
 - 线程安全

# spdlog的使用
## 安装spdlog
本人使用CentOS,dnf没有`spdlog`包。从源码安装也很简单。根据官方提示
``` shell
$ git clone https://github.com/gabime/spdlog.git
$ cd spdlog && mkdir build && cd build
$ cmake .. && make -j
```

## 简单使用

首先包含头文件 `#include <spdlog/spdlog.h>`，然后可以直接使用`spdlog::info("Welcome to spdlog!");`打印log，这条语句把log打印到控制台上。

`spdlog`把日志分为以下几个级别：`trace`,`debug`,`info`,`warn`,`error`,`critical`。通过`spdlog::set_level(spdlog::level::debug);`设置打印日志的级别，比如`spdlog::level::debug`表示打印`debug`级别及其后面的所有级别。

`spdlog`提供了`SPDLOG_TRACE`,`SPDLOG_DEBUG`等等几个宏可以在编译器关闭某些日志打印。通过设置`SPDLOG_ACTIVE_LEVEL`打开对应的日志打印。

`spdlog`使用`fmt_lib`格式化日志,例如：`spdlog::error("Some error message with arg: {}", 1);`,使用`{}`作为占位符，格式化后面的参数。大括号之前可以配置格式化参数，例如 `{:d}`表示按照十进制打印，`{:x}`十六进制，`{:o}`八进制，`{:b}`二进制。

`spdlog`支持自定义日志格式，通过`spdlog::set_pattern`设置，例如：`spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");`,具体格式请参考[3.-Custom-formatting](https://github.com/gabime/spdlog/wiki/3.-Custom-formatting#pattern-flags)

更多内容可以参考[spdlog_example1.cpp](spdlog_example1.cpp)

## 打印日志到其他地方

`spdlog`默认创建了stdout的日志对象，设置`SPDLOG_DISABLE_DEFAULT_LOGGER`宏可以禁止创建默认日志对象。禁止创建默认日志对象后，`spdlog::error`就无法打印日志了，需要通过`spdlog::set_default_logger(new_logger);`手动注册默认日志对象。

创建文件日志对象：
1. 包含文件 `#include "spdlog/sinks/basic_file_sink.h"`
2. `auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");`创建日志对象。`_mt`表示创建多线程日志对象,`spdlog::basic_logger_st`创建单线程日志对象，如果软件的运行环境就是单线程的，使用单线程日志对象提供运行效率。
3. `logger->info("File logger");`打印日志

参考：
1. https://github.com/fmtlib/fmt
2. https://github.com/gabime/spdlog/wiki/3.-Custom-formatting#pattern-flags