#include <iostream>

using namespace std;

struct UniversalType {
  template <typename T>
  operator T();
};

template <typename T, typename... UTypes>
consteval size_t member_count() {
    if constexpr (requires { T{{UTypes{}}..., {UniversalType{}}}; } == true) {
        return member_count<T, UTypes..., UniversalType>();
    } else {
        return sizeof...(UTypes);
    }
}

template<typename T, typename... Args>
void Visit(T t, Args... args)
{
    if constexpr (sizeof...(args) == 0) {
        cout << t << endl;
    } else {
        cout << t << ",";
        Visit(args...);
    }
}

struct Vector {
    float x,y,z;
};

template<typename T>
void serialize(const T &t) {
    if constexpr (member_count<T>() == 0) {
    } else if constexpr (member_count<T>() == 1) {
        auto && [a01] = t;
        Visit(a01);
    } else if constexpr (member_count<T>() == 2) {
        auto && [a01, a02] = t;
        Visit(a01, a02);
    } else if constexpr (member_count<T>() == 3) {
        auto && [a01, a02, a03] = t;
        Visit(a01, a02, a03);
    }
}

int main() {
    Vector v{1.0,2.0,3.0};
    serialize(v);
}
