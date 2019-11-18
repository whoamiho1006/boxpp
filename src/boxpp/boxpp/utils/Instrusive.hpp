#ifndef __BOXPP_UTILS_INSTRUSIVE_HPP__
#define __BOXPP_UTILS_INSTRUSIVE_HPP__

/* Loads boxpp.hpp header if not loaded. */
#ifndef __BOXPP_HPP__
#include <boxpp.hpp>
#endif

#include <boxpp/utils/Movable.hpp>
#include <boxpp/utils/Forward.hpp>
#include <boxpp/utils/Functionality.hpp>
#include <boxpp/sharedptr/SharedPtr.hpp>

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
		u8 Storage [sizeof(Type)];

	public:
		FASTINLINE operator bool() const { return Valid; }
		FASTINLINE bool operator !() const { return !Valid; }

		FASTINLINE Type* GetRaw() const { return Valid ? (Type*)Storage : nullptr; }

		FASTINLINE Type* operator ->() const { return GetRaw(); }
		FASTINLINE Type& operator *() const { return *GetRaw(); }

	public:
		FASTINLINE const TSharedPtr<Type> MakeShared() const {
			sharedptr::TSmartProxy<Type> Proxy = { 0, };

			if (*this) {

				Proxy.Counter = new sharedptr::TSharedCount<Type,
					sharedptr::TEmptyDeleter<Type>>(GetRaw());

				Proxy.Object = GetRaw();
			}

			return Proxy;
		}

	public:
		template<typename = EnableIf<IsCopyConstructible<Type>>>
		FASTINLINE bool ConstructFrom(const Type& Object)
		{
			if (!Valid) {
				Valid = (new (Storage) Type(Object)) != nullptr;
				return true;
			}

			return false;
		}

		template<typename = EnableIf<IsMoveConstructible<Type>>>
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

#endif // !__BOXPP_UTILS_INSTRUSIVE_HPP__