# Kaguya

C++ binding to Lua

-   The `standalone` branch packed all necessary Boost headers (about 11 MB),
    and it works on Visual Studio 2010 (terrible support for C++11) without any
    dependency.
-   If you just copy all boost headers, it will become
    <http://whudoc.qiniudn.com/2016/kaguya-with-boost.7z> (6.98 MB). Way to big!
-   But instead, you use [`bcp`](https://github.com/district10/bcp) to grab out necessary header files,
    it's <http://whudoc.qiniudn.com/2016/kaguya-standalone.7z> (523 KB, same as
    the `standalone` branch), it's amazing.

---

Licensed under [Boost Software License](http://www.boost.org/LICENSE_1_0.txt)

## Requirements

- Lua 5.3 (already included)
- C++03 compiler with boost library or C++11 compiler(gcc 4.8+,clang 3.4+,MSVC2015) without boost.

## Introduction

Kaguya is Lua binding library for C++

- header-file only
- seamless access to lua elements

## run test

```
mkdir build
cd build
cmake ..
make
./test_runner
```

If you want to use a non-system default library, add three option to cmake command

```
cmake -DLUA_INCLUDE_DIRS=path/to/lua/header/dir -DLUA_LIBRARY_DIRS=/abspath/to/lua/library/dir -DLUA_LIBRARIES=lualibname
```

## Usage

add "kaguya/include" directory to "header search path" of your project.

Or generate one header file.

```
cd  utils
python generate_one_header.py > ../kaguya.hpp
```

### Create Lua context

```cpp
#include "kaguya/kaguya.hpp"
int main()
{
  kaguya::State state;
  state.dofile("/path/to/script.lua")
}
```

### Or using exist lua state

```cpp
extern "C" int luaopen_modulename(lua_State *L)
{
   kaguya::State state(l);
   kaguya::LuaTable module = state.newTable();
   module["function"] = kaguya::funtion(somefunction);
   return module.push();//number of return lib
}
```

### Runnig Lua code

```cpp
kaguya::State state;
state("a = 'test'");//load and execute from string
state.dofile("path/to/luascript.lua");//load and execute from file

kaguya::LuaFunction f1 = state.loadfile("path/to/luascript.lua");//load file without execute
f1();//execute
kaguya::LuaFunction f2 = state.loadstring("a = 'test'");//load string without execute
f2();//execute
```

### Accessing values

```cpp
kaguya::State state;
state("a = \"test\"");
std::string a_value = state["a"];
assert(a_value == "test");

state["tbl"] = kaguya::NewTable();//tbl ={};
state["tbl"]["value"] = 1;//tbl.value = 1 in lua

state("assert(tbl.value == 1)");
```

### Retain Lua value from C++

LuaRef type is like a Variant type.
You can use for holding a Lua-value in native code.

```cpp
kaguya::State state;
state["a"] = "test";
LuaRef a = state["a"];
assert(a == "test");

state["tbl"] = kaguya::NewTable();//tbl ={};
LuaTable tbl = state["tbl"];//holding Lua Table
tbl["value"] = 1;//tbl.value = 1 in lua
state("assert(tbl.value == 1)");
```

## Call lua function

```cpp
int ret = state["math"]["abs"](-32);//call math.abs of Lua function
assert(ret == 32);
//or
auto ret = state["math"]["abs"].call<int>(-32);//call math.abs of Lua function
assert(ret == 32);
```

### Multiple Results from Lua

```cpp
state("multresfun =function() return 1,2,4 end");//registering multiple results function
int a, b, c;

kaguya::tie(a, b, c) = state["multresfun"]();
assert(a == 1 && b == 2 && c == 4 );
//or
std::tuple<int,int,int> result_tuple = state["multresfun"].call<std::tuple<int,int,int>>();
TEST_EQUAL(std::get<0>(result_tuple), 1);
TEST_EQUAL(std::get<1>(result_tuple), 2);
TEST_EQUAL(std::get<2>(result_tuple), 4);
//or
```

### Registering Classes

```cpp
struct ABC
{
	ABC():v(0) {}
	ABC(int value) :v(value) {}
	int value()const { return v; }
	void setValue(int v) { v_ = v; }
	void overload1() {std::cout << "call overload1"<<std::endl }
	void overload2(int) {std::cout << "call overload2"<<std::endl }
private:
	int v_;
};
state["ABC"].setClass(kaguya::UserdataMetatable<ABC>()
	.setConstructors<ABC(),ABC(int)>()
	.addFunction("get_value", &ABC::value)
	.addFunction("set_value", &ABC::setValue)
  .addOverloadedFunctions("overload", &ABC::overload1, &ABC::overload2)
	.addStaticFunction("nonmemberfun", [](ABC* self,int){return 1;})//c++11 lambda function
	);
```

```lua
--Lua
abc = ABC.new()--call default constructor
assert(0 == abc:get_value())
abc = ABC.new(42)--call (int) constructor
assert(42 == abc:get_value())
abc:set_value(30)
assert(30 == abc:get_value())
abc:overload() -- call overload1
abc:overload(1) --call overload2
```

#### Registering inheritance

```cpp
struct Base
{
	int a;
};
struct Base2
{
	int a2;
};
struct Derived:Base
{
	int b;
};
struct MultipleInheritance:Base,Base2
{
	int b;
};
int base_function(Base* b) {
	b->a = 1;
	return b->a;
}
//
kaguya::State state;
state["Base"].setClass(kaguya::UserdataMetatable<Base>()
  .addFunction("a", &Base::a)
  );
state["Derived"].setClass(kaguya::UserdataMetatable<Derived, Base>()
  .addFunction("b", &Derived::b)
  );

//can use kaguya::MultipleBase<BaseTypes...> for multiple inheritance class
state["MultipleInheritance"].setClass(kaguya::UserdataMetatable<MultipleInheritance, kaguya::MultipleBase<Base, Base2> >()
  .addFunction("b", &MultipleInheritance::b)
  );

state["base_function"] = &base_function;
Derived derived;
state["base_function"](&derived);//Base arguments function
state("assert(1 == derived:a())");//accessing Base member
```

#### Registering object instance

```cpp
state["ABC"].setClass(kaguya::UserdataMetatable<ABC>()
	.setConstructors<ABC(),ABC(int)>()
	.addFunction("get_value", &ABC::value)
	.addFunction("set_value", &ABC::setValue)
	);
	ABC abc(43);
  //register object pointer
	state["abc"] = &abc;
	state("assert(43 == abc:get_value())");
  //or copy instance
	state["copy_abc"] = abc;
	state("assert(43 == copy_abc:get_value())");
  //or registering shared instance
	state["shared_abc"] = kaguya::standard::shared_ptr<ABC>(new ABC(43));//kaguya::standard::shared_ptr is std::shared_ptr or boost::shared_ptr.
	state("assert(43 == shared_abc:get_value())");
```

#### Object lifetime

```cpp
state["Base"].setClass(kaguya::UserdataMetatable<Base>());
Base base;

//registering pointer. lifetime is same base
state["b"] = &base;
state["b"] = kaguya::standard::ref(base);

//registering copy instance. copied instance lifetime is handling in lua vm(garbage collection).
state["b"] = base;
state["b"] = static_cast<Base const&>(base);
```

### Registering function

```cpp
void c_free_standing_function(int v){std::cout <<"c_free_standing_function called:" << v << std::endl}
state["fun"] = &c_free_standing_function;
state["fun"](54); //c_free_standing_function called:54
state("fun(22)");//c_free_standing_function called:22

state["lambda"] = kaguya::function([]{std::cout << "lambda called" << std::endl;});//C++11 lambda
state("lambda()");//lambda called


state["overload"] = kaguya::overload([](int) {std::cout << "int version" << std::endl; },
	[](const std::string&) {std::cout << "string version" << std::endl; },
	[]() {std::cout << "no arg version" << std::endl; }
);
state("overload()");//no args version
state("overload(2)");//int version
state("overload('2')");//string version

```

#### Variadic arguments function

```cpp
state["va_fun"] = kaguya::function([](kaguya::VariadicArgType args) {for (auto v : args) { std::cout << v.get<std::string>() << ","; }std::cout << std::endl; });//C++11 lambda
state("va_fun(3,4,6,\"text\",6,444)");//3,4,6,text,6,444,
```

#### Multiple Results to Lua

If return type of function is tuple, it returns the multiple results to Lua

```cpp
state["multireturn"] = kaguya::function([]() { return std::tuple<int, int>(32, 34); });
state("print(multireturn())");//32    34
```

#### Coroutine

```cpp
kaguya::LuaThread cor = state.newThread();
state("corfun = function(arg)"
"coroutine.yield(arg) "
"coroutine.yield(arg*2) "
"coroutine.yield(arg*3) "
"return arg*4 "
" end");//define corouine function

kaguya::LuaFunction corfun = state["corfun"];//lua function get

//exec coroutine with function and argment
std::cout << int(cor(corfun, 3)) << std::endl;//3
std::cout << int(cor()) << std::endl;//6
//resume template argument is result type
std::cout << cor.resume<int>() << std::endl;//9
std::cout << int(cor()) << std::endl;//12

kaguya::LuaThread cor2 = state.newThread();
//3,6,9,12,
while(!cor2.isThreadDead())
{
    std::cout << cor2.resume<int>(corfun, 3) << ",";
}
```

### Automatic type conversion

std::map and std::vector will be convert to a lua-table by default

```cpp
kaguya::State s;
std::vector<int> vect = { 2,4,6,1 };
s["vect"] = vect;
s("print(type(vect))");// table
s("for i,v in ipairs(vect) do print(v) end");
```

output:

```
table
1 2
2 4
3 6
4 1
```

```cpp
kaguya::State s;
std::map<std::string, int> map = { { "apple",3 },{ "dog",5 },{"cat",124},{ "catfood",644 } };
s["map"] = map;
s("print(type(map))");// table
s("for i,v in pairs(map) do print(i,v) end");
```

output:

```
table
catfood 644
dog     5
cat     124
apple   3
```

#### Type conversion customize

If you want to customize the conversion to type of lua yourself ,implement specialize of kaguya::lua_type_traits

example: this code is default implements for std::string

```cpp
template<>	struct lua_type_traits<std::string> {
typedef std::string get_type;
typedef const std::string& push_type;

static bool strictCheckType(lua_State* l, int index)
{
    return lua_type(l, index) == LUA_TSTRING;
}
static bool checkType(lua_State* l, int index)
{
    return lua_isstring(l, index) != 0;
}
static get_type get(lua_State* l, int index)
{
    size_t size = 0;
    const char* buffer = lua_tolstring(l, index, &size);
    return std::string(buffer, size);
}
static int push(lua_State* l, push_type s)
{
    lua_pushlstring(l, s.c_str(), s.size());
    return 1;
}
};
```

#### Handling Errors

Lua error encountered will write to the console by default, and it is customizable:

```cpp
void HandleError(int errCode, const char * szError)
{  //customize your error handling, eg. write to file...
}
kaguya::State l;
l.setErrorHandler(HandleError);
l.dofile("./scripts/custom.lua"); // eg. accesing a file not existed will invoke HandleError above
```
