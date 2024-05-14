#ifndef __TUPLE_ZIP_H__
#define __TUPLE_ZIP_H__

#include <utility>
#include <tuple>

#include "index_traits.hpp"


//< 将两个tuple合起来, 前一个tuple中的每个元素为key, 后一个tuple中的每个元素为value, 组成一个pair集合
namespace details
{
	template<std::size_t N, typename T1, typename T2>
	using pair_type = std::pair<typename std::tuple_element<N, T1>::type, typename std::tuple_element<N, T2>::type>;

	template<std::size_t N, typename T1, typename T2>
	pair_type<N, T1, T2> pair(const T1& tup1, const T2& tup2)
	{
		return std::make_pair(std::get<N>(tup1), std::get<N>(tup2));
	}

	template<int... Indexes, typename T1, typename T2>
	auto pairs_helper(IndexSeq<Indexes...>, const T1& tup1, const T2& tup2) -> decltype(std::make_tuple(pair<Indexes>(tup1, tup2)...))
	{
		return std::make_tuple(pair<Indexes>(tup1, tup2)...);
	}

} // namespace details


template<typename... Args1, typename... Args2>
auto tuple_pairs(const std::tuple<Args1...>& tup1, const std::tuple<Args2...>& tup2) -> decltype(details::pairs_helper(typename make_indexes<Args1...>::type(), tup1, tup2))
{
	static_assert(sizeof... (Args1) == sizeof... (Args2), "tuples should be the same size.");
	return details::pairs_helper(typename make_indexes<Args1...>::type(), tup1, tup2);
}


#endif	//< __TUPLE_ZIP_H__


