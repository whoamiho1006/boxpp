#pragma once
#include <boxpp/Base.hpp>
#include <boxpp/BaseTypes.hpp>

#include <boxpp/utils/TypeCompares.hpp>
#include <boxpp/containers/SortedArray.hpp>

namespace boxpp
{

	template<typename KeyType, typename ValueType>
	class TMap
	{
	private:
		class SortablePair
		{
		public:
			SortablePair(const KeyType& Key)
				: Key(Key)
			{
			}

			SortablePair(const KeyType& Key, const ValueType& InValue)
				: Key(Key)
			{
				Value.ConstructFrom(InValue);
			}

			SortablePair(KeyType&& Key, ValueType&& InValue)
				: Key(TMovable<KeyType>::Movable(Key))
			{
				Value.ConstructFrom(Forward<ValueType>(InValue));
			}

			SortablePair(const SortablePair& Other)
				: Key(Other.Key)
			{
				if (Other.Value)
				{
					Value.ConstructFrom(
						*Other.Value.GetRaw());
				}
			}

			SortablePair(SortablePair&& Other)
				: Key(TMovable<KeyType>::Movable(Other.Key))
			{
				if (Other.Value)
				{
					Value.ConstructFrom(TMovable<ValueType>
						::Movable(*Other.Value.GetRaw()));
				}
			}

		public:
			KeyType Key;
			TInstrusive<ValueType> Value;

		public:
			FASTINLINE bool operator ==(const SortablePair& Other) const { return !Compare(Key, Other.Key); }
			FASTINLINE bool operator !=(const SortablePair& Other) const { return Compare(Key, Other.Key); }
			FASTINLINE bool operator <=(const SortablePair& Other) const { return Compare(Key, Other.Key) <= 0; }
			FASTINLINE bool operator >=(const SortablePair& Other) const { return Compare(Key, Other.Key) >= 0; }
			FASTINLINE bool operator < (const SortablePair& Other) const { return Compare(Key, Other.Key) < 0; }
			FASTINLINE bool operator > (const SortablePair& Other) const { return Compare(Key, Other.Key) > 0; }
		};

	public:
		TMap(u32 InitialCapacity = 0)
			: Pairs(InitialCapacity)
		{
		}

		TMap(const TMap<KeyType, ValueType>& Other)
			: Pairs(Other.Pairs)
		{
		}

		TMap(TMap<KeyType, ValueType>&& Other)
			: Pairs(Forward<TSortedArray<SortablePair>>(Other.Pairs))
		{
		}

		~TMap()
		{

		}

	private:
		TSortedArray<SortablePair> Pairs;

	public:
		FASTINLINE bool operator ==(const TMap<KeyType, ValueType>& Other) { return this == &Other; }
		FASTINLINE bool operator !=(const TMap<KeyType, ValueType>& Other) { return this != &Other; }

		FASTINLINE ValueType& operator [](const KeyType& Key) {
			SortablePair HalfPair(Key);
			const SortablePair& FoundPair = Pairs[Pairs.Search(HalfPair)];

			return *(FoundPair.Value.GetRaw());
		}

		FASTINLINE const ValueType& operator [](const KeyType& Key) const {
			SortablePair HalfPair(Key);
			const SortablePair& FoundPair = Pairs[Pairs.Search(HalfPair)];

			return *(FoundPair.Value.GetRaw());
		}

	public:
		FASTINLINE TMap<KeyType, ValueType>& operator =(const TMap<KeyType, ValueType>& Other) {
			Pairs = Other.Pairs;
			return *this;
		}

		FASTINLINE TMap<KeyType, ValueType>& operator =(TMap<KeyType, ValueType>&& Other) {
			Pairs = Forward<TSortedArray<SortablePair>>(Other.Pairs);
			return *this;
		}

	public:
		FASTINLINE void Clear() {
			Pairs.Clear();
		}

		FASTINLINE bool ContainsKey(const KeyType& Key) const {
			SortablePair HalfPair(Key);
			return Pairs.Search(HalfPair) >= 0;
		}

		FASTINLINE bool ContainsValue(const ValueType& Value) const {
			for (const SortablePair& Each : Pairs) {
				if (Each.Value->GetRaw() &&
					!Compare(*Each.Value->GetRaw(), Value))
				{
					return true;
				}
			}

			return false;
		}

	public:
		FASTINLINE void GetKeys(TArray<KeyType>& Keys) const {
			for (const SortablePair& Each : Pairs) {
				Keys.Add(Each.Key);
			}
		}

	public:
		FASTINLINE bool Remove(const KeyType& Key) {
			SortablePair HalfPair(Key);
			return Pairs.Remove(HalfPair);
		}

	public:
		FASTINLINE bool Emplace(const KeyType& Key, const ValueType& Value, bool bReplace = true) {
			SortablePair HalfPair(Key);
			s32 Index = Pairs.Search(HalfPair);

			if (Index < 0)
				Index = Pairs.Add(Forward<SortablePair>(HalfPair));

			else if (!bReplace)
				return false;

			TInstrusive<ValueType>& ValueStorage = Pairs[Index].Value;

			if (ValueStorage)
				ValueStorage.Destruct();

			ValueStorage.ConstructFrom(Value);
			return true;
		}

		FASTINLINE bool Emplace(const KeyType& Key, ValueType&& Value, bool bReplace = true) {
			SortablePair HalfPair(Key);
			s32 Index = Pairs.Search(HalfPair);

			if (Index < 0)
				Index = Pairs.Add(Forward<SortablePair>(HalfPair));

			else if (!bReplace)
				return false;

			TInstrusive<ValueType>& ValueStorage = Pairs[Index].Value;

			if (ValueStorage)
				ValueStorage.Destruct();

			ValueStorage.ConstructFrom(Forward<ValueType>(Value));
			return true;
		}


		FASTINLINE bool Emplace(KeyType&& Key, ValueType&& Value, bool bReplace = true) {
			SortablePair HalfPair(Forward<KeyType>(Key));
			s32 Index = Pairs.Search(HalfPair);

			if (Index < 0)
				Index = Pairs.Add(Forward<SortablePair>(HalfPair));

			else if (!bReplace)
				return false;

			TInstrusive<ValueType>& ValueStorage = Pairs[Index].Value;

			if (ValueStorage)
				ValueStorage.Destruct();

			ValueStorage.ConstructFrom(Forward<ValueType>(Value));
			return true;
		}

	};

}
