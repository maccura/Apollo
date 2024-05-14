#ifndef __TUPLE_APPLY_H__
#define __TUPLE_APPLY_H__

#include <type_traits>
#include <utility>
#include <tuple>

#include "index_traits.hpp"
#include "result_of.hpp"


template<class F, class... Args, int... Indexes >
typename ::result_of<F(Args...)>::type apply_helper(F&& pf, IndexSeq<Indexes...>&& in, std::tuple<Args...>&& tup)
{
	return std::forward<F>(pf)(std::forward<Args>(std::get<Indexes>(tup))...);
}

template<class F, class ... Args>
typename ::result_of<F(Args...)>::type apply(F&& pf, const std::tuple<Args...>& tup)
{
	return apply_helper(std::forward<F>(pf), typename make_indexes<Args...>::type(), std::tuple<Args...>(tup));
}

template<class F, class ... Args>
typename ::result_of<F(Args...)>::type apply(F&& pf, std::tuple<Args...>&& tup)
{
	return apply_helper(std::forward<F>(pf), typename make_indexes<Args...>::type(), std::forward<std::tuple<Args...>>(tup));
}



///***************
//< recurse
//  <<1, <0>, Args...>type>
//  <<2, <1>, Args...>type>
//  <<3, <2>, Args...>type>
//  <<4, <3>, Args...>type>
//  terminate
//  <<5, <>>type> 
//  ==> evalue
//  ==> result index sequence: <3, 2, 1, 0>
///

template<class F, class ... Args>
typename ::result_of<F(Args...)>::type apply2(F&& pf, const std::tuple<Args...>& tup)
{
	return apply_helper(std::forward<F>(pf), typename make_indexes2<sizeof ...(Args)>::type(), std::tuple<Args...>(tup));
}

template<class F, class ... Args>
typename ::result_of<F(Args...)>::type apply2(F&& pf, std::tuple<Args...>&& tup)
{
	return apply_helper(std::forward<F>(pf), typename make_indexes2<sizeof ...(Args)>::type(), std::forward<std::tuple<Args...>>(tup));
}


template<class F, class ... Args>
typename ::result_of<F(Args...)>::type apply3(F&& pf, const std::tuple<Args...>& tup)
{
	return apply_helper(std::forward<F>(pf), typename make_indexes3<sizeof ...(Args), 0>::type(), std::tuple<Args...>(tup));
}

template<class F, class ... Args>
typename ::result_of<F(Args...)>::type apply3(F&& pf, std::tuple<Args...>&& tup)
{
	return apply_helper(std::forward<F>(pf), typename make_indexes3<sizeof ...(Args), 0>::type(), std::forward<std::tuple<Args...>>(tup));
}

////< 根据值获取tuple index
//namespace detail
//{
//	template<size_t N, typename Tuple, typename T>
//	static std::enable_if_t<std::is_convertible<std::tuple_element_t<N, Tuple>, T>::value, bool> equal_val(const Tuple& tp, const T& t)
//	{
//		return std::get<N>(tp) == t;
//	}
//
//	template<size_t N, typename Tuple, typename T>
//	static std::enable_if_t<!std::is_convertible<std::tuple_element_t<N, Tuple>, T>::value, bool> equal_val(const Tuple& tp, const T& t)
//	{
//		return false;
//	}
//
//	template<int I, typename T, typename... Args>
//	struct find_index
//	{
//		static int call(std::tuple<Args...> const& t, T&& val)
//		{
//			using U = std::remove_reference_t < std::remove_cv_t<T>>;
//			using V = std::tuple_element_t<I - 1, std::tuple<Args...>>;
//			return (std::is_convertible<U, V>::value && equal_val<I - 1>(t, val)) ? I - 1 : find_index<I - 1, T, Args...>::call(t, std::forward<T>(val));
//		}
//	};
//
//	template<typename T, typename... Args>
//	struct find_index<0, T, Args...>
//	{
//		static int call(std::tuple<Args...> const& t, T&& val)
//		{
//			using U = std::remove_reference_t < std::remove_cv_t<T>>;
//			using V = std::tuple_element_t<0, std::tuple<Args...>>;
//			return (std::is_convertible<U, V>::value && equal_val<0>(t, val)) ? 0 : -1;
//		}
//	};
//}
//
//
//template<typename T, typename... Args>
//int find_index(std::tuple<Args...> const& t, T&& val)
//{
//	return detail::find_index<sizeof...(Args), T, Args...>::call(t, std::forward<T>(val));
//}


#endif	//< __TUPLE_APPLY_H__


