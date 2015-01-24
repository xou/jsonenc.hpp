/**
 * jsonenc.hpp - a minimal stl2json encoder.
 */
#include<map>
#include<vector>

#include<string>

#ifndef JSONSTREAM_ESCAPE_STRINGS
#define JSONSTREAM_ESCAPE_STRINGS 1
#endif

template<class S>
class jsonstream : public S {
  S& mem;
public:
  jsonstream(S& c) : mem(c) { };
  template <typename T>
  jsonstream& operator << (const T& rhs) {
    mem << rhs;
    return *this;
  };

  template <typename T>
  jsonstream& write_raw(const T &data) {
    mem << data;
    return *this;
  }
};

// Put string in quotes. TODO: Escape string.
template<class S>
jsonstream<S>& operator<< (jsonstream<S>& lhs, const std::string &rhs) {
  lhs << "\"";
#if JSONSTREAM_ESCAPE_STRINGS
  for (size_t i = 0; i < rhs.size(); ++i) {
    if (rhs[i] == '"' || rhs[i] == '\\') {
      lhs.write_raw('\\');
    }
    lhs.write_raw(rhs[i]);
  }
#else
  lhs.write_raw(rhs);
#endif
  lhs << "\"";
  return lhs;
}

// std::pair
template<class S, class Key, class T>
jsonstream<S>& operator<< (jsonstream<S>& lhs, const std::pair<Key, T>& r) {
  lhs << '"';
  lhs.write_raw(r.first);
  lhs << "\": " << r.second;
  return lhs;
}

// std::map
template<class S, class Key, class T>
jsonstream<S>& operator<< (jsonstream<S>& lhs, const std::map<Key, T>& r) {
  lhs << "{";
  for (typename std::map<Key, T>::const_iterator it = r.begin(); it != r.end(); ++it) {
    lhs << *it << ",";
  }
  lhs << "}";
  return lhs;
}

// std::vector
template<class S, class T>
jsonstream<S>& operator<< (jsonstream<S>& lhs, const std::vector<T>& r) {
  lhs << "[";
  for (typename std::vector<T>::const_iterator it = r.begin(); it != r.end(); ++it) {
    lhs << *it << ",";
  }
  lhs << "]";
  return lhs;
}

