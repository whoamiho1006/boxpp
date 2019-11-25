#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/traits/BypassType.hpp>

namespace boxpp
{
	template<typename ... Types>
	struct TTuple 
	{
	};

	template<typename Type, typename ... Types>
	struct TTupleStorage
	{
		TTupleStorage(BypassType<Type> Value) 
			: Value(Bypass<Type>(Value))
		{
		}

		Type Value;
	};

	namespace _ {
		template<typename ... Types>
		struct TTupleCounter
		{
			static constexpr s32 N = 0;
		};

		template<typename Type, typename ... Types>
		struct TTupleCounter<Type, Types ...>
		{
			static constexpr s32 N = TTupleCounter<Types ...>::N + 1;
		};

		template<s32 N, typename ...>
		struct TTupleAccessor
		{
		};

		template<s32 N, typename Type, typename ... Types>
		struct TTupleAccessor<N, TTuple<Type, Types ...>>
			: TTupleAccessor<TTuple<>, TTuple<Type, Types ...>>
		{
		};

		template<s32 N, typename Type, typename ... PreTypes, typename ... PostTypes>
		struct TTupleAccessor<N, TTuple<PreTypes ...>, TTuple<Type, PostTypes ...>>
			: TTupleAccessor<N - 1, TTuple<PreTypes ..., Type>, TTuple<PostTypes ...>>
		{
		};

		template<typename _Type, typename ... PreTypes, typename ... PostTypes>
		struct TTupleAccessor<0, TTuple<PreTypes ...>, TTuple<_Type, PostTypes ...>>
		{
			using Type = _Type;

			FASTINLINE static Type& Get(const TTuple<PreTypes ..., _Type, PostTypes ...>& Tuple) {
				return (static_cast<const TTupleStorage<_Type, PostTypes ...>*>(&Tuple))->Value;
			}

			FASTINLINE static void Set(TTuple<PreTypes ..., _Type, PostTypes ...>& Tuple, BypassType<Type> Value) {
				(static_cast<TTupleStorage<_Type, PostTypes ...>*>(&Tuple))->Value = Value;
			}
		};
	}

	template<typename Type, typename ... Types>
	struct TTuple<Type, Types ...> 
		: TTupleStorage<Type, Types...>, TTuple<Types ...>
	{
		TTuple(BypassType<Type> Value, BypassType<Types> ... Args)
			: TTupleStorage(Bypass<Type>(Value)), TTuple<Types ...>(Args ...)
		{
		}
	};

	template<typename ... Types>
	FASTINLINE TTuple<Types...> MakeTuple(BypassType<Types> ... Args)
	{
		return TTuple<Types...> MakeTuple(Bypass<Types>(Args) ...);
	}

	template<s32 N, typename TupleType>
	FASTINLINE BypassType<typename _::TTupleAccessor<N, TupleType>::Type> GetTuple(const TupleType& Tuple) {
		return _::TTupleAccessor<N, TupleType>::Get(Tuple);
	}

	template<s32 N, typename TupleType>
	FASTINLINE void SetTuple(TupleType& Tuple, BypassType<typename _::TTupleAccessor<N, TupleType>::Type> Value) {
		_::TTupleAccessor<N, TupleType>::Set(Tuple, Value);
	}
}