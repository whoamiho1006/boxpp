#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/traits/EnableIf.hpp>
#include <boxpp/base/tpls/traits/Forward.hpp>
#include <boxpp/base/tpls/traits/Movable.hpp>

#include <boxpp/base/tpls/traits/IsCallableType.hpp>
#include <boxpp/base/tpls/traits/IsConstructibleType.hpp>
#include <boxpp/base/tpls/traits/AbstractedOperators.hpp>

namespace boxpp
{
	/*	Instrusive object storage. */
	template<typename Type>
	class TInstrusive
	{
	public:
		FASTINLINE TInstrusive() : Valid(false) { }
		FASTINLINE ~TInstrusive() { Destruct(); }

	private:
		bool Valid;
		u8 Storage[sizeof(Type)];

	public:
		FASTINLINE operator bool() const { return Valid; }
		FASTINLINE bool operator !() const { return !Valid; }

		FASTINLINE Type* GetRaw() const { return Valid ? (Type*)Storage : nullptr; }

		FASTINLINE Type* operator ->() const { return GetRaw(); }
		FASTINLINE Type& operator *() const { return *GetRaw(); }

	public:
		FASTINLINE bool ConstructFrom(const Type& Object)
		{
			if (!Valid) {
				Valid = (new (Storage) Type(Object)) != nullptr;
				return true;
			}

			return false;
		}

		FASTINLINE bool ConstructFrom(Type&& Object)
		{
			if (!Valid) {
				Valid = (new (Storage) Type(TMovable<Type>::Movable(Object))) != nullptr;
				return true;
			}

			return false;
		}

		template<typename ... ArgumentTypes>
		FASTINLINE bool Construct(ArgumentTypes&& ... Arguments)
		{
			if (!Valid) {
				Valid = (new (Storage) Type(Forward<ArgumentTypes>(Arguments) ...)) != nullptr;
				return true;
			}

			return false;
		}

		FASTINLINE bool Destruct()
		{
			if (Valid) {
				Valid = false;
				(*((Type*)Storage)).~Type();
				return true;
			}

			return false;
		}
	};
}