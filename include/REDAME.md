## use `bcp` to pack all boost dep files

```bash
# cd boost source dir
$ ls
INSTALL Jamroot LICENSE_1_0.txt README.md boost/ boost-build.jam boost.css
boost.png boostcpp.jam bootstrap.bat bootstrap.sh* config.cpp doc/ index.htm
index.html lib64-msvc-10.0/ libs/ more/ rst.css stage/ status/ tools/

$ mkdir outputs
$ bcp boost/function.hpp boost/tuple/tuple.hpp boost/shared_ptr.hpp boost/make_shared.hpp boost/type_traits.hpp boost/static_assert.hpp outputs

$ du outputs/boost
11.4 MB
```
