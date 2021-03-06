#pragma once
#include <boxpp/base/BaseMacros.hpp>
#include <boxpp/base/BaseTypes.hpp>

#include <boxpp/base/tpls/traits/Movable.hpp>
#include <boxpp/base/tpls/containers/Comparator.hpp>

namespace boxpp {

	/* Generic iterator type. */
	template<typename ContainerType, bool bReverse = false>
	class TIterator;

	/* Generic iterator type. */
	template<typename ElemType, template<typename> typename BaseContainerType, bool bReverse>
	class TIterator<BaseContainerType<ElemType>, bReverse>
	{
	public:
		template<typename, bool> friend class TIterator;
		typedef BaseContainerType<ElemType> ContainerType;
		typedef TIterator<BaseContainerType<ElemType>, bReverse> SelfType;
		typedef TIterator<BaseContainerType<ElemType>, !bReverse> ReverseType;
		typedef typename BaseContainerType<ElemType>::OffsetType OffsetType;

	public:
		TIterator()
			: Container(nullptr)
		{
		}

		TIterator(const ContainerType& Container, const OffsetType& Offset)
			: Current(Offset), Container(const_cast<ContainerType*>(&Container))
		{
		}

		TIterator(const SelfType& Other)
			: Current(Other.Current), Container(Other.Container)
		{
		}
		
		TIterator(SelfType&& Other)
			: Current(TMovable<OffsetType>::Movable(Other.Current)), Container(Other.Container)
		{
			Other.Container = nullptr;
		}

		TIterator(const ReverseType& Other)
			: Current(Other.Current), Container(Other.Container)
		{
		}

		TIterator(ReverseType&& Other)
			: Current(TMovable<OffsetType>::Movable(Other.Current)), Container(Other.Container)
		{
			Other.Container = nullptr;
		}

	protected:
		OffsetType Current;
		ContainerType* Container;

	public:
		FASTINLINE operator bool() const { return Container && Container->IsValid(Current); }
		FASTINLINE bool operator !() const { return !Container || !Container->IsValid(Current); }
		FASTINLINE OffsetType Offset() const { return Current; }

	public:
		FASTINLINE bool operator ==(const SelfType& Other) const {
			return this == &Other || (Container == Other.Container &&
				TEquals<OffsetType>::Compare(Current, Other.Current));
		}

		FASTINLINE bool operator !=(const SelfType& Other) const {
			return this != &Other && (Container != Other.Container ||
				!TEquals<OffsetType>::Compare(Current, Other.Current));
		}

		FASTINLINE SelfType& operator =(const SelfType& Other) {
			if (*this != Other) {
				Current = Other.Current;
				Container = Other.Container;
			}

			return *this;
		}

		FASTINLINE SelfType& operator =(SelfType&& Other) {
			if (*this != Other) {
				Swap(Current, Other.Current);
				Swap(Container, Other.Container);
			}

			return *this;
		}

		FASTINLINE ReverseType& operator =(const ReverseType& Other) {
			if (*this != Other) {
				Current = Other.Current;
				Container = Other.Container;
			}

			return *this;
		}

		FASTINLINE ReverseType& operator =(ReverseType&& Other) {
			if (*this != Other) {
				Swap(Current, Other.Current);
				Swap(Container, Other.Container);
			}

			return *this;
		}

	public:
		FASTINLINE SelfType& operator ++() {
			if (*this) { 
				if (bReverse)
					--Current;

				else ++Current;
			}
			return *this;
		}

		FASTINLINE SelfType& operator --() {
			if (*this) { 
				if (bReverse)
					++Current;

				else --Current;
			}
			return *this;
		}

		FASTINLINE SelfType operator ++(int) {
			SelfType Clone = *this;
			if (*this) { 
				if (bReverse)
					--Current;

				else ++Current; 
			}
			return Clone;
		}

		FASTINLINE SelfType operator --(int) {
			SelfType Clone = *this;
			if (*this) {
				if (bReverse)
					++Current;

				else --Current; 
			}
			return Clone;
		}

	public:
		template<typename = typename std::enable_if<!std::is_const_v<ContainerType>>::type>
		FASTINLINE ElemType& operator *() { return (*Container)[Current]; }

		template<typename = typename std::enable_if<!std::is_const_v<ContainerType>>::type>
		FASTINLINE ElemType* operator ->() { return &((*Container)[Current]); }

		FASTINLINE const ElemType& operator *() const { return (*Container)[Current]; }
		FASTINLINE const ElemType* operator ->() const { return &((*Container)[Current]); }
	};


