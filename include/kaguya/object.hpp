// Copyright satoren
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <string>
#include <cstring>
#include <typeinfo>

#include "kaguya/config.hpp"
#include "kaguya/traits.hpp"
#include "kaguya/exception.hpp"

namespace kaguya
{
	namespace types
	{
		template<typename T>
		struct typetag {};
	}

#define KAGUYA_METATABLE_PREFIX "kaguya_object_type_"
#define KAGUYA_METATABLE_TYPE_NAME_KEY "_kaguya_type_name"
#define KAGUYA_METATABLE_BASE_TYPE_KEY "_kaguya_base_types"
	template<typename T>
	inline const std::string& metatableName()
	{
		typedef typename traits::remove_cv<T>::type noncv_type;
		typedef typename traits::remove_pointer<noncv_type>::type noncvpointer_type;
		typedef typename traits::remove_const_and_reference<noncvpointer_type>::type noncvpointerref_type;

		static const std::string v = std::string(KAGUYA_METATABLE_PREFIX) + typeid(noncvpointerref_type*).name();

		return v;
	}
	template<typename T>
	const std::type_info& metatableType()
	{
		typedef typename traits::remove_cv<T>::type noncv_type;
		typedef typename traits::remove_pointer<noncv_type>::type noncvpointer_type;
		typedef typename traits::remove_const_and_reference<noncvpointer_type>::type noncvpointerref_type;
		return typeid(noncvpointerref_type*);
	}

	namespace class_userdata
	{
		template<typename T>bool get_metatable(lua_State* l)
		{
			luaL_getmetatable(l, metatableName<T>().c_str());
			return !lua_isnil(l, -1);
		}
		template<typename T>bool available_metatable(lua_State* l)
		{
			util::ScopedSavedStack save(l);
			return get_metatable<T>(l);
		}
		template<typename T>bool newmetatable(lua_State* l)
		{
			if (luaL_newmetatable(l, metatableName<T>().c_str()))
			{
#if LUA_VERSION_NUM < 503
				lua_pushstring(l, metatableName<T>().c_str());
				lua_setfield(l, -2, "__name");
#endif
				lua_pushstring(l, metatableName<T>().c_str());
				lua_setfield(l, -2, KAGUYA_METATABLE_TYPE_NAME_KEY);
				return true;
			}
			return false;
		}
		template<typename T>void setmetatable(lua_State* l)
		{
			return luaL_setmetatable(l, metatableName<T>().c_str());
		}

		template<typename T>T* test_userdata(lua_State* l, int index)
		{
			return static_cast<T*>(luaL_testudata(l, index, metatableName<T>().c_str()));
		}

		template<typename T>inline void destructor(T* pointer)
		{
			if (pointer)
			{
				pointer->~T();
			}
		}
	}
	inline bool available_metatable(lua_State* l, const char* t)
	{
		util::ScopedSavedStack save(l);
		luaL_getmetatable(l, t);
		return !lua_isnil(l, -1);
	}
	template<typename T>
	bool available_metatable(lua_State* l, types::typetag<T> type = types::typetag<T>())
	{
		return class_userdata::available_metatable<T>(l);
	}

	struct ObjectWrapperBase
	{
		virtual bool is_native_type(const std::string& type) = 0;

		virtual const void* native_cget() = 0;
		virtual void* native_get() = 0;
		virtual const void* cget() = 0;
		virtual void* get() = 0;

		virtual const std::type_info& type() = 0;
		virtual const std::string& typeName() = 0;

		virtual void addRef(lua_State* state, int index) {};

		ObjectWrapperBase() {}
		virtual ~ObjectWrapperBase() {}
	private:

		//noncopyable
		ObjectWrapperBase(const ObjectWrapperBase&);
		ObjectWrapperBase& operator=(const ObjectWrapperBase&);
	};

	template<class T>
	struct ObjectWrapper : ObjectWrapperBase
	{
		T object;

		ObjectWrapper() :object() {}
#if KAGUYA_USE_CPP11
		template<class... Args>
		ObjectWrapper(Args&&... args) : object(std::forward<Args>(args)...) {}
#else

