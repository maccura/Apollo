#ifndef __INDEX_TRAITS_H__
#define __INDEX_TRAITS_H__

#include <type_traits>
#include <tuple>

//< 生成可变模板参数类的整型序列


//< tuple参数的索引序列
template <int...>
struct IndexSeq
{
};


//< 通过继承方式展开可变参数模板类的参数包
template<int N, int... Indexes>
struct MakeIndexes : MakeIndexes <N - 1, N - 1, Indexes...>
{
};

//< 模板特化, 终止展开参数包的条件
template<int... Indexes>
struct MakeIndexes<0, Indexes...>
{
	//typedef IndexSeq<Indexes...> type;
	using type = IndexSeq<Indexes...>;
};


//////////////////////////////////////////////////////////////////////////


//< forward declaration
template <int I, typename Index, typename... Types>
struct make_indexes_impl;

//< declaration
template <int I, int... Indexes, typename T, typename... Types>
struct make_indexes_impl<I, IndexSeq<Indexes...>, T, Types...>
{
	using type = typename make_indexes_impl<I + 1, IndexSeq<Indexes..., I>, Types...>::type;
};


//< terminate
template<int I, int... Indexes>
struct make_indexes_impl<I, IndexSeq<Indexes...>>
{
	using type = IndexSeq<Indexes...>;
};

//< type trait
template<typename ... Types>
struct make_indexes : make_indexes_impl<0, IndexSeq<>, Types...>
{
};


//////////////////////////////////////////////////////////////////////////
//< for indexes reverse

template<int I, typename Index, typename... Types>
struct make_indexes_reverse_impl;

//< declaration
template<int I, int... Indexes, typename T, typename... Types>
struct make_indexes_reverse_impl<I, IndexSeq<Indexes...>, T, Types...>
{
	using type = typename make_indexes_reverse_impl <I - 1, IndexSeq <Indexes..., I - 1>, Types... >::type;
};

//< terminate
template<int I, int... Indexes>
struct make_indexes_reverse_impl<I, IndexSeq<Indexes...>>
{
	using type = IndexSeq<Indexes...>;
};

//< type trait
template<typename ... Types>
struct make_reverse_indexes : make_indexes_reverse_impl<sizeof...(Types), IndexSeq<>, Types...>
{
};


// ---- another method to generate sequence/indexes from tuple arguments.

template<int N, int... Indexes>
struct make_indexes2 : make_indexes2 < N - 1, N - 1, Indexes... > {};

//< template specialization - termination condition
template<int... Indexes>
struct make_indexes2<0, Indexes...>
{
	typedef IndexSeq<Indexes...> type;
};


//////////////////////////////////////////////////////////////////////////

template< int end, int cur, int... Indexes >
struct make_indexes3 : make_indexes3 < end, cur + 1, Indexes..., cur > {};

//< cur == end; the list has been built.
template <int end, int... Indexes>
struct make_indexes3<end, end, Indexes...>
{
	typedef IndexSeq<Indexes...> type;
};


#if 0
//////////////////////////////////////////////////////////////////////////
//< 根据元素值获取索引位置

//< 对于可转换的类型值则直接比较
template <typename T, typename U>
typename std::enable_if_t<std::is_convertible_v<T, U> || std::is_convertible_v<U, T>, bool> compare(T t, U u)
{
	return t == u;
}

//< 不能互相转换的则直接返回false
bool compare(...)
{
	return false;
}


//< 根据值查找索引
template<int I, typename T, typename... Args>
struct find_index
{
	static int call(std::tuple<Args...> const& t, T&& val)
	{
		return (compare(std::get < I - 1 >(t), val) ? I - 1 : find_index < I - 1, T, Args... >::call(t, std::forward<T>(val)));
	}
};


template<typename T, typename... Args>
struct find_index<0, T, Args...>
{
	static int call(std::tuple<Args...> const& t, T&& val)
	{
		return compare(std::get<0>(t), val) ? 0 : -1;
	}
};
#endif

//< 根据值获取tuple index
namespace detail
{
	template<size_t N, typename Tuple, typename T>
	static std::enable_if_t<std::is_convertible<std::tuple_element_t<N, Tuple>, T>::value, bool> equal_val(const Tuple& tp, const T& t)
	{
		return std::get<N>(tp) == t;
	}

