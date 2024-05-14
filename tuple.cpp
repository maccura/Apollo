#include <iostream>
using std::cout;
using std::endl;


#if 1

#include "index_traits.hpp"


#include "tuple_print.hpp"
#include "tuple_zip.hpp"
#include "tuple_for_each.hpp"
#include "tuple_apply.hpp"
#include "tuple_reverse.hpp"


//< 可变模板参数不能直接作为变量保存, 而需要借助tuple保存起来, 保存之后再在需要时通过一些手段将tuple转换为可变模板参数

template<typename T>
void print(T t)
{
	cout << "typeid(T).name() = " << typeid(T).name() << endl;
	cout << "value : " << t << endl;
}

template<typename T, typename... Args>
void print(T t, Args... args)
{
	print(t);
	print(args...);
}


/*
 * 先将可变模板参数保存在tuple中, 然后再通过元函数make_indexes2生成一个整型序列， 这个整形序列就是index_tuple<0,1,2>
 * 整形序列代表了tuple中元素的索引, 生成整形序列之后再调用print_helper, 在print_helper中展开这个整型序列,
 * 展开的过程中根据具体的索引从tuple中获取对应的元素, 最终将从tuple中取出来的元素组成一个可变模板参数,
 * 从而实现了tuple"还原"为可变模板参数, 最终调用print打印可变模板参数.
 */

template<int... Indexes, typename... Args>
void print_helper(IndexSeq<Indexes...>, std::tuple<Args...>&& tup)
{
	//< 再将tuple转换为可变参数模板, 将参数还原回来, 再调用print
	print(std::get<Indexes>(tup)...);
}

template<typename... Args>
void print_args(Args... args)
{
	print_helper(typename make_indexes2<sizeof...(Args)>::type(), std::make_tuple(args...));
}

//////////////////////////////////////////////////////////////////////////

struct Functor
{
	template<typename T>
	void operator()(const T& t)
	{
		//t.do_something();
		std::cout << __FUNCTION__ << ", t : " << t << endl;
	}
};

//////////////////////////////////////////////////////////////////////////


void test_func(int a, double b)
{
	cout << "a + b = " << (a + b) << endl;
}

//////////////////////////////////////////////////////////////////////////



int main(int argc, char* argv[])
{
	if (false)
	{
		auto tp1 = std::make_tuple<int, double>(1, 4.2);
		auto tp2 = std::make_tuple<char, std::string>('a', std::string("Hello"));

		//< std::tuple合并, 返回一个tuple
		auto p = tuple_pairs(tp1, tp2);
		cout << "key = " << std::get<0>(p).first << endl;
		cout << "value = " << std::get<0>(p).second << endl;

		cout << endl;

		cout << "key = " << std::get<1>(p).first << endl;
		cout << "value = " << std::get<1>(p).second << endl;


		cout << endl;
	}

	if (false)
	{
		apply(test_func, std::make_tuple<int, int>(1, 2));
	}

	if (false)
	{
		auto tp1 = std::make_tuple<int, short, double, std::string>(1, 2, 4.2, std::string("Hello"));
		//auto tp2 = reverse(std::move(tp1));
		auto tp2 = tuple_reverse(std::make_tuple<int, short, double, std::string>(1, 2, 4.2, std::string("Hello")));
		
		PrintTuple(tp1);
		PrintTuple(tp2);

		cout << endl;
	}

	if (true)
	{
		tuple_for_each(Functor(), std::make_tuple<int, double>(1, 4.2));

		cout << endl;

		std::tuple<int, double, std::string> tp = std::make_tuple(1, 4.2, "Hello");
		int idx = find_index(tp, std::string{"Hello"});
		cout << "tuple index : " << idx << endl;

		cout << "find_index(tp, 1): " << find_index(tp, 1) << endl;
		cout << "find_index(tp, 4.2): " << find_index(tp, 4.2) << endl;

		cout << endl;
	}

	if (true)
	{
		print_args(1, 4.2, "Hello");

		cout << endl;
	}
	


#ifdef _WIN32
	::system("pause");
#endif
	return EXIT_SUCCESS;
}

#endif



