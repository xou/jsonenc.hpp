/**
 * example.cpp - jsonenc.hpp example driver.
 * compile with -std=c++11
 */
#include<iostream>
#include<string>
#include <sstream>
#include "jsonenc.hpp"

class CustomType {
  std::vector<double> data1;
  std::map<std::string, int> data2;
  public:
    CustomType() {
      data1 = {0.1, 0.4, 0.9, 3.2e13};
      data2 = {{"abc", 1}, {"def", -1}};
    }

  /**
   * build a custom serializer.
   * If you're using C++11, you may also use the following syntax:
   * lhs << make_tuple( make_pair(string("data1"), rhs.data1), make_pair(string("data2"), rhs.data2) );
   */
  template<class S>
  friend jsonstream<S>& operator<<(jsonstream<S> &lhs, const CustomType &rhs) {
    lhs.write_raw("{");
    lhs << make_pair("data1", rhs.data1);
    lhs.write_raw(",");
    lhs << make_pair("data2", rhs.data2);
    lhs.write_raw("}");
    return lhs;
  }
};

int main() {
  jsonstream<std::ostream> js(std::cout);
  typedef std::map<std::string, std::map<int, std::string>> map_type;

  /**
   * Serializing a map of maps.
   * Uses C++ 11 initializer lists
   */
  map_type xm;
  xm["hello"] = {{1, "world"}};
  js << xm; // output: {"hello": {"1": "world",},}

  js << js_newline; // for debugging. "\n" would be converted to a "\\n".

  /**
   * Serializing a complex nested type
   */
  std::vector<map_type> vec = { xm, xm }; // put in a couple of copies in a vector
  vec[1]["hello data"][2] = "wor\\l\"d";
  js << vec << js_newline; // output [{"hello": {"1": "world",},},{"hello": {"1": "world",},"hello data": {"2": "wor\\l\"d",},},]

  /**
   * Custom object serialization. Not very beautiful, i admit.
   */
  CustomType a;
  js << a << js_newline; // output: {'data1':[0.1,0.4,0.9,3.2e+13,], 'data2':{"abc": 1,"def": -1,}}

  /**
   * Writing to a stringstream
   */
  std::stringstream buffer;
  jsonstream<std::stringstream> jss(buffer);
  jss << vec;
  std::cout << buffer.str() << std::endl; // output as above

  /**
   * writing a tuple - C++11
   * If the first element in the tuple is a pair whose first argument is a
   * std::string, the result will be an object { }, otherwise it will be
   * an array.
   */
  js << std::make_tuple(
      std::make_pair(std::string("Hello"), 123),
      // susbsequent pair keys don't need to be std::string
      std::make_pair("World", std::vector<int>({99,22,-1000})),
      // the implementation (currently) does not check whether all following elements are
      // actually pairs, so the follwing will result in invalid output:
      3
      ) << js_newline;
}
