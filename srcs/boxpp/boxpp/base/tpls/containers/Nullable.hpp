#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/traits/EnableIf.hpp>
#include <boxpp/base/tpls/traits/IsConstructibleType.hpp>
#include <boxpp/base/tpls/containers/Instrusive.hpp>

namespace boxpp
{

	template<typename Type>
	class TNullable
	{
	public:
		TNullable(nullptr_t = nullptr) { }

		template<typename = EnableIf<IsCopyConstructibleType<Type>>>
		TNullable(const Type& Value) {
			Storage.ConstructFrom(Value);
		}

		template<typename = EnableIf<IsMoveConstructibleType<Type>>>
		TNullable(Type&& Value) {
			Storage.ConstructFrom(Forward<Type>(Value));
		}

		template<typename = EnableIf<IsCopyConstructibleType<Type>>>
		TNullable(const TNullable<Type>& Other) {
			if (Other.Storage) {
				Storage.ConstructFrom(*Other.Storage.GetRaw());
			}
		}

		template<typename = EnableIf<IsMoveConstructibleType<Type>>>
		TNullable(TNullable<Type>&& Other) {
			if (Other.Storage) {
				Storage.ConstructFrom(Forward<Type>(*Other.Storage.GetRaw()));
			}
		}

	private:
		TInstrusive<Type> Storage;

	public:
		FASTINLINE operator bool() const { return Storage; }
		FASTINLINE bool operator !() const { return !Storage; }

		FASTINLINE TNullable& operator =(nullptr_t) {
			Storage.Destruct();
			return *this;
		}

		template<typename = EnableIf<IsCopyConstructibleType<Type>>>
		FASTINLINE TNullable& operator =(const TNullable<Type>& Other) {
			if (this != &Other) {
				Storage.Destruct();
				if (Other.Storage) {
					Storage.ConstructFrom(*Other.Storage.GetRaw());
				}
			}

			return *this;
		}

		template<typename = EnableIf<IsMoveConstructibleType<Type>>>
		FASTINLINE TNullable& operator =(TNullable<Type>&& Other) {
			if (this != &Other) {
				Storage.Destruct();
				if (Other.Storage) {
					Storage.ConstructFrom(Forward<Type>(*Other.Storage.GetRaw()));
				}
			}

			return *this;
		}
	};

}