#ifndef __TUPLE_INDEXES_H__
#define __TUPLE_INDEXES_H__


#include <tuple>


//< tuple参数的索引序列
template<int...>
struct index_tuple 
{
};


//< forward declare
template<int I, typename IndexTuple, typename... Types>
struct make_indexes_impl;

//< declaration
template<int I, int... Indexes, typename T, typename... Types>
struct make_indexes_impl<I, index_tuple<Indexes...>, T, Types...>
{
	using type = typename make_indexes_impl<I + 1, index_tuple<Indexes..., I>, Types...>::type;
};

//< terminate
template<int I, int... Indexes>
struct make_indexes_impl<I, index_tuple<Indexes...>>
{
	using type = index_tuple<Indexes...>;
};

//< type trait
template<typename ... Types>
struct make_indexes : make_indexes_impl<0, index_tuple<>, Types...>
{};



//////////////////////////////////////////////////////////////////////////
//< tuple indexes reverse


template<int I, typename IndexTuple, typename... Types>
struct make_indexes_reverse_impl;

//< declaration
template<int I, int... Indexes, typename T, typename... Types>
struct make_indexes_reverse_impl<I, index_tuple<Indexes...>, T, Types...>
{
	using type = typename make_indexes_reverse_impl<I - 1, index_tuple<Indexes..., I - 1>, Types...>::type;
};

//< terminate
template<int I, int... Indexes>
struct make_indexes_reverse_impl<I, index_tuple<Indexes...>>
{
	using type = index_tuple<Indexes...>;
};

//< type trait
template<typename ... Types>
struct make_reverse_indexes : make_indexes_reverse_impl<sizeof...(Types), index_tuple<>, Types...>
{
};


//////////////////////////////////////////////////////////////////////////

// ---- another method to generate sequence/indexes from tuple arguments.

template<int N, int... Indexes>
struct make_indexes2 : make_indexes2<N - 1, N - 1, Indexes...> 
{
};

//< template specialization - termination condition
template<int... Indexes>
struct make_indexes2<0, Indexes...>
{
	typedef index_tuple<Indexes...> type;
};


//////////////////////////////////////////////////////////////////////////

template< int end, int cur, int... Indexes >
struct make_indexes3 : make_indexes3< end, cur + 1, Indexes..., cur > 
{
};

//< cur == end; the list has been built.
template< int end, int... Indexes >
struct make_indexes3< end, end, Indexes... >
{
	typedef index_tuple<Indexes...> type;
};


#endif	//< __TUPLE_INDEXES_H__


