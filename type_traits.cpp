#include <iostream>
using std::cout;
using std::endl;

#include <string>

#include <type_traits>
#include <functional>
#include <vector>


//< ref to : https://www.cnblogs.com/qicosmos/p/4480460.html

#include "function_traits.hpp"
#include "index_traits.hpp"
#include "result_of.hpp"
#include "type_name.hpp"
#include "Variant.hpp"



#if 1

//< function
int fn(int x)
{
	return x;
}

//< function reference
typedef int(&fn_ref)(int);

//< function pointer
typedef int(*fn_ptr)(int);

//< function-like class(function object)
struct fn_class
{
	int operator()(int x)
	{
		return x;
	}
};


//////////////////////////////////////////////////////////////////////////

//< std::decay 除了移除普通类型的cv符的作用外, 还可以将函数类型转换为函数指针类型, 
//< 从而将函数指针变量保存起来, 以便后续延迟执行
template<typename F>
struct SimpleFunction
{
	using FnType = typename std::decay<F>::type;	//< 先移除引用再添加指针

	SimpleFunction(F& f) : fn(f)
	{
		std::cout << "typeid(F).name() : " << typeid(FnType).name() << std::endl;
	}

	int run(int x)
	{
		return fn(x);
	}

	FnType fn;
};


int simple_fun(int i)
{
	cout << __FUNCTION__ <<  " i : " << i << endl;
	return i;
}


//////////////////////////////////////////////////////////////////////////


double just_simple(int x, double y)
{
	cout << __FUNCTION__ << ": " << "x + y = " << (x + y) << endl;
	return (x + y);
}

void void_func(void)
{
	cout << __FUNCTION__ << endl;
}

template<typename Callback, typename... Args>
auto in_template_call(Callback&& f, Args&&... args)
{
	// < std::invoke_result和std::result_of之间的区别在于前者接受【可调用类型】和【参数类型】
	//typedef typename ::result_of<std::decay_t<Callback>(std::decay_t<Args>...)>::type ReturnType;
	typedef typename std::invoke_result_t<std::decay_t<Callback>, std::decay_t<Args>...> ReturnType;

	cout << "typeid(RT).name() = " << typeid(ReturnType).name() << endl;

	if (typeid(ReturnType) == typeid(void))
	{
		cout << "get a void type" << endl;
		f(args...);
	}
	else
	{
		return f(args...);
	}
}


//////////////////////////////////////////////////////////////////////////


template<typename Type>
void print_type()
{
	using T = std::invoke_result<std::decay_t<Type>>;

	//auto ft = decltype(std::invoke_result<FunctionType>);

	//cout << "typeid(T).name() : " << typeid(T).name() << endl;

	cout << "typeid(Type).name() : " << typeid(T).name() << endl;

	cout << endl;
}


float (*cast_func)(std::string, int);

float free_function(const std::string& a, int b)
{
	return (float)(a.size() / b);
}

struct AA
{
	int f(int a, int b) volatile
	{
		return (a + b);
	}

	int operator()(int) const
	{
		return 0;
	}
};


int function11(int a)
{
	cout << "function(int a) : " << a << endl;
	return a;
}


int func1(const int& a)
{
	cout << "func1(const int& a) : " << a << endl;
	return a;
}

