#ifndef __FUNCTION_TRAITS_H__
#define __FUNCTION_TRAITS_H__

/************************************************************************/
/* @file	: function_traits.hpp										*/
/* @brief	: 函数类型萃取类												*/
/*                                                                      */
/* @author : zhouguangming												*/
/* @date   : 2022-01-07													*/
/* @version: 0.1														*/
/*                                                                      */
/* @par Copyright(C) : 													*/
/*       2015-2022 迈克医疗电子有限公司, all rights reserved.				*/
/*                                                                      */
/* @par History :														*/
/* @li          2022-01-07  新建文件										*/
/************************************************************************/

#include <functional>
#include <tuple>
#include <type_traits>


//< 模板类定义(forward declaration): 将可调用对象转换为std::function<>和函数指针
template<typename T>
struct function_traits;

//<
//< 普通函数的模板类特化
//< 模板参数Ret: 函数返回类型
//<
template<typename Ret, typename... Args>
struct function_traits<Ret(Args...)>
{
public:
	enum { arity = sizeof...(Args) };							//< 函数形参个数
	typedef Ret function_type(Args...);							//< 函数类型
	typedef Ret return_type;									//< 返回值类型
	typedef Ret(*pointer)(Args...);								//< 函数指针
	using stl_function_type = std::function<function_type>;		//< 函数的std::function类型

	//< 用于获取指定索引的形参类型
	//< 在C++17(and later)中, 需要添加"template"关键字使"args"成为依赖型模板名称(Dependent Names of Templates)
	//< 用法: using T = typename function_traits<F>::template args<0>::type; 
	template<size_t I>
	struct args
	{
		static_assert(I < arity, "[error] index is out of range, index must less than sizeof Args.");
		using type = typename std::tuple_element<I, std::tuple<Args...>>::type;
	};

	typedef std::tuple<std::remove_cv_t<std::remove_reference<Args>>...> tuple_type;
	typedef std::tuple<std::remove_const<std::remove_reference<Args>>...> bare_tuple_type;

	//< 获取指定参数的类型
	template<size_t N>
	using arg_type = typename std::tuple_element_t<N, std::tuple<Args...>>;
};


//<
//< 模板类特化(函数指针)
//< 模板参数Ret: 函数返回值类型
//< 
template<typename Ret, typename... Args>
struct function_traits<Ret(*)(Args...)> : function_traits<Ret(Args...)>
{
};

//<
//< 模板类特化(std::function)
//< 模板参数Ret: 函数返回值类型
//< 
template<typename Ret, typename... Args>
struct function_traits<std::function<Ret(Args...)>> : function_traits<Ret(Args...)>
{
};

//<
//< 模板类特化(仿函数/函数对象)
//< 模板参数Callable: 仿函数名称
//< 
template<typename Callable>
struct function_traits : function_traits<decltype(&Callable::operator())>
{
};


//<
//< 模板类特化(类成员函数)
//< 模板参数ReturnType: 成员函数返回值类型
//< 模板参数ClassType: 类名称
//< 
#define FUNCTION_TRAITS(...) \
template<typename ReturnType, typename ClassType, typename... Args> \
struct function_traits<ReturnType(ClassType::*)(Args...) __VA_ARGS__> : function_traits<ReturnType(Args...)> \
{\
};

FUNCTION_TRAITS()
FUNCTION_TRAITS(const)
FUNCTION_TRAITS(volatile)
FUNCTION_TRAITS(const volatile)


/************************************************************************/
/* @brief																*/
/*		 获得指定函数的std::function类型									*/
/*		 模板参数Function: 函数类型, 可以为函数指针、std::function、			*/
/*		                  仿函数、类成员函数以及lambda表达式				*/
/*                                                                      */
/* @param[in] : f (函数指针、std::function、仿函数、类成员函数)				*/
/*                                                                      */
/* @return : std::function类型的函数对象									*/
/*                                                                      */
/* @par History :														*/
/* @li  zhou.guangming     2022-01-11  新建函数							*/
/************************************************************************/
template<typename Function>
typename function_traits<Function>::stl_function_type to_function(const Function& f)
{
	return static_cast<typename function_traits<Function>::stl_function_type>(f);
}


/************************************************************************/
/* @brief																*/
/*		 获得指定函数的std::function类型									*/
/*		 模板参数Function: 函数类型, 可以为函数指针、std::function、			*/
/*		                  仿函数、类成员函数以及lambda表达式				*/
/*                                                                      */
/* @param[in] : f (函数指针、std::function、仿函数、类成员函数)				*/
/*                                                                      */
/* @return : std::function类型的函数对象									*/
/*                                                                      */
/* @par History :														*/
/* @li  zhou.guangming     2022-01-11  新建函数							*/
/************************************************************************/
template<typename Function>
typename function_traits<Function>::stl_function_type to_function(Function&& f)
{
	return static_cast<typename function_traits<Function>::stl_function_type>(std::forward<Function>(f));
}


/************************************************************************/
/* @brief																*/
/*		 获得指定函数的函数指针类型											*/
/*		 模板参数Function: 函数类型, 可以为函数指针、std::function、			*/
/*		                  仿函数、类成员函数以及lambda表达式				*/
/*                                                                      */
/* @param[in] : f (函数指针、std::function、仿函数、类成员函数)				*/
/*                                                                      */
/* @return : 函数指针类型													*/
/*                                                                      */
/* @par History :														*/
/* @li  zhou.guangming     2022-01-11  新建函数							*/
/************************************************************************/
template<typename Function>
typename function_traits<Function>::pointer to_function_pointer(const Function& f)
{
	return static_cast<typename function_traits<Function>::pointer>(f);
}


template<typename Function>
typename function_traits<Function>::pointer to_function_pointer(Function&& f)
{
	return static_cast<typename function_traits<Function>::pointer>(std::forward<Function>(f));
}


//////////////////////////////////////////////////////////////////////////
//< 函数封装器

template<typename Function, typename... Args>
inline auto func_wrapper(Function&& f, Args &&... args) -> decltype(f(std::forward<Args>(args)...))
{
	return f(std::forward<Args>(args)...);
}

#endif	//< __FUNCTION_TRAITS_H__