	template<size_t N, typename Tuple, typename T>
	static std::enable_if_t<!std::is_convertible<std::tuple_element_t<N, Tuple>, T>::value, bool> equal_val(const Tuple& tp, const T& t)
	{
		return false;
	}

	template<int I, typename T, typename... Args>
	struct find_index
	{
		static int call(std::tuple<Args...> const& t, T&& val)
		{
			using U = std::remove_reference_t < std::remove_cv_t<T>>;
			using V = std::tuple_element_t<I - 1, std::tuple<Args...>>;
			return (std::is_convertible<U, V>::value && equal_val<I - 1>(t, val)) ? I - 1 : find_index<I - 1, T, Args...>::call(t, std::forward<T>(val));
		}
	};

	template<typename T, typename... Args>
	struct find_index<0, T, Args...>
	{
		static int call(std::tuple<Args...> const& t, T&& val)
		{
			using U = std::remove_reference_t < std::remove_cv_t<T>>;
			using V = std::tuple_element_t<0, std::tuple<Args...>>;
			return (std::is_convertible<U, V>::value && equal_val<0>(t, val)) ? 0 : -1;
		}
	};
}


template<typename T, typename... Args>
int find_index(std::tuple<Args...> const& t, T&& val)
{
	return detail::find_index<sizeof...(Args), T, Args...>::call(t, std::forward<T>(val));
}



//////////////////////////////////////////////////////////////////////////

//< 编译期从一个整型序列中获取最大值

//< 获取最大的整数
template<size_t arg, size_t... rest>
struct IntegerMax;

template<size_t arg>
struct IntegerMax<arg> : std::integral_constant<size_t, arg>
{
};

template<size_t arg1, size_t arg2, size_t... rest>
struct IntegerMax<arg1, arg2, rest...> : std::integral_constant<size_t, arg1 >= arg2 ? IntegerMax<arg1, rest...>::value : IntegerMax<arg2, rest...>::value>
{
};



//////////////////////////////////////////////////////////////////////////

//< 根据元素类型获取tuple中的元素

//< C++14 

template<typename T, std::size_t N, typename... Args>
struct IndexOf;

template<typename T, std::size_t N, typename... Args>
struct IndexOf<T, N, T, Args...>
{
	static constexpr auto value = N;
};

template<typename T, std::size_t N, typename U, typename... Args>
struct IndexOf<T, N, U, Args...>
{
	static constexpr auto value = IndexOf<T, N + 1, Args...>::value;
};

template<typename T, std::size_t N>
struct IndexOf<T, N>
{
	static constexpr auto value = -1;
	static_assert((value == -1), "the type is not exists!");
};

template<typename T, typename... Args>
T get_elem_by_type(const std::tuple<Args...>& tp)
{
	return std::get<IndexOf<T, 0, Args...>::value>(tp);
}



//////////////////////////////////////////////////////////////////////////



#if 0

//< 编译期获取类型的索引
template<typename T, typename... List>
struct IndexOf;

template<typename T, typename Head, typename... Rest>
struct IndexOf<T, Head, Rest...>
{
	enum { value = IndexOf<T, Rest...>::value + 1 };
};

template<typename T, typename... Rest>
struct IndexOf<T, T, Rest...>
{
	enum { value = 0 };
};

template<typename T>
struct IndexOf<T>
{
	enum { value = -1 };
};

#endif

//////////////////////////////////////////////////////////////////////////

//< 编译期根据索引位置查找类型
template<int index, typename... Types>
struct At;

template<int index, typename First, typename... Types>
struct At<index, First, Types...>
{
	using type = typename At<index - 1, Types...>::type;
};

template<typename T, typename... Types>
struct At<0, T, Types...>
{
	using type = T;
};


//<编译期遍历类型
template<typename T>
void print_arg()
{
	cout << "type : " << typeid(T).name() << endl;
}

template<typename... Args>
void for_each()
{
	std::initializer_list<int>{(print_arg<Args>(), 0)...};
}

#endif	//< __INDEX_TRAITS_H__

