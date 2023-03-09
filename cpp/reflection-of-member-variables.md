---
title: C++ 一种成员变量的反射方法
date: 2023-02-28
draft: false
---

虽然C++没有提供完善的内建反射机制，但是在工程领域却有强烈的反射需求。本文介绍一种通过列表初始化和结构化绑定获取成员变量，可以获取到成员变量的数量和引用，不过无法获取成员变量名。另外本方法只针对POD类型，其他复杂类型可能会失效，比如：std::vector。

## 列表初始化(List initialization)
从C++11开始可以使用初始化列表初始化对象，使用范例如下：

```C++
class Vector {
public:
    float x;
    float y;
    float z;
};

Vector v1{1.0};
Vector v2{1.0, 2.0};
Vector v3{1.0, 2.0, 3.0};
```

初始化列表的成员数量可以小于类的成员变量。我们可以利用这个特性获取类的成员变量数量。

```C++ 
Vector v1{1.0};                // 编译通过
Vector v2{1.0, 2.0};           // 编译通过
Vector v3{1.0, 2.0, 3.0};      // 编译通过
Vector v4{1.0, 2.0, 3.0, 4.0}  // 编译不通过
Vector v5{1.0, 2.0, 3.0, 4.0}  // 编译不通过
```

根据上面例子，我们可以写非常多vN代码，根据最后一个可以编译通过的代码我们就知道了类的成员变量数量。问题来了：我们怎么让编译器告诉我们v3可以通过编译，v4不可以通过编译呢？下面我们就介绍C++20引入的`requires`关键字。

## Requires 表达式

语法如下：

```
requires { requirement-seq }
requires ( parameter-list (optional) ) { requirement-seq }
```

`requirement-seq`是表达式列表，如果表达式可以编译通过，返回 `true`；无法编译通过返回 `false`(这个描述不是特别准确。有兴趣的话，请参考专门介绍`concept`和`requires`的文章)。

举例如下：

```C++
template <typename T>
bool has_abc_member() {
    if constexpr (requires (T t) { t.abc; } == true)
        return true;
    return false;
}

template <typename T>
bool has_least_one_int_member() {
    if constexpr (requires { T{int()}; } == true)
        return true;
    return false;
}
```

`has_abc_member`判断`T`是否有`public`的成员变量`abc`。

`has_least_one_int_member`判断`T`的第一个成员变量是否是`int`类型。

### 获取成员变量数量

顺着`has_least_one_int_member`的思路，我们可以写非常多的判断`int`类型成员数量的方法。

```C++
template <typename T>
bool has_least_two_int_member() {
    if constexpr (requires { T{int(), int()}; } == true)
        return true;
    return false;
}

template <typename T>
size_t member_count() {
    if constexpr (has_least_two_int_member<T>()) {
        return 2;
    } else if constexpr (has_least_one_int_member<T>()) {
        return 1;
    }
    return 0;

}
```

如果`int`成员小于3的情况，`member_count` 就可以返回正确的结果。为了适应更多的成员，可以适当的写更多个方法来判断更多的成员。

来这里后，出现了两个问题：

1. `member_count`只能判断`int`类型的成员，其他的类型怎么办
2. 到底多少个才算是适当的数量呢

我们接下来依次解决这两个问题

### 适配所有类型

C++有类型转换操作符，代码大概是这样的：

```C++
class Container
{
public:
    operator bool() {
        return !this.empty();
    }
}
```

上面代码把`Container`类型转换成`bool`类型，如果`Container`为空，返回`false`,否则返回`true`。类型转换函数支持模版。

```C++
struct UniversalType {
  template <typename T>
  operator T();
};
```

`UniversalType`可以转换成任何类型。我们使用`UniversalType`修改`has_least_one_int_member`后，如下：

```C++
template <typename T>
bool has_least_one_member() {
    if constexpr (requires { T{UniversalType()}; } == true)
        return true;
    return false;
}
```

这样`has_least_one_member`可以判断任何类型了，然后依次修改所有的判断函数。

### 精简 has_least_N_member 函数

从C++11开始，支持可变参数模板。代码例子：

```C++
template<typename... Types>
class Tuple {};

Tuple<>   t0;
Tuple<int>  t1;
Tuple<int, int> t2; 
```

可以写任意多的模板参数。根据这个特性，我们把`member_count`改成下面的代码：

```C++
template <typename T, typename... UTypes>
consteval size_t member_count() {
    if constexpr (requires { T{{UTypes{}}..., {UniversalType{}}}; } == true) {
        return member_count<T, UTypes..., UniversalType>();
    } else {
        return sizeof...(UTypes);
    }
}
```

`member_count`尝试添加一个`UniversalType`参数初始化`T`。如果成功，就添加一个模版参数调用自己，如果失败返回模板参数个数，该数量就是`T`的成员变量的数量。

至此，获取成员变量数量的关键技术已经全部打通。下面介绍怎么获取成员变量的引用。

## 结构化绑定(Structured binding)
结构化绑定是C++17的新特性，在很多其他语言也有类似的技术，比如：JavaScript的解构赋值，Python的解构等。

