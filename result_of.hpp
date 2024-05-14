#ifndef __RESULT_OF_H__
#define __RESULT_OF_H__

#include <iostream>
#include <type_traits>

//namespace maccura
//{

namespace cpp
{
	template <typename T>
	struct is_reference_wrapper : std::false_type
	{
	};

	template <typename U>
	struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type
	{
	};

	template<typename T>
	struct invoke_impl
	{
		template<typename F, typename... Args>
		static auto call(F&& f, Args&&... args) -> decltype(std::forward<F>(f)(std::forward<Args>(args)...));
	};

	template<typename B, typename MT>
	struct invoke_impl<MT B::*>
	{
		template<typename T, typename Td = typename std::decay<T>::type, class = typename std::enable_if<std::is_base_of<B, Td>::value>::type>
		static auto get(T&& t)->T&&;

		template<typename T, typename Td = typename std::decay<T>::type, class = typename std::enable_if<is_reference_wrapper<Td>::value>::type>
		static auto get(T&& t) -> decltype(t.get());

		template<typename T, typename Td = typename std::decay<T>::type, class = typename std::enable_if<!std::is_base_of<B, Td>::value>::type, class = typename std::enable_if<!is_reference_wrapper<Td>::value>::type>
		static auto get(T&& t) -> decltype(*std::forward<T>(t));

		template<typename T, typename... Args, typename MT1, class = typename std::enable_if<std::is_function<MT1>::value>::type>
		static auto call(MT1 B::* pmf, T&& t, Args&&... args) -> decltype((invoke_impl::get(std::forward<T>(t)).*pmf)(std::forward<Args>(args)...));

		template<typename T>
		static auto call(MT B::* pmd, T&& t) -> decltype(invoke_impl::get(std::forward<T>(t)).*pmd);
	};

	template<typename F, typename... Args, typename Fd = typename std::decay<F>::type>
	auto INVOKE(F&& f, Args&&... args) -> decltype(invoke_impl<Fd>::call(std::forward<F>(f), std::forward<Args>(args)...));
} // namespace cpp


// Minimal C++11 implementation:
template <typename>
struct result_of;

template <typename F, typename... ArgTypes>
struct result_of<F(ArgTypes...)>
{
	using type = decltype(cpp::INVOKE(std::declval<F>(), std::declval<ArgTypes>()...));
};


// Conforming C++14 implementation (is also a valid C++11 implementation):
namespace cpp
{
	template <typename AlwaysVoid, typename, typename...>
	struct invoke_result
	{
	};

	template <typename F, typename... Args>
	struct invoke_result<decltype(void(cpp::INVOKE(std::declval<F>(), std::declval<Args>()...))), F, Args...>
	{
		using type = decltype(cpp::INVOKE(std::declval<F>(), std::declval<Args>()...));
	};
} //< namespace cpp


//< Deduces the return type of an INVOKE expression at compile time
//< F must be a callable type, reference to function, or reference to callable type.
//< Invoking F with ArgTypes... must be a well-formed expression
//< F and all types in ArgTypes can be any complete type, array of unknown bound, or (possibly cv-qualified) void


template <typename F, typename... ArgTypes>
struct invoke_result : cpp::invoke_result<void, F, ArgTypes...>
{
};


//}


#endif	//< __RESULT_OF_H__