	/* Generic iterator type. */
	template<typename ElemType, template<typename> typename BaseContainerType, bool bReverse>
	class TIterator<const BaseContainerType<ElemType>, bReverse>
	{
	public:
		template<typename, bool> friend class TIterator;
		typedef const BaseContainerType<ElemType> ContainerType;
		typedef TIterator<BaseContainerType<ElemType>, bReverse> NonConstType;
		typedef TIterator<const BaseContainerType<ElemType>, bReverse> SelfType;
		typedef TIterator<const BaseContainerType<ElemType>, !bReverse> ReverseType;
		typedef typename BaseContainerType<ElemType>::OffsetType OffsetType;

	public:
		TIterator()
			: Container(nullptr)
		{
		}

		TIterator(const ContainerType& Container, const OffsetType& Offset)
			: Current(Offset), Container(const_cast<ContainerType*>(&Container))
		{
		}

		TIterator(const SelfType& Other)
			: Current(Other.Current), Container(Other.Container)
		{
		}

		TIterator(const NonConstType& Other)
			: Current(Other.Current), Container(Other.Container)
		{
		}

		TIterator(SelfType&& Other)
			: Current(TMovable<OffsetType>::Movable(Other.Current)), Container(Other.Container)
		{
			Other.Container = nullptr;
		}

		TIterator(const ReverseType& Other)
			: Current(Other.Current), Container(Other.Container)
		{
		}

		TIterator(ReverseType&& Other)
			: Current(TMovable<OffsetType>::Movable(Other.Current)), Container(Other.Container)
		{
			Other.Container = nullptr;
		}

	protected:
		OffsetType Current;
		ContainerType* Container;

	public:
		FASTINLINE operator bool() const { return Container && Container->IsValid(Current); }
		FASTINLINE bool operator !() const { return !Container || !Container->IsValid(Current); }
		FASTINLINE OffsetType Offset() const { return Current; }

	public:
		FASTINLINE bool operator ==(const SelfType& Other) const {
			return this == &Other || (Container == Other.Container &&
				TEquals<OffsetType>::Compare(Current, Other.Current));
		}

		FASTINLINE bool operator !=(const SelfType& Other) const {
			return this != &Other && (Container != Other.Container ||
				!TEquals<OffsetType>::Compare(Current, Other.Current));
		}

		FASTINLINE SelfType& operator =(const SelfType& Other) {
			if (*this != Other) {
				Current = Other.Current;
				Container = Other.Container;
			}

			return *this;
		}

		FASTINLINE SelfType& operator =(SelfType&& Other) {
			if (*this != Other) {
				Swap(Current, Other.Current);
				Swap(Container, Other.Container);
			}

			return *this;
		}

		FASTINLINE ReverseType& operator =(const ReverseType& Other) {
			if (*this != Other) {
				Current = Other.Current;
				Container = Other.Container;
			}

			return *this;
		}

		FASTINLINE ReverseType& operator =(ReverseType&& Other) {
			if (*this != Other) {
				Swap(Current, Other.Current);
				Swap(Container, Other.Container);
			}

			return *this;
		}

	public:
		FASTINLINE SelfType& operator ++() {
			if (*this) {
				if (bReverse)
					--Current;

				else ++Current;
			}
			return *this;
		}

		FASTINLINE SelfType& operator --() {
			if (*this) {
				if (bReverse)
					++Current;

				else --Current;
			}
			return *this;
		}

		FASTINLINE SelfType operator ++(int) {
			SelfType Clone = *this;
			if (*this) {
				if (bReverse)
					--Current;

				else ++Current;
			}
			return Clone;
		}

		FASTINLINE SelfType operator --(int) {
			SelfType Clone = *this;
			if (*this) {
				if (bReverse)
					++Current;

				else --Current;
			}
			return Clone;
		}

	public:
		template<typename = typename std::enable_if<!std::is_const_v<ContainerType>>::type>
		FASTINLINE ElemType& operator *() { return (*Container)[Current]; }

		template<typename = typename std::enable_if<!std::is_const_v<ContainerType>>::type>
		FASTINLINE ElemType* operator ->() { return &((*Container)[Current]); }

		FASTINLINE const ElemType& operator *() const { return (*Container)[Current]; }
		FASTINLINE const ElemType* operator ->() const { return &((*Container)[Current]); }
	};

	template<typename ContainerType>
	using TReverseIterator = TIterator<ContainerType, true>;
}
