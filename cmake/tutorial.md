---
title: CMake 入门
date: 2023-04-06
draft: false
---

> 文章翻译自：[CMake Tutorial](https://cmake.org/cmake-tutorial/)

[第一步](#s1) | [第二步](#s2) | [第三步](#s3) | [第四步](#s4) | [第五步](#s5) | [第六步](#s6) | [第七步](#s7)

下面会一步一步的来教你如何使用`CMake`完成软件的构建系统。大部分章节在[Mastering CMake](http://www.kitware.com/products/books/CMakeBook.html)都有专门的介绍，但是把他们整理在一起还是非常有用的。本教程可以在`CMake`的源码目录中的[Tests/Tutorial](https://github.com/Kitware/CMake/tree/master/Tests/Tutorial)目录找到。每一步都有它自己的子目录，子目录有完整的源代码。

<a name="s1" id="s1"></a>
# 基本使用方法(第一步)
  最常见的工程都是从源代码文件构建出可执行程序。最简单的CMakeLists.txt文件只需要两行，我们就从这个文件开始。CMakeLists 文件是这样的。
```
cmake_minimum_required (VERSION 2.6)
project(Tutorial)
add_executable(Tutorial tutorial.cxx)
```
  需要说明一下本例程中CMakeLists文件都使用小写的命令。对于大写，小写，大小写混合的命令书写方法，CMake都支持。tutorial.cxx的源码用来计算一个数字的平方根，第一个版本非常简单，如下：
```
// A simple program that computes the square root of a number
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s number\n", argv[0]);
        return 1;
    }
    double inputValue = atof(argv[1]);
    double outputValue = sqrt(inputValue);
    printf("The square root of %g is %g\n", inputValue, outputValue);
    return 0;
}
```
  假如CMakeList.txt和tutorial.cxx文件都存放在`~/cmake_tutorial`目录，使用如下命令就可以构建出tutorial执行文件：
``` bash
# mkdir ~/cmake_tutorial/build
# cd ~/cmake_tutorial/build
# cmake -G "Unix Makefiles" ..
# make 
# ./Tutorial 9
The square root of 9 is 3
```

# 添加版本号和配置头文件
  我们给工程添加的第一个功能就是版本号。当然了，你也可以专门修改源代码，但是通过CMakeLists有更大的灵活性。为了增加版本号，CMakeLists 文件如下：
```
cmake_minimum_required (VERSION 2.6)
project(Tutorial)

# The version number.
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
    "${PROJECT_SOURCE_DIR}/TutorialConfig.h.in"
    "${PROJECT_BINARY_DIR}/TutorialConfig.h"
    )

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")
     
# add the executable
add_executable(Tutorial tutorial.cxx)
```
  因为我们把配置文件写入到了二进制目录中，所有，我们需要把二进制目录加入到`包含目录`中。在源代码目录中，我们新建TutorialConfig.h.in文件，内容如下：
```
// the configured options and settings for Tutorial
#define Tutorial_VERSION_MAJOR @Tutorial_VERSION_MAJOR@
#define Tutorial_VERSION_MINOR @Tutorial_VERSION_MINOR@
```
  CMake 会使用 CMakeLists 文件中的`Tutorial_VERSION_MAJOR` 和 `Tutorial_VERSION_MINOR` 值替换配置文件的`@Tutorial_VERSION_MAJOR@` 和 `@Tutorial_VERSION_MINOR@`。接下来，我们修改`Tutorial.cxx`，让它包含配置头文件，使用版本号。最后的源代码文件如下：
``` C
// A simple program that computes the square root of a number
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "TutorialConfig.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("%s Version %d.%d\n",
            argv[0],
            Tutorial_VERSION_MAJOR,
            Tutorial_VERSION_MINOR);
        printf("Usage: %s number\n", argv[0]);
        return 1;
    }
    double inputValue = atof(argv[1]);
    double outputValue = sqrt(inputValue);
    printf("The square root of %g is %g\n", inputValue, outputValue);
    return 0;
}
```
  主要的修改就是包含了`TutorialConfig.h`头文件，在使用消息中打印了版本号。
执行的命令如下：
``` bash
# cmake .. 
# make
# ./Tutorial
Tutorial Version 1.0
Usage: Tutorial number
````

<a name="s2" id="s2"></a>
# 添加一个库(第二步)
　　现在我们添加一个库到工程中。这个库是我们自己实现的求平方根。可执行文件使用我们自己的库替换由编译器提供的求平方根函数。在这个教程中，我们把库源文件放到`MathFunctions`，这个单独的目录中。只需要一行的CMakeLists就足够了：
```
add_library(MathFunctions mysqrt.cxx)
```
　　`mysqrt.cxx`文件只有一个函数叫`mysqrt`，`mysqrt`和编译器提供的`sqrt`提供相同的功能。为了使用新库，我们在顶层的CMakeLists文件中调用`add_subdirectory`，这样库才会被编译。我们还需要把`MathFunctions`目录添加到包含目录中，这样才能找到`MathFunctions/mysqrt.h`文件。最后，还需要把新库添加到可执行文件中。最终的CMakeLists的最后几行是这样的：
```
include_directories ("${PROJECT_SOURCE_DIR}/MathFunctions")
add_subdirectory (MathFunctions) 
     
# add the executable
add_executable (Tutorial tutorial.cxx)
target_link_libraries (Tutorial MathFunctions)
```
　　现在我们考虑使用`MathFunctions`作为可选项。在本教程中，没有理由需要这么做，但是，对于更大的库或依赖第三方代码的库，你可能就原因这么做了。第一步在顶层CMakeLists文件中添加一个选项：
```
# should we use our own math functions?
option (USE_MYMATH 
        "Use tutorial provided math implementation" ON) 
```
　　这会在CMake GUI中显示默认值为`ON`的选项，用户可以根据需要修改。这个配置会存储在缓存中，在用户下次CMake工程时，不需要重新配置这个选项。下一个修改是，通过配置确定是否需要编译、链接`MathFunctions`。我们把顶层CMakeLists文件的最后几行修改成这样：

```
# add the MathFunctions library?
#
if (USE_MYMATH)
  include_directories ("${PROJECT_SOURCE_DIR}/MathFunctions")
  add_subdirectory (MathFunctions)
  set (EXTRA_LIBS ${EXTRA_LIBS} MathFunctions)
endif (USE_MYMATH)
 
# add the executable
add_executable (Tutorial tutorial.cxx)
target_link_libraries (Tutorial  ${EXTRA_LIBS})
```

　　使用`USE_MYMATH`配置来确定是否需要编译和使用`MathFunctions`。使用变量(本教程中的`EXTRA_LIBS`)来收集可选库，然后，链接进可执行文件中。这是一个通用的方法来保持有很多可选库的大工程的清晰性。对源代码的修改已经很直截了当了：

``` C
// A simple program that computes the square root of a number
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "TutorialConfig.h"
#ifdef USE_MYMATH
#include "MathFunctions.h"
#endif
 
int main (int argc, char *argv[])
{
  if (argc < 2)
    {
    printf("%s Version %d.%d\n", argv[0],
            Tutorial_VERSION_MAJOR,
            Tutorial_VERSION_MINOR);
    printf(stdout,"Usage: %s number\n",argv[0]);
    return 1;
    }
 
  double inputValue = atof(argv[1]);
 
#ifdef USE_MYMATH
  double outputValue = mysqrt(inputValue);
#else
  double outputValue = sqrt(inputValue);
#endif
 
  printf("The square root of %g is %g\n",
          inputValue, outputValue);
  return 0;
}
```

　　在源码中，我们同样使用了`USE_MYMATH`。这是通过在`TutorialConfig.h.in`配置文件中添加下面的内容实现的：

    #cmakedefine USE_MYMATH

<a name="s3" id="s3"></a>
# 安装和测试(第三步)
　　在这一步中，我们给工程添加安装规则和测试支持。安装规则已经很明显了。对于`MathFunctions`库，我们需要安装库文件和头文件，在`MathFunctions/CMakeLists`中间添加下面两行：
```
install (TARGETS MathFunctions DESTINATION bin)
install (FILES MathFunctions.h DESTINATION include)
```
　　对于程序文件，我们在顶层CMakeLists文件中添加下面几行来安装可以执行文件和配置文件：
```
# add the install targets
install (TARGETS Tutorial DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/TutorialConfig.h"        
         DESTINATION include)
```
　　到此已经全部做完了。现在，你可以构建本教程，执行`make install`(或在IDE中构建`INSTALL`目标），将会安装合适的头文件、库文件、可执行文件。CMake变量`CMAKE_INSTALL_PREFIX`用来确定文件安装的根目录。添加测试也已经很明显了。在顶层CMakeLists文件的最后，我们使用一些计算好的数字来验证程序是否正确运行：
```
# does the application run
add_test (TutorialRuns Tutorial 25)
 
# does it sqrt of 25
add_test (TutorialComp25 Tutorial 25)
 
set_tests_properties (TutorialComp25 
  PROPERTIES PASS_REGULAR_EXPRESSION "25 is 5")
 
# does it handle negative numbers
add_test (TutorialNegative Tutorial -25)
set_tests_properties (TutorialNegative
  PROPERTIES PASS_REGULAR_EXPRESSION "-25 is 0")
 
# does it handle small numbers
add_test (TutorialSmall Tutorial 0.0001)
set_tests_properties (TutorialSmall
  PROPERTIES PASS_REGULAR_EXPRESSION "0.0001 is 0.01")
 
# does the usage message work?
add_test (TutorialUsage Tutorial)
set_tests_properties (TutorialUsage
  PROPERTIES 
  PASS_REGULAR_EXPRESSION "Usage:.*number")
```
　　第一个测试简单的验证程序是否可以执行，有没有段错误或其他原因导致的崩溃，并且返回值为０。这个是最基本的`CTest`测试。接下来几个测试都使用`PASS_REGULAR_EXPRESSION`测试属性来验证程序的输出是否满足特定的规则。这些测试案例都是用来验证程序计算出的平方根是否是正确的和在错误参数时，是否打印出使用方法。如果，你需要验证更多的数字，你可以考虑使用下面的宏：
```
#define a macro to simplify adding tests, then use it
macro (do_test arg result)
  add_test (TutorialComp${arg} Tutorial ${arg})
  set_tests_properties (TutorialComp${arg}
    PROPERTIES PASS_REGULAR_EXPRESSION ${result})
endmacro (do_test)
 
# do a bunch of result based tests
do_test (25 "25 is 5")
do_test (-25 "-25 is 0")
```
　　每次调用`do_test`都会添加名称、输入、结果基于参数的测试案例。

<a name="s4" id="s4"></a>
# 添加系统内省(第四步)
　　接下来，让我们根据目标平台是否支持某些特性，来增加一些代码到我们的工程。比如这个例子，我们通过判断目标平台是否支持`log`和`exp`函数，来确定是否启用代码。当然了，几乎所有的平台都支持这些函数，但对于本教授假设它们是不太常见的。如果平台有`log`函数，我们在`mysqrt`中用于记录平方根。我们首先在最上层的CMakeLists.txt里面，使用CheckFunctionExists.cmake宏来检测是否有这些行数。
```
# does this system provide the log and exp functions?
include (CheckFunctionExists)
check_function_exists (log HAVE_LOG)
check_function_exists (exp HAVE_EXP)
```
　　接着我们在TutorialConfig.h.in定义以下变量
```
// does the platform provide exp and log functions?
#cmakedefine HAVE_LOG
#cmakedefine HAVE_EXP
```
　　一定要在`log`和`exp`检测完成后，才能使用`configure_file`命令。`configure_file`命令使用当前的设置生成配置文件。最后，我们根据平台是否存在`log`和`exp`函数，来完成不同的`mysqrt`函数
```
// if we have both log and exp then use them
#if defined (HAVE_LOG) && defined (HAVE_EXP)
  result = exp(log(x)*0.5);
#else // otherwise use an iterative approach
  . . .

```
<a name="s5" id="s5"></a>
# 添加一个生成的文件和文件生成器(第五步)
　　在本章节中，我们将向你展示如何在软件的构建过程中添加一个生成的源文件。在这个例子中，我们将把预先计算好平方根的表格添加到构建过程中，并且把表格编译进软件中。为了完成这个例子，我们首先需要一个程序来生成表格。在MathFunctions的子目录，新建文件MakeTable.cxx来生成表格。
```
// A simple program that builds a sqrt table 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
 
int main (int argc, char *argv[])
{
  int i;
  double result;
 
  // make sure we have enough arguments
  if (argc < 2)
    {
    return 1;
    }
  
  // open the output file
  FILE *fout = fopen(argv[1],"w");
  if (!fout)
    {
    return 1;
    }
  
  // create a source file with a table of square roots
  fprintf(fout,"double sqrtTable[] = {\n");
  for (i = 0; i < 10; ++i)
    {
    result = sqrt(static_cast<double>(i));
    fprintf(fout,"%g,\n",result);
    }
 
  // close the table with a zero
  fprintf(fout,"0};\n");
  fclose(fout);
  return 0;
}
```
　　另外，这个表格是有效的C++代码，表格保存的文件名通过命令行参数传递。接着我们在MathFunctions的CMakeLists.txt文件中添加合适的命令来生成MakeTable可执行文件，并且在构建过程中执行它。需要几个命令来实现这个目的，如下所示：
```
# first we add the executable that generates the table
add_executable(MakeTable MakeTable.cxx)
 
# add the command to generate the source code
add_custom_command (
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Table.h
  COMMAND MakeTable ${CMAKE_CURRENT_BINARY_DIR}/Table.h
  DEPENDS MakeTable
  )
 
# add the binary tree directory to the search path for 
# include files
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
 
# add the main library
add_library(MathFunctions mysqrt.cxx ${CMAKE_CURRENT_BINARY_DIR}/Table.h  )
```
　　首先添加命令生成MakeTable的可执行文件。然后添加自定义命令指定如何使用MakeTable生成Table.h文件。接着，通过把生成的文件Table.h添加到MathFunctions库的源文件列表，我们让CMake知道mysqrt.cxx依赖生成的文件Table.h。我们还需要把当前二进制目录添加到包含目录列表中，所以mysqrt.cxx才能包含Table.h，并且在包含目录中发现Table.h。当工程被构建时，首先构建MakeTable可执行文件，接着运行MakeTable生成Table.h。最后，编译包含Table.h的mysqrt.cxx文件生成MathFunctions库。这时，所有功能已完成的最上层的CMakeList.txt文件看起来是这样的：
```
cmake_minimum_required (VERSION 2.6)
project (Tutorial)
include(CTest)
 
# The version number.
set (Tutorial_VERSION_MAJOR 1)
set (Tutorial_VERSION_MINOR 0)
 
# does this system provide the log and exp functions?
include (${CMAKE_ROOT}/Modules/CheckFunctionExists.cmake)
 
check_function_exists (log HAVE_LOG)
check_function_exists (exp HAVE_EXP)
 
# should we use our own math functions
option(USE_MYMATH 
  "Use tutorial provided math implementation" ON)
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/TutorialConfig.h.in"
  "${PROJECT_BINARY_DIR}/TutorialConfig.h"
  )
 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories ("${PROJECT_BINARY_DIR}")
 
# add the MathFunctions library?
if (USE_MYMATH)
  include_directories ("${PROJECT_SOURCE_DIR}/MathFunctions")
  add_subdirectory (MathFunctions)
  set (EXTRA_LIBS ${EXTRA_LIBS} MathFunctions)
endif (USE_MYMATH)
 
# add the executable
add_executable (Tutorial tutorial.cxx)
target_link_libraries (Tutorial  ${EXTRA_LIBS})
 
# add the install targets
install (TARGETS Tutorial DESTINATION bin)
install (FILES "${PROJECT_BINARY_DIR}/TutorialConfig.h"        
         DESTINATION include)
 
# does the application run
add_test (TutorialRuns Tutorial 25)
 
# does the usage message work?
add_test (TutorialUsage Tutorial)
set_tests_properties (TutorialUsage
  PROPERTIES 
  PASS_REGULAR_EXPRESSION "Usage:.*number"
  )
 
 
#define a macro to simplify adding tests
macro (do_test arg result)
  add_test (TutorialComp${arg} Tutorial ${arg})
  set_tests_properties (TutorialComp${arg}
    PROPERTIES PASS_REGULAR_EXPRESSION ${result}
    )
endmacro (do_test)
 
# do a bunch of result based tests
do_test (4 "4 is 2")
do_test (9 "9 is 3")
do_test (5 "5 is 2.236")
do_test (7 "7 is 2.645")
do_test (25 "25 is 5")
do_test (-25 "-25 is 0")
do_test (0.0001 "0.0001 is 0.01")
```
TutorialConfig.h.in是这样的：
```
// the configured options and settings for Tutorial
#define Tutorial_VERSION_MAJOR @Tutorial_VERSION_MAJOR@
#define Tutorial_VERSION_MINOR @Tutorial_VERSION_MINOR@
#cmakedefine USE_MYMATH
 
// does the platform provide exp and log functions?
#cmakedefine HAVE_LOG
#cmakedefine HAVE_EXP
```
MathFunctions的CMakeLists.txt如下：
```
# first we add the executable that generates the table
add_executable(MakeTable MakeTable.cxx)
# add the command to generate the source code
add_custom_command (
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Table.h
  DEPENDS MakeTable
  COMMAND MakeTable ${CMAKE_CURRENT_BINARY_DIR}/Table.h
  )
# add the binary tree directory to the search path 
# for include files
include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
 
# add the main library
add_library(MathFunctions mysqrt.cxx ${CMAKE_CURRENT_BINARY_DIR}/Table.h)
 
install (TARGETS MathFunctions DESTINATION bin)
install (FILES MathFunctions.h DESTINATION include)
```
<a name="s6" id="s6"></a>
# 构建安装包(第六步)
　　接下来，假设我们想要把我们的工程分发给其他人，以便他们可以使用它。我们想要在不同的平台分发二进制和源码。这里的安装和我们在之前的章节安装和测试(第三步)中的是有些区别的，在第三步我们编译源代码后，直接安装二进制文件。在本例子中，我们将要构建二进制安装包，可以在包管理系统中使用，例如：cygwin，debian，RPMs等等。我们使用CPack来创建平台相关的安装包。我们需要在最上层的CMakeLists.txt的尾部添加一些代码：
```
# build a CPack driven installer package
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE  
     "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set (CPACK_PACKAGE_VERSION_MAJOR "${Tutorial_VERSION_MAJOR}")
set (CPACK_PACKAGE_VERSION_MINOR "${Tutorial_VERSION_MINOR}")
include (CPack)
```
　　做起来很简单。我们从包含`InstallRequiredSystemLibraries`开始。这个模块会包含当前平台需要使用的运行时库。接下来我们设置一些`CPack`变量，比如：版权许可文件目录，软件版本信息。在本教程的开始部分，我们已经设置好了版本信息。最后我们包含`CPack`模块，它会使用这些变量和其他的系统属性去生成安装包。
　　下一步，正常的构建工程，然后运行`CPack`。建立一个二进制分发，你可以运行：
```
cpack --config CPackConfig.cmake
```
建立一个源码分发，你可以运行：
```
cpack --config CPackSourceConfig.cmake
```

<a name="s7" id="s7"></a>
# 添加仪表盘(Dashboard)支持(第七步)
　　把我们的测试结果提交到仪表盘是非常简单的。在本教程前面的步骤中，我们已经定义了一些测试用例。我们只需要运行这些测试用例，并且提交结果到仪表盘。为了支持仪表盘，我们需要在最上层的`CMakeLists.txt`中包含`CTest`模块。
```
# enable dashboard scripting
include (CTest)
```
我们需要新建`CTestConfig.cmake`，在文件中定义本项目在仪表盘中的名称。
```
set (CTEST_PROJECT_NAME "Tutorial")
```
`CTest`在执行时会读取这个文件。你可以使用`CMake`为你的项目创建一个简单的仪表盘，把目录切换到二进制目录，然后运行`ctest -D Experimental`。你的仪表盘的结果会提交到Kitware的公共仪表盘[这里](http://www.cdash.org/CDash/index.php?project=PublicDashboard)
