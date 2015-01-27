/**
 * jsonenc.hpp - a minimal stl2json encoder.
 */
#include<map>
#include<vector>

#include<string>

#ifndef JSONSTREAM_ESCAPE_STRINGS
#define JSONSTREAM_ESCAPE_STRINGS 1
#endif

const static struct js_newline_t {} js_newline;

template<class S>
class jsonstream {
  S& mem;
public:
  jsonstream(S& c) : mem(c) { };
  // generic operator overload for non-specialized functions
  template <typename T>
  jsonstream& operator << (const T& rhs) {
    mem << rhs;
    return *this;
  };

  jsonstream& operator << (const js_newline_t&) {
    mem << "\n";
    return *this;
  };

  // explicit write call used by overload specializations
  template <typename T>
  jsonstream& write_raw(const T &data) {
    mem << data;
    return *this;
  }
};

// Put string in quotes. TODO: Escape string more.
template<class S>
jsonstream<S>& operator<< (jsonstream<S>& lhs, const std::string &rhs) {
  lhs.write_raw("\"");
#if JSONSTREAM_ESCAPE_STRINGS
  for (size_t i = 0; i < rhs.size(); ++i) {
    if (rhs[i] == '\n') {
      lhs.write_raw("\\n");
      continue;
    }
    if (rhs[i] == '"' || rhs[i] == '\\') {
      lhs.write_raw('\\');
    }
    lhs.write_raw(rhs[i]);
  }
#else
  lhs.write_raw(rhs);
#endif
  lhs.write_raw("\"");
  return lhs;
}

template<class S>
jsonstream<S>&operator <<(jsonstream<S>&lhs, const char *rhs) {
  return lhs << std::string(rhs);
}

// std::pair
template<class S, class Key, class T>
jsonstream<S>& operator<< (jsonstream<S>& lhs, const std::pair<Key, T>& r) {
  lhs.write_raw('"');
  lhs.write_raw(r.first);
  lhs.write_raw("\": ");
  lhs << r.second;
  return lhs;
}

// std::map
template<class S, class Key, class T>
jsonstream<S>& operator<< (jsonstream<S>& lhs, const std::map<Key, T>& r) {
  lhs.write_raw("{");
  for (typename std::map<Key, T>::const_iterator it = r.begin(); it != r.end(); ++it) {
    lhs << *it;
    lhs.write_raw(",");
  }
  lhs.write_raw("}");
  return lhs;
}

// std::vector
template<class S, class T>
jsonstream<S>& operator<< (jsonstream<S>& lhs, const std::vector<T>& r) {
  lhs.write_raw("[");
  for (typename std::vector<T>::const_iterator it = r.begin(); it != r.end(); ++it) {
    lhs << *it;
    lhs.write_raw(",");
  }
  lhs.write_raw("]");
  return lhs;
}

#if __cplusplus >= 201103L
#include <tuple>
#include <type_traits>

/**
 * Add support for writing tuples of std::pair<const char*, T>
 */
template<class S, size_t idx, typename... X,
  typename std::enable_if<(idx < ((sizeof...(X))-1))>::type* = nullptr>
jsonstream<S>& jsonstream_array_tuple_inner (jsonstream<S>& lhs, const std::tuple<X...> &r) {
  lhs << std::get<idx>(r);
  lhs.write_raw(",");
  jsonstream_array_tuple_inner<S, idx+1, X...>(lhs, r);
  return lhs;
}

template<class S, size_t idx, typename... X,
  typename std::enable_if<(idx == (sizeof...(X))-1)>::type* = nullptr>
jsonstream<S>& jsonstream_array_tuple_inner (jsonstream<S>& lhs, const std::tuple<X...> &r) {
  lhs << std::get<idx>(r);
  return lhs;
}

template<class S, size_t idx, typename... X,
  typename std::enable_if<(idx >= (sizeof...(X)))>::type* = nullptr>
jsonstream<S>& jsonstream_array_tuple_inner (jsonstream<S>& lhs, const std::tuple<X...> &r) {
  return lhs;
}

template<class S, typename... X>
jsonstream<S>& operator<< (jsonstream<S>& lhs, const std::tuple<X...>&r) {
  lhs.write_raw("[");
  jsonstream_array_tuple_inner<S, 0, X...>(lhs,  r);
  lhs.write_raw("]");
  return lhs;
};

template<class S, typename T, typename...X>
jsonstream<S>& operator<< (jsonstream<S>&lhs, const std::tuple<std::pair<std::string, T>, X...>& r)
{
  lhs.write_raw("{");
  lhs << std::get<0>(r);
  if (sizeof...(X) > 0) {
    lhs.write_raw(",");
    jsonstream_array_tuple_inner<S, 1, std::pair<std::string, T>, X...>(lhs,  r);
  }
  lhs.write_raw("}");
  return lhs;
}

#endif

