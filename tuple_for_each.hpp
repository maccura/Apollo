#ifndef __TUPLE_FOR_EACH_H__
#define __TUPLE_FOR_EACH_H__

#include <utility>
#include <tuple>
#include <type_traits>

#include "index_traits.hpp"


namespace details 
{

	template<typename Func, typename Last>
	void for_each_impl(Func&& f, Last&& last)
	{
		f(std::forward<Last>(last));
	}

	template<typename Func, typename First, typename ... Rest>
	void for_each_impl(Func&& f, First&& first, Rest&&...rest)
	{
		f(std::forward<First>(first));
		for_each_impl(std::forward<Func>(f), rest...);
	}

	template<typename Func, int ... Indexes, typename ... Args>
	void for_each_helper(Func&& f, IndexSeq<Indexes...>, std::tuple<Args...>&& tup)
	{
		for_each_impl(std::forward<Func>(f), std::forward<Args>(std::get<Indexes>(tup))...);
	}

} // namespace details 


template<typename Func, typename ... Args>
void tuple_for_each(Func&& f, std::tuple<Args...>& tup)
{
	using namespace details;
	for_each_helper(std::forward<Func>(f), typename make_indexes<Args...>::type(), std::tuple<Args...>(tup));
}

template<typename Func, typename ... Args>
void tuple_for_each(Func&& f, std::tuple<Args...>&& tup)
{
	using namespace details;
	for_each_helper(std::forward<Func>(f), typename make_indexes<Args...>::type(), std::forward<std::tuple<Args...>>(tup));
}

#endif	//< __TUPLE_FOR_EACH_H__


