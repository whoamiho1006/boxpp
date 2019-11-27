#pragma once
#include <boxpp/base/tpls/BaseTraits.hpp>

namespace boxpp
{
	namespace ops {
#define BOX_FUNCTIONALITY_OPERATOR(Name, Operation) \
		template<typename Type = void> struct Name { \
			constexpr auto operator ()(const Type& L, const Type& R) const \
				-> decltype(L Operation R) { return L Operation R; } \
		}; \
		template<> struct Name<void> { template<typename Left, typename Right> \
			constexpr auto operator ()(const Left& L, const Right& R) const \
				-> decltype(L Operation R) { return L Operation R; } \
		}

#define BOX_FUNCTIONALITY_OPERATOR_ASSIGN(Name, Operation) \
		template<typename Type = void> struct Name { \
			constexpr auto operator ()(Type& L, const Type& R) const \
				-> decltype(L Operation R) { return L Operation R; } \
		}; \
		template<> struct Name<void> { template<typename Left, typename Right> \
			constexpr auto operator ()(Left& L, const Right& R) const \
				-> decltype(L Operation R) { return L Operation R; } \
		}

#define BOX_FUNCTIONALITY_PREOPERATOR(Name, Operation) \
		template<typename Type> struct Name { \
			constexpr auto operator()(const Type& V) const \
				-> decltype(Operation V) { return Operation V; } \
		}

#define BOX_FUNCTIONALITY_POSTOPERATOR(Name, Operation) \
		template<typename Type> struct Name { \
			constexpr auto operator()(const Type& V) const \
				-> decltype(V Operation) { return V Operation; } \
		}

		BOX_FUNCTIONALITY_OPERATOR(Equal,					==);
		BOX_FUNCTIONALITY_OPERATOR(NotEqual,				!=);
		BOX_FUNCTIONALITY_OPERATOR(Greater,					>);
		BOX_FUNCTIONALITY_OPERATOR(Less,					<);
		BOX_FUNCTIONALITY_OPERATOR(GreaterEqual,			>=);
		BOX_FUNCTIONALITY_OPERATOR(LessEqual,				<=);
		BOX_FUNCTIONALITY_PREOPERATOR(PreIncrement,			++);
		BOX_FUNCTIONALITY_PREOPERATOR(PreDecrement,			--);
		BOX_FUNCTIONALITY_POSTOPERATOR(PostIncrement,		++);
		BOX_FUNCTIONALITY_POSTOPERATOR(PostDecrement,		--);
		BOX_FUNCTIONALITY_PREOPERATOR(Not,					!);
		BOX_FUNCTIONALITY_OPERATOR(And,						&&);
		BOX_FUNCTIONALITY_OPERATOR(Or,						||);
		BOX_FUNCTIONALITY_PREOPERATOR(BitwiseNot,			!);
		BOX_FUNCTIONALITY_OPERATOR(BitwiseAnd,				&);
		BOX_FUNCTIONALITY_OPERATOR(BitwiseOr,				|);
		BOX_FUNCTIONALITY_OPERATOR(BitwiseXor,				&);
		BOX_FUNCTIONALITY_OPERATOR(Plus,					+);
		BOX_FUNCTIONALITY_OPERATOR(Minus,					-);
		BOX_FUNCTIONALITY_OPERATOR(Multiply,				*);
		BOX_FUNCTIONALITY_OPERATOR(Modular,					%);
		BOX_FUNCTIONALITY_OPERATOR(Divide,					/);
		BOX_FUNCTIONALITY_OPERATOR(Assign,					=);
		BOX_FUNCTIONALITY_OPERATOR_ASSIGN(PlusAssign,		+=);
		BOX_FUNCTIONALITY_OPERATOR_ASSIGN(MinusAssign,		-=);
		BOX_FUNCTIONALITY_OPERATOR_ASSIGN(MultiplyAssign,	*=);
		BOX_FUNCTIONALITY_OPERATOR_ASSIGN(ModularAssign,	%=);
		BOX_FUNCTIONALITY_OPERATOR_ASSIGN(DivideAssign,		/=);
		BOX_FUNCTIONALITY_OPERATOR_ASSIGN(LeftShiftAssign,	<<=);
		BOX_FUNCTIONALITY_OPERATOR_ASSIGN(RightShiftAssign,	>>=);
	}
	
#undef BOX_FUNCTIONALITY_OPERATOR
#undef BOX_FUNCTIONALITY_PREOPERATOR
#undef BOX_FUNCTIONALITY_POSTOPERATOR
}