		template<class Arg1>
		ObjectWrapper(const Arg1& v1) : object(v1) {}
		template<class Arg1, class Arg2>
		ObjectWrapper(const Arg1& v1, const Arg2& v2) : object(v1, v2) {}
		template<class Arg1, class Arg2, class Arg3>
		ObjectWrapper(const Arg1& v1, const Arg2& v2, const Arg3& v3) : object(v1, v2, v3) {}
		template<class Arg1, class Arg2, class Arg3, class Arg4>
		ObjectWrapper(const Arg1& v1, const Arg2& v2, const Arg3& v3, const Arg4& v4) : object(v1, v2, v3, v4) {}
		template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
		ObjectWrapper(const Arg1& v1, const Arg2& v2, const Arg3& v3, const Arg4& v4, const Arg5& v5) : object(v1, v2, v3, v4, v5) {}
		template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6>
		ObjectWrapper(const Arg1& v1, const Arg2& v2, const Arg3& v3, const Arg4& v4, const Arg5& v5, const Arg6& v6) : object(v1, v2, v3, v4, v5, v6) {}
		template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7>
		ObjectWrapper(const Arg1& v1, const Arg2& v2, const Arg3& v3, const Arg4& v4, const Arg5& v5, const Arg6& v6, const Arg7& v7) : object(v1, v2, v3, v4, v5, v6, v7) {}
		template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8>
		ObjectWrapper(const Arg1& v1, const Arg2& v2, const Arg3& v3, const Arg4& v4, const Arg5& v5, const Arg6& v6, const Arg7& v7, const Arg8& v8) : object(v1, v2, v3, v4, v5, v6, v7, v8) {}
		template<class Arg1, class Arg2, class Arg3, class Arg4, class Arg5, class Arg6, class Arg7, class Arg8, class Arg9>
		ObjectWrapper(const Arg1& v1, const Arg2& v2, const Arg3& v3, const Arg4& v4, const Arg5& v5, const Arg6& v6, const Arg7& v7, const Arg8& v8, const Arg9& v9) : object(v1, v2, v3, v4, v5, v6, v7, v8, v9) {}

#endif

		virtual bool is_native_type(const std::string& type)
		{
			return metatableName<T>() == type;
		}
		virtual const std::type_info& type()
		{
			return metatableType<T>();
		}
		virtual const std::string& typeName()
		{
			return metatableName<T>();
		}

		virtual void* get()
		{
			return &object;
		}
		virtual const void* cget()
		{
			return &object;
		}
		virtual const void* native_cget() { return cget(); };
		virtual void* native_get() { return get(); };
	};

	template<class T>
	struct ObjectSmartPointerWrapper : ObjectWrapperBase
	{
		T object;

		ObjectSmartPointerWrapper(const T& sptr) :object(sptr) {}
#if KAGUYA_USE_RVALUE_REFERENCE
		ObjectSmartPointerWrapper(T&& sptr) : object(std::move(sptr)) {}
#endif


		virtual bool is_native_type(const std::string& type)
		{
			return metatableName<T>() == type;
		}
		virtual const std::type_info& type()
		{
			return metatableType<T::element_type>();
		}
		virtual const std::string& typeName()
		{
			return metatableName<T::element_type>();
		}
		virtual void* get()
		{
			return object.get();
		}
		virtual const void* cget()
		{
			return object.get();
		}
		virtual const void* native_cget() { return &object; };
		virtual void* native_get() { return &object; };
	};

	template<class T>
	struct ObjectPointerWrapper : ObjectWrapperBase
	{
		T* object;

		ObjectPointerWrapper(T* ptr) :object(ptr) {}

		virtual bool is_native_type(const std::string& type)
		{
			return metatableName<T>() == type;
		}
		virtual const std::type_info& type()
		{
			return metatableType<T>();
		}
		virtual const std::string& typeName()
		{
			return metatableName<T>();
		}
		virtual void* get()
		{
			if (traits::is_const<T>::value)
			{
				return 0;
			}
			return const_cast<void*>(static_cast<const void*>(object));
		}
		virtual const void* cget()
		{
			return object;
		}
		virtual const void* native_cget() { return cget(); };
		virtual void* native_get() { return get(); };