//////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[])
{
	cout << std::boolalpha << endl;

	if (false)
	{
		cout << "type name ... " << endl << endl;

		std::tuple<int, double, std::string> tp = std::make_tuple(1, 4.2, std::string("how are you?"));

		using T = typename std::tuple<int, double, std::string>;
		cout << "tuple type name : " << type_name<T>();
		std::vector<int> vi{};
		using VT = typename std::vector<std::string>;
		//cout << "vector type name : " << type_name<decltype(vi)>();
		cout << "vector type name : " << type_name<VT>();


		//cout << "function type: " << type_name<function_traits<decltype(free_function)>::function_type>() << endl << endl;
		//cout << "function argument 1st : " << type_name<function_traits<decltype(free_function)>::template args<0>::type>() << endl << endl;
		//cout << "function argument 2nd : " << type_name<function_traits<decltype(free_function)>::template args<1>::type>() << endl;

		cout << endl;
	}

	if (false)
	{
		cout << "function_traits... " << endl << endl;

		std::function<int(int)> f = [](int a) {return a; };

		//print_type<function_traits<std::function<int(int)>>::function_type>();			//< function type
		//print_type<function_traits<std::function<int(int)>>::template args<0>::type>();	//< function parameter type

		//print_type<function_traits<decltype(f)>::function_type>();						//< function type
		 
		print_type<function_traits<decltype(free_function)>::function_type>();				//< function type
		print_type<function_traits<decltype(free_function)>::template args<0>::type>();		//< function parameter type
		print_type<function_traits<decltype(free_function)>::template args<1>::type>();		//< function parameter type

		print_type<function_traits<decltype(free_function)>::template arg_type<0>>();		//< function parameter type
		print_type<function_traits<decltype(free_function)>::template arg_type<1>>();		//< function parameter type
		
																				
		//print_type<function_traits<decltype(cast_func)>::function_type>();		//< function type

		//print_type<function_traits<AA>::function_type>();

		//< 类成员函数
		//using type = decltype(&AA::f);
		//print_type<type>();

		//print_type<function_traits<decltype(&AA::f)>::function_type>();

		static_assert(std::is_same_v<function_traits<decltype(f)>::return_type, int>, "");


		//cout << endl;

		//using FunctionType = typename std::function<int(int)>;
		//auto func = to_function(std::forward<FunctionType>(function11));
		//print_type<function_traits<decltype(func)>::function_type>();

		//auto func_pointer = to_function_pointer(std::move(func1));
		//print_type<function_traits<decltype(func_pointer)>::function_type>();
		//print_type<function_traits<decltype(func_pointer)>::pointer>();

		cout << endl;

	}


	if (false)
	{
		auto rt = in_template_call(just_simple, 1, 4.2);
		cout << "rt = " << rt << endl << endl;

		in_template_call(void_func);

		cout << endl;
	}


	// < std::invoke_result和std::result_of之间的区别在于前者接受【可调用类型】和【参数类型】,
	//< 而后者接受...

	if (false)
	{
		//< invoke_result : 获取的是可调用对象(表达式)的返回类型(而不是可调用对象本身)!!!

		//< 需要调用 decltype(fn)将函数转换为可调用对象!
		using TA = std::invoke_result<decltype(fn), int>::type;
		cout << "typeid(TA).name() : " << typeid(TA).name() << endl;	//< int

		//< 函数引用和函数指针及函数对象均为可调用对象(因此不用转换)
		using TB = std::invoke_result<fn_ref, int>::type;
		cout << "typeid(TB).name() : " << typeid(TB).name() << endl;	//< int

		using TC = std::invoke_result<fn_ptr, int>::type;
		cout << "typeid(TC).name() : " << typeid(TC).name() << endl;	//< int

		using TD = std::invoke_result<fn_class, int>::type;
		cout << "typeid(TD).name() : " << typeid(TD).name() << endl;	//< int

		cout << endl;
	}

	if (false)
	{
		//< result_of : 获取的是可调用对象(表达式)的返回类型(而不是可调用对象本身)!!!

		//< 需要调用 decltype(fn)将函数转换为可调用对象!
		using TA = ::result_of<decltype(fn)& (int)>::type;
		cout << "typeid(TA).name() : " << typeid(TA).name() << endl;	//< int

		//< 函数引用和函数指针及函数对象均为可调用对象(因此不用转换)
		using TB = ::result_of<fn_ref(int)>::type;
		cout << "typeid(TB).name() : " << typeid(TB).name() << endl;	//< int

		using TC = ::result_of<fn_ptr(int)>::type;
		cout << "typeid(TC).name() : " << typeid(TC).name() << endl;	//< int

		using TD = ::result_of<fn_class(int)>::type;
		cout << "typeid(TD).name() : " << typeid(TD).name() << endl;	//< int

		cout << endl;
	}



	if (false)
	{
		using T = At<0, int, double, char>::type;
		cout << "typeid(T).name() = " << typeid(T).name() << endl;

		cout << "At<0, int, double, char>::type = " << typeid(At<0, int, double, char>::type).name() << endl;
		cout << "At<1, int, double, char>::type = " << typeid(At<1, int, double, char>::type).name() << endl;
		cout << "At<2, int, double, char>::type = " << typeid(At<2, int, double, char>::type).name() << endl;

		cout << endl;
		::for_each<int, double, float, std::string>();

		cout << endl;
	}

	if (false)
	{
		auto ret = IntegerMax<1, 6, 2, 15>::value;
		cout << "ret = " << ret << endl;

		std::tuple<int, double, char, std::string> tp = std::make_tuple(1, 4.2, 'a', std::string("Hello"));
		//auto r = get_elem_by_type<float>(tp);	//< 编译器断言错误
		auto r = get_elem_by_type<std::string>(tp);
		cout << "type value : " << r << endl;


		//auto r = IndexOf<int, double, short, char, float>::value;
		//cout << "r = " << r << endl;

		//cout << "IndexOf<short, char, float>::value = " << IndexOf<short, char, float>::value << endl;
		//cout << "IndexOf<short, double, int, char, float>::value = " << IndexOf<short, int, double, char, float>::value << endl;

		cout << endl;
	}

	if (false)
	{
		SimpleFunction<int(int)> f = simple_fun;

		auto ret = f.run(1);
		cout << "ret = " << ret << endl;

		cout << endl;
	}


	//cout << meta_func<1, 2>::value << endl;

	//< 元函数只处理元数据, 元数据是编译期常量和类型, 所以下面的代码是编译不过的: 元函数无法处理运行时的普通数据
	//int i = 1, j = 2;
	//cout << meta_func<i, j>::value << endl;
	//cout << one_type::value << endl;

	//cout << std::true_type::value << endl;
	//cout << std::false_type::value << endl;


#ifdef _WIN32
	::system("pause");
#endif
	return EXIT_SUCCESS;
}

#endif