对应的语句如下：

```
1. attr(optional) cv-auto ref-qualifier(optional) [ identifier-list ] = expression;
2. attr(optional) cv-auto ref-qualifier(optional) [ identifier-list ] = expression;
3. attr(optional) cv-auto ref-qualifier(optional) [ identifier-list ]( expression );
```

说明如下：

|  字段   | 说明  |
|  ----  | ----  |
| attr(optional)  | C++11的新特性，[[ attribute-list ]] 给变量增加了很多属性 |
| cv-auto  | const volatile static修饰符， 变量类型必须是 auto |
|ref-qualifier(optional)|引用修饰符，可以是 &、&&|
|identifier-list|逗号分隔的标识符|
|expression|可以是数组、tuple、class等等|

`identifier-list`中标识符的数量必须和`expression`中的成员数量一致。

### 绑定数组

`expression`是个数组。`identifier-list`中的每一个`identifier`按照顺序绑定到数组的每个元素上。

```C++
int a[2] = {1, 2};
 
auto [x, y] = a;    // creates e[2], copies a into e,
                    // then x refers to e[0], y refers to e[1]
auto& [xr, yr] = a; // xr refers to a[0], yr refers to a[1]
```

### 绑定到类似tuple的类型

`expression`是个实现了`std::tuple_size<E>::value`,`std::tuple_element<i, E>::type`,`e.get<i>()`的类型。

```C++
std::tuple<float, float, float> point(1.0, 2.0, 3.0);
const auto& [x, y, z] = point;
```

### 绑定成员变量

`expression`是普通的类或者结构体，可以有继承关系，但是继承链里面只允许一个类有成员变量，然后绑定非static，public的成员。

```C++
struct Vector {
    float x;
    float y;
    float z;
}

Vector v{1.0, 2.0, 3.0};
auto [x, y, z] = v;

const auto& [rx, ry, rz] = v;
```

### 获取成员变量的通用方法

细心的读者可能已经发现了，我们可以使用`绑定成员变量`这个技术来获取成员变量。结构化绑定需要`identifier-list`中的标识符数量和`expression`中的元素数量完全一致，否则无法编译。例如：

```C++
Vector v{1.0,2.0,3.0};
const auto& [rx, ry, rz] = v;   // 数量一致，编译通过

const auto& [x1, y1] = v;       // 数量不一致，编译不通过
```

根据本文前一部分中获取到的成员变量的数量，我们可以写出下面的代码

```C++
Type t;

if constexpr (member_count<Type>() == 0) {
} else if constexpr (member_count<Type>() == 1) {
    auto && [a01] = t;
} else if constexpr (member_count<Type>() == 2) {
    auto && [a01, a02] = t;
}
```

按照模板继续追加代码，我们就可以根据成员变量的数量做不同的处理。不过，好像写了这么多，这技术在工程上有啥作用呢？我想到一种场景序列化：按照成员变量的顺序写入；反序列化：从文件中读出后按照顺序赋值给成员变量。我们还是要使用到可变参数模板来完成这个魔法, 但是绑定成员变量这一堆代码无法精简的。

```C++
template<typename T, typename... Args>
void serialize_visit(vector<char>& result, const T& t, const Args &... args)
{
    serialize_to_vector(result, t);
    if constexpr (sizeof...(args) > 0) {
        serialize_visit(result, args...);
    }
}

template<typename T>
vector<char> serialize(const T &t) {
    vector<char> result;
    if constexpr (member_count<T>() == 0) {
    } else if constexpr (member_count<T>() == 1) {
        auto && [a01] = t;
        serialize_visit(result, a01);
    } else if constexpr (member_count<T>() == 2) {
        auto && [a01, a02] = t;
        serialize_visit(result, a01, a02);
    } else if constexpr (member_count<T>() == 3) {
        auto && [a01, a02, a03] = t;
        serialize_visit(result, a01, a02, a03);
    }
    return result;
}
```

上面`serialize`方法封装了技术细节，根据需要提供不同的`Visit`就可以实现个性化功能。完整的代码参考[reflection-of-member-variables.cpp](reflection-of-member-variables.cpp)

## Demo

根据上面介绍的所有知识点，我写了个demo，支持POD、`string`类型的序列化和反序列化，通过提供更多特化的`serialize_to_vector`和`deserialize_from_vector`,从而支持更多的数据类型。Demo使用起来很简单。

```C++
    Vector v1{1.0, 2.0, 3.0};
    vector<char> buffer = serialize(v1);
    // 通过其他方式持续化

    // 反序列化
    Vector v2 = deserialize<Vector>(buffer);
```

## 参考

1. https://en.cppreference.com/w/cpp/language/list_initialization
2. https://en.cppreference.com/w/cpp/language/requires
3. https://en.cppreference.com/w/cpp/language/structured_binding
4. https://en.cppreference.com/w/cpp/language/parameter_pack
5. https://cloud.tencent.com/developer/article/1736661
6. https://github.com/alibaba/yalantinglibs/