		virtual void addRef(lua_State* state, int index) { retain_ref_.push_back(util::Ref(state, index)); };
	private:
		std::vector<util::Ref> retain_ref_;
	};

	//for internal use
	struct PointerConverter
	{
		typedef void* (*convert_function_type)(void*);
		typedef std::pair<std::string, std::string> convert_map_key;

		void add_function(const std::string& to_type, const std::string& from_type, convert_function_type f)
		{
			size_t findex = cvt_function_list_.size();
			cvt_function_list_.push_back(f);

			std::map<convert_map_key, std::vector<size_t> > add_map;
			for (std::map<convert_map_key, std::vector<size_t> >::iterator it = function_index_map_.begin();
			it != function_index_map_.end(); ++it)
			{
				if (it->first.first == from_type)
				{
					std::vector<size_t> newlist = it->second;
					newlist.push_back(findex);
					add_map[convert_map_key(to_type, it->first.second)] = newlist;
				}

				if (it->first.second == to_type)
				{
					std::vector<size_t> newlist;
					newlist.push_back(findex);
					newlist.insert(newlist.end(), it->second.begin(), it->second.end());
					add_map[convert_map_key(it->first.first, from_type)] = newlist;
				}
			}
			function_index_map_.insert(add_map.begin(), add_map.end());

			std::vector<size_t> flist; flist.push_back(findex);
			function_index_map_[convert_map_key(to_type, from_type)] = flist;
		}

		template<typename TO>
		TO* get_pointer(ObjectWrapperBase* from)const
		{
			if (metatableName<TO>() == from->typeName())
			{
				return static_cast<TO*>(from->get());
			}
			std::map<convert_map_key, std::vector<size_t> >::const_iterator match = function_index_map_.find(convert_map_key(metatableName<TO>(), from->typeName()));
			if (match != function_index_map_.end())
			{
				return static_cast<TO*>(pcvt_list_apply(from->get(), match->second));
			}
			return 0;
		}
		template<typename TO>
		const TO* get_const_pointer(ObjectWrapperBase* from)const
		{
			if (metatableName<TO>() == from->typeName())
			{
				return static_cast<const TO*>(from->cget());
			}
			std::map<convert_map_key, std::vector<size_t> >::const_iterator match = function_index_map_.find(convert_map_key(metatableName<TO>(), from->typeName()));
			if (match != function_index_map_.end())
			{
				return static_cast<const TO*>(pcvt_list_apply(const_cast<void*>(from->cget()), match->second));
			}
			return 0;
		}


		static int deleter(lua_State *state)
		{
			PointerConverter* ptr = (PointerConverter*)lua_touserdata(state, 1);
			ptr->~PointerConverter();
			return 0;
		}

		static PointerConverter& get(lua_State* state)
		{
			util::ScopedSavedStack save(state);
			lua_pushlightuserdata(state, &get);
			lua_gettable(state, LUA_REGISTRYINDEX);
			if (lua_isuserdata(state, -1))
			{
				return *static_cast<PointerConverter*>(lua_touserdata(state, -1));
			}
			else
			{
				void* ptr = lua_newuserdata(state, sizeof(PointerConverter));//dummy data for gc call
				if (!ptr) { throw std::runtime_error("critical error. maybe failed memory allocation"); }//critical error
				PointerConverter* converter = new(ptr) PointerConverter();
				if (!converter) { throw std::runtime_error("critical error. maybe failed memory allocation"); }//critical error

				lua_createtable(state, 0, 0);
				lua_pushcclosure(state, &deleter, 0);
				lua_setfield(state, -2, "__gc");
				lua_pushvalue(state, -1);
				lua_setfield(state, -2, "__index");
				lua_setmetatable(state, -2);//set to userdata
				lua_pushlightuserdata(state, &get);
				lua_pushvalue(state, -2);
				lua_settable(state, LUA_REGISTRYINDEX);
				return *converter;
			}
		}
	private:

