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
   * This is, obviously, not too beautiful at the moment. It might be possible to build on
   * std::tuple, such that one could write
   * lhs << make_tuple( make_pair("data1", rhs.data1), make_pair("data2", rhs.data2) );
   * but i'm not sure whether i should (and how i could) assert that all tuple elements
   * are pairs, and if i shouldn't, how i should handle mixed tuples.
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
  js << xm << "\n"; // output: {"hello": {"1": "world",},}

  /**
   * Serializing a complex nested type
   */
  std::vector<map_type> vec = { xm, xm }; // put in a couple of copies in a vector
  vec[1]["hello data"][2] = "wor\\l\"d";
  js << vec; // output [{"hello": {"1": "world",},},{"hello": {"1": "world",},"hello data": {"2": "wor\\l\"d",},},]

  /**
   * Notes on string serialization:
   * const char[] is not escaped (would probably be possible, but would require more code to prevent
   * infinite recursion).
   */
  js << "\n" << "Hello, world" << "\n"; // bad, will be written without quotes.
  js << std::string("Hello, World") << "\n"; // Will be in quotes.

  /**
   * Custom object serialization. Not very beautiful, i admit.
   */
  CustomType a;
  js << a << "\n"; // output: {'data1':[0.1,0.4,0.9,3.2e+13,], 'data2':{"abc": 1,"def": -1,}}

  /**
   * Writing to a stringstream
   */
  std::stringstream buffer;
  jsonstream<std::stringstream> jss(buffer);
  jss << vec;
  std::cout << buffer.str() << std::endl; // output as above
}
