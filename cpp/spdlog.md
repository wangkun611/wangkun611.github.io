---
title: 介绍 spdlog 日志库
date: 2023-04-03
draft: true
---

之前有个项目使用的log4cpp记录日志，项目目前已经完全采用C++11了。是时候更新下日志库了。

# spdlog的简介
spdlog是一个“非常快的”，仅使用头文件的，C++的日志库。有以下几个特点：
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

`spdlog`使用`fmt_lib`格式化日志,例如：`spdlog::error("Some error message with arg: {}", 1);`,使用`{}`作为占位符，格式化后面的参数。大括号之间可以配置格式化参数，例如 `{:d}`表示按照十进制打印，`{:x}`十六进制，`{:o}`八进制，`{:b}`二进制。详细格式说明请参考[syntax.html](https://fmt.dev/latest/syntax.html)

`spdlog`支持自定义日志格式，通过`spdlog::set_pattern`设置，例如：`spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");`,具体格式请参考[3.-Custom-formatting](https://github.com/gabime/spdlog/wiki/3.-Custom-formatting#pattern-flags)

更多内容可以参考[spdlog_example1.cpp](spdlog_example1.cpp)

## 高级日志对象

`spdlog`默认创建了stdout的日志对象，定义`SPDLOG_DISABLE_DEFAULT_LOGGER`宏可以禁止创建默认日志对象。禁止创建默认日志对象后，`spdlog::error`就无法打印日志了，需要通过`spdlog::set_default_logger(new_logger);`手动注册默认日志对象。

创建文件日志对象：
```C++
#include "spdlog/sinks/basic_file_sink.h"
auto logger = spdlog::basic_logger_mt("basic_logger", "logs/basic-log.txt");
logger->info("File logger");
```

`spdlog::basic_logger_mt`创建线程安全日志对象,`spdlog::basic_logger_st`创建单线程日志对象。如果软件的运行环境就是单线程的，建议使用单线程日志对象提供运行效率。

创建分割文件日志对象：
```C++
#include "spdlog/sinks/rotating_file_sink.h"
auto max_size = 5 * 1024 * 1024;
auto max_files = 3;
auto logger_rotating = spdlog::rotating_logger_mt("rotating_logger", "logs/rotating.txt", max_size, max_files);
logger_rotating->info("Rotating logger");
```
`max_size`表示日志文件的大小。在打印完一条日志后，`rotating_logger`检查当前文件大小是否大于`max_size`，如果大于，就执行分割操作。`max_files`表示分割文件的数量，每次分割文件时操作如下：`log.3.txt -> delete,log.2.txt -> log.3.txt,log.1.txt -> log.2.txt,log.txt -> log.1.txt, reopen log.txt`。所以，本实例的代码最多有4个log文件，总占用空间不大于 20M。

按照日期分割日志对象：
```C++
#include "spdlog/sinks/daily_file_sink.h"
// Create a daily logger - a new file is created every day on 2:30am
int rotation_hour = 2;
int rotation_minute = 30;
int max_files_daily = 3;
auto logger = spdlog::daily_logger_mt("daily_logger", "logs/daily.txt", 2, 30, false, max_files_daily);
```
日志分割时机是在打印一条日志之前，如果日志的生成时间大于等于2点30，会执行分割操作。默认的文件名格式为`daily_2023-04-04.txt`,可以使用`daily_file_format_sink_mt`创建自定义文件名的日志对象。

异步日志对象：
```C++
#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
auto async_file = spdlog::create_async<spdlog::sinks::basic_file_sink_mt>("async_file_logger", "logs/async_log.txt");
auto async_file_daily = spdlog::create_async<spdlog::sinks::daily_file_sink_mt>("async_file_logger_daily", "logs/async_log_daily.txt", 2, 30);
```
`spdlog`默认是同步记录日志的，意味着格式化、记录、文件分割都是同步的。如果，日志记录特别频繁，会影响到业务代码的执行。异步日志对象可以把日志记录、文件分割放到线程池中执行，但是日志格式化还是同步的，因此，如果有大量的格式化，也是会影响到业务代码执行。

除了上面介绍的文件相关的Sink，`spdlog`还支持非常多的Sink,例如：TCP，UDP,Kafka等等。所有的sink都放在`spdlog/sinks`这个目录，可以参考使用。


参考：
1. https://github.com/fmtlib/fmt
2. https://github.com/gabime/spdlog/wiki/3.-Custom-formatting#pattern-flags