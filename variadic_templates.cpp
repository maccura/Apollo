#include <iostream>
using std::cout;
using std::endl;

#include <type_traits>
#include <tuple>


#if 1

//////////////////////////////////////////////////////////////////////////

template<typename... T>
void f(T... args)
{
	cout << "parameter count : " << sizeof...(args) << endl;	//< 打印可变参数个数
}


//////////////////////////////////////////////////////////////////////////

//< 递归方式展开参数包

//< 递归终止函数
void print()
{
	cout << "empty" << endl;
}

template<typename T>
void print(T t)
{
	cout << "paramter : " << t << endl;
}

//< 展开函数参数
template<typename T, typename... Args>
void print(T head, Args... rest)
{
	cout << "paramter : " << head << endl;
	print(rest...);
}

template<typename T>
T sum(T t)
{
	return t;
}

template<typename T, typename... Types>
T sum(T first, Types... rest)
{
	return (first + sum<T>(rest...));
}

//////////////////////////////////////////////////////////////////////////

//< 逗号表达式展开参数包(不用递归终止函数)

template<typename T>
T print_arg(T t)
{
	cout << "paramter : " << t << endl;
	cout << "typeid(T).name() : " << typeid(t).name() << endl;
	return t;
}

template<typename... Args>
void expand(Args... args)
{
	auto arr = {(print_arg(args), 0)...};
	//cout << arr[0] << endl;
}

template<typename F, typename... Args>
void expand1(const F& f, Args&&... args)
{
	std::initializer_list<int>{ (f(std::forward<Args>(args)), 0)... };
}


//////////////////////////////////////////////////////////////////////////


//< 模板偏特化和递归方式展开可变模板参数类的参数包
//< 一般需要定义三个类, 包括类声明和偏特化的模板类

//< sum: 编译器计算出参数包中参数类型的size之和

//< forward declaration: 声明Sum是一个可变模板参数类
template<typename... Args>
struct Sum;

//< 支持0个参数
template<>
struct Sum<>
{
	enum {value = 0};
};

//< 基本定义
template<typename First, typename... Rest>
struct Sum<First, Rest...>
{
	enum { value = Sum<First>::value + Sum<Rest...>::value };
};

//< 递归终止
template<typename Last>
struct Sum<Last>
{
	enum {value = sizeof(Last)};
};

//////////////////////////////////////////////////////////////////////////

//< 继承方式展开(可变模板参数类)的参数包

//< 整型序列定义
template<int...>
struct IndexSeq
{
};

//< 继承方式展开参数包: 生成一个可变模板参数类的整型序列
template<int N, int... Indexes>
struct MakeIndexes : MakeIndexes<N - 1, N - 1, Indexes...>
{
};

//< 模板特化, 终止展开参数包的条件
template<int... Indexes>
struct MakeIndexes<0, Indexes...>
{
	using type = IndexSeq<Indexes...>;
	//typedef IndexSeq<Indexes...> type;
};


//< 不通过继承方式生成整型序列
template<int N, int... Indexes>
struct MakeIndexes3
{
	using type = typename MakeIndexes3<N - 1, N - 1, Indexes...>::type;
};

template<int... Indexes>
struct MakeIndexes3<0, Indexes...>
{
	using type = IndexSeq<Indexes...>;
};


template<typename Tuple, int...Indexes>
void transform(const IndexSeq<Indexes...>& in, const Tuple& tp)
{
	print(std::get<Indexes>(tp)...);
}


//////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[])
{
	if (true)
	{
		using Tuple = std::tuple<int, double>;
		Tuple tp = std::make_tuple(1, 2);
		transform(MakeIndexes<std::tuple_size<Tuple>::value>::type(), tp);
	}

	if (true)
	{
		cout << endl;

		using T = MakeIndexes<6>::type;
		cout << "typeid(T).name() = " << typeid(T).name() << endl;


		using T3 = MakeIndexes3<5>::type;
		cout << "typeid(T3).name() = " << typeid(T3).name() << endl;

		cout << endl;
	}


	if (true)
	{
		cout << "sum<int, double, short>::value = " << Sum<int, double, short int>::value << endl;

		cout << endl;
	}

	//if (false)
	//{
	//	print(1, 2, 4.2, "hello");

	//	cout << "sum(1, 2, 3, 4) = " << sum(1, 2, 3, 4) << endl;

	//	expand(1, 2, 3, 4);

	//	cout << endl;

	//	expand1([](int i) 
	//		{
	//			cout << "i = " << i << endl;
	//		}, 1, 2, 3, 4);

	//	cout << endl;

	//	expand1([](auto i) 
	//		{
	//			cout << "i = " << i << endl;
	//		}, 1, 2.0, "hello");

	//	cout << endl;
	//}

	//if (false)
	//{
	//	f();
	//	f(1);
	//	f(1, 2);
	//	f(3, 4.2, "");

	//	cout << endl;
	//}



#ifdef _WIN32
	::system("pause");
#endif
	return EXIT_SUCCESS;
}

#endif


