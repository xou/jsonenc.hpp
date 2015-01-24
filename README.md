jsonenc.hpp
===========

A minimal JSON header-only encoder for STL types. Currently supports maps, vectors, pairs and any
nested variation of those.
For example, a `map<int, vector<string>>` X; can be used direclty with the stream operator
`jsonstream << X;`
and will be converted into a JSON string such as
 will be automatically converted into an JSON string such as `{'1' : ['a', 'b', 'c']}`.
See example.cpp for more detailed usage examples ;)

It is meant as a quick and dirty helper to export structured data encapsulated in STL
containers to JSON format. It does not do any form of validation of the input data (only
minimal escaping is performed).

