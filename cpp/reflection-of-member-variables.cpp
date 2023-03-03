#include <iostream>
#include <vector>
#include <string>
#include <cstring>

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

template<typename T>
void serialize_to_vector(vector<char>& result, const T& d)
{
    result.reserve(result.size() + sizeof(d));
    result.insert(result.end(), (const char*)&d, (const char*)&d + sizeof(d));
}

template<>
void serialize_to_vector(vector<char>& result, const string& d)
{
    result.reserve(result.size() + sizeof(int) + d.size());
    int len = d.size();
    result.insert(result.end(), (char*)&len, (char*)&len + sizeof(len));
    result.insert(result.end(), d.begin(), d.end());
}

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

template<typename T>
void deserialize_from_vector(const vector<char>& buffer, size_t& off, T& d)
{
    memcpy((char*)&d, &buffer[off], sizeof(d));
    off += sizeof(d);
}

template<>
void deserialize_from_vector(const vector<char>& buffer, size_t& off, string& d)
{
    int len;
    deserialize_from_vector(buffer, off, len);
    if (len) {
        d.insert(d.end(), (const char*)&buffer[off], (const char*)&buffer[off + len]);
        off += len;
    }
}

template<typename T, typename... Args>
void deserialize_visit(const vector<char>& buffer, size_t& off, T& t, Args &... args)
{
    deserialize_from_vector(buffer, off, t);
    if constexpr (sizeof...(args) > 0) {
        deserialize_visit(buffer, off, args...);
    }
}

template<typename T>
T deserialize(const vector<char> &buffer) {
    T t;
    size_t off = 0;
    if constexpr (member_count<T>() == 0) {
    } else if constexpr (member_count<T>() == 1) {
        auto & [a01] = t;
        deserialize_visit(buffer, off, a01);
    } else if constexpr (member_count<T>() == 2) {
        auto & [a01, a02] = t;
        deserialize_visit(buffer, off, a01, a02);
    } else if constexpr (member_count<T>() == 3) {
        auto & [a01, a02, a03] = t;
        deserialize_visit(buffer, off, a01, a02, a03);
    }
    return t;
}



struct Vector {
    float x,y,z;
};

struct TestString {
    string a01;
};
int main() {
    Vector v1{1.0, 2.0, 3.0};
    cout << "Test POD: " << endl;
    cout << "Vector: " << v1.x << ", " << v1.y << ", " << v1.z << ", " << endl;
    vector<char> buffer = serialize(v1);
    cout << "Buffer Size: " << buffer.size() << endl;

    Vector v2 = deserialize<Vector>(buffer);
    cout << "Vector: " << v2.x << ", " << v2.y << ", " << v2.z << ", " << endl << endl;


    TestString ts1{"test"};
    cout << "Test String: " << endl;
    cout << "String: " << ts1.a01 << endl;
    buffer = serialize(ts1);
    cout << "Buffer Size: " << buffer.size() << endl;
    TestString ts2 = deserialize<TestString>(buffer);
    cout << "String: " << ts2.a01 << endl;
    return 0;
}