		void* pcvt_list_apply(void* ptr, const std::vector<size_t>& flist)const
		{
			for (std::vector<size_t>::const_iterator i = flist.begin();i != flist.end(); ++i)
			{
				ptr = cvt_function_list_[*i](ptr);
			}
			return ptr;
		}


		PointerConverter() {}

		std::vector<convert_function_type> cvt_function_list_;
		std::map<convert_map_key, std::vector<size_t> > function_index_map_;
		PointerConverter(PointerConverter&);
		PointerConverter& operator=(PointerConverter&);
	};

	inline bool recursive_base_type_check(lua_State* l, int index, const std::string& require_type)
	{
		if (lua_getmetatable(l, index))
		{
			lua_getfield(l, -1, "__name");
			const char* metatable_name = lua_tostring(l, -1);
			if (metatable_name)
			{
				if (require_type == metatable_name)
				{
					return true;
				}
				return recursive_base_type_check(l, -2, require_type);
			}
		}
		return false;
	}
	namespace detail
	{
		inline bool object_type_check(lua_State* l, int index, const std::string& require_type)
		{
			if (lua_getmetatable(l, index))
			{
				lua_getfield(l, -1, KAGUYA_METATABLE_TYPE_NAME_KEY);
				const char* metatable_name = lua_tostring(l, -1);
				if (metatable_name)
				{
					return true;
				}
			}
			return false;
		}
	}

	inline ObjectWrapperBase* object_wrapper(lua_State* l, int index, const std::string& require_type = std::string())
	{
		if (lua_type(l, index) == LUA_TUSERDATA)
		{
			util::ScopedSavedStack save(l);

			void* ptr = lua_touserdata(l, index);
			if (static_cast<ObjectWrapperBase*>(ptr)->is_native_type(require_type))
			{
				return static_cast<ObjectWrapperBase*>(ptr);
			}
			else if (require_type.empty() || detail::object_type_check(l, index, require_type))
			{
				return static_cast<ObjectWrapperBase*>(ptr);
			}
		}
		return 0;
	}
	template<class T>
	T* get_pointer(lua_State* l, int index, types::typetag<T> tag)
	{
		int type = lua_type(l, index);

		if (type == LUA_TLIGHTUSERDATA)
		{
			return (T*)lua_topointer(l, index);
		}
		else if (type != LUA_TUSERDATA)
		{
			return 0;
		}
		else
		{
			ObjectWrapperBase* objwrapper = object_wrapper(l, index, metatableName<T>());
			if (objwrapper)
			{
				if (static_cast<ObjectWrapperBase*>(objwrapper)->is_native_type(metatableName<T>()))
				{
					return static_cast<T*>(objwrapper->native_get());
				}
				else if (objwrapper->typeName() == metatableName<T>())
				{
					return static_cast<T*>(objwrapper->get());
				}
				else
				{
					PointerConverter& pcvt = PointerConverter::get(l);
					return pcvt.get_pointer<T>(objwrapper);
				}
			}
		}
		return 0;
	}
	template<class T>
	const T* get_const_pointer(lua_State* l, int index, types::typetag<T> tag)
	{
		int type = lua_type(l, index);

		if (type == LUA_TLIGHTUSERDATA)
		{
			return (T*)lua_topointer(l, index);
		}
		else if (type != LUA_TUSERDATA)
		{
			return 0;
		}
		else
		{
			ObjectWrapperBase* objwrapper = object_wrapper(l, index, metatableName<T>());
			if (objwrapper)
			{
				if (objwrapper->is_native_type(metatableName<T>()))
				{
					return static_cast<const T*>(objwrapper->native_cget());
				}
				else if (objwrapper->typeName() == metatableName<T>())
				{
					return static_cast<const T*>(objwrapper->cget());
				}
				else
				{
					PointerConverter& pcvt = PointerConverter::get(l);
					return pcvt.get_const_pointer<T>(objwrapper);
				}
			}
		}
		return 0;
	}
};
