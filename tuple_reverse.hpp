#ifndef __TUPLE_REVERSE_H__
#define __TUPLE_REVERSE_H__

#include <tuple>
#include <utility>
#include <type_traits>

#include "index_traits.hpp"


//////////////////////////////////////////////////////////////////////////
//< 翻转Tuple

template <class...Args, int...Indexes>
auto reverse_impl(std::tuple<Args...>&& tup, IndexSeq<Indexes...>&&) -> decltype(std::make_tuple(std::get<Indexes>(std::forward<std::tuple<Args...>>(tup))...))
{
	return std::make_tuple(std::get<Indexes>(std::forward<std::tuple<Args...>>(tup))...);
}

template <class...Args>
auto tuple_reverse(std::tuple<Args...>&& tup) -> decltype(reverse_impl(std::forward<std::tuple<Args...>>(tup), typename make_reverse_indexes<Args...>::type()))
{
	return reverse_impl(std::forward<std::tuple<Args...>>(tup), typename make_reverse_indexes<Args...>::type());
}


#endif	//< __TUPLE_REVERSE_H__



