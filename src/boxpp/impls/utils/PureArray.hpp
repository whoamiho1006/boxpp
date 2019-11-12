#pragma once
#include <boxpp.hpp>

#include <boxpp/utils/Movable.hpp>
#include <boxpp/utils/TypeCompares.hpp>
#include <boxpp/utils/Comparator.hpp>

#include <malloc.h>
#include <string.h>

namespace boxpp
{
	/*
		Pure Array that are used for 
		`Pre-constructed` layers.
	*/
	template<typename ElemType>
	class TPureArray
	{
	public:
		TPureArray() 
			: Storage(nullptr), Count(0)
		{
		}

		~TPureArray() { Clear(); }

	private:
		ElemType* Storage;
		ssize_t Count;

	public:
		/* Determines this array is empty or not. */
		FASTINLINE bool IsEmpty() const { return !Storage || !Count; }

		/* Determines this array is not empty (true) or empty (false). */
		FASTINLINE operator bool() const { return !IsEmpty(); }

		/* Determines this array is empty or not. */
		FASTINLINE bool operator !() const { return IsEmpty(); }

		/* Get size of this pure array. */
		FASTINLINE size_t GetSize() const { return size_t(Count); }

	public:
		FASTINLINE ElemType& operator[](ssize_t Index) { return Storage[Index]; }
		FASTINLINE const ElemType& operator[](ssize_t Index) const { return Storage[Index]; }

	public:
		/* Clear all items. */
		FASTINLINE void Clear() {
			if (Storage) {
				if (!IsTypePOD<ElemType>) {
					for (ssize_t i = 0; i < Count; i++) {
						Storage[i].~ElemType();
					}
				}

				::free(Storage);
			}

			Storage = nullptr;
			Count = 0;
		}

		/* Add an item into this array. */
		FASTINLINE ssize_t Add(const ElemType& Value) {
			if (Storage) {
				if (IsTypePOD<ElemType>) {
					Storage = (ElemType*)realloc(Storage,
						(sizeof(ElemType) * (Count + 1))
					);
				}

				else {
					ElemType* NewStorage = (ElemType*)malloc(
						(sizeof(ElemType) * (Count + 1))
					);

					/* Moves all items to new storage. */
					for (ssize_t i = 0; i < Count; i++) {
						new (&NewStorage[i]) ElemType(
							TMovable<ElemType>::Movable(Storage[i]));
					}

					/* And then, kill original items. */
					for (ssize_t i = 0; i < Count; i++) {
						Storage[i].~ElemType();
					}

					::free(Storage);
					Storage = NewStorage;
				}

				new (&Storage[Count++]) ElemType(Value);
				return Count - 1;
			}

			else {
				Storage = (ElemType*)malloc(
					(sizeof(ElemType) * (Count = 1))
				);

				new (&Storage[Count - 1]) ElemType(Value);
				return 0;
			}

			return -1;
		}

		/* Find index of specified item. */
		FASTINLINE ssize_t Find(const ElemType& Item) {
			if (Count) {
				for (ssize_t i = 0; i < Count; i++) {
					if (!Compare(Storage[i], Item))
						return i;
				}
			}

			return -1;
		}

		/* Remove an item at given index. */
		FASTINLINE bool RemoveAt(ssize_t Index) {
			if (Index >= 0 && Index < Count) {
				if (Count - 1 <= 0)
					Clear();

				else if (IsTypePOD<ElemType>) {
					::memmove(Storage + Index, Storage + Index + 1, Count - Index - 1);

					Storage = (ElemType*)realloc(Storage,
						(sizeof(ElemType) * (--Count))
					);
				}

				else {
					ElemType* NewStorage = (ElemType*)malloc(
						(sizeof(ElemType) * (Count - 1))
					);

					/* Moves front items to new storage. */
					for (ssize_t i = 0; i < Index; i++) {
						new (&NewStorage[i]) ElemType(
							TMovable<ElemType>::Movable(Storage[i]));
					}

					/* Moves back items to new storage. */
					for (ssize_t i = Index + 1; i < Count; i++) {
						new (&NewStorage[i - 1]) ElemType(
							TMovable<ElemType>::Movable(Storage[i]));
					}

					/* And then, kill original items. */
					for (ssize_t i = 0; i < Count; i++) {
						Storage[i].~ElemType();
					}

					::free(Storage);
					Storage = NewStorage;

					--Count;
				}

				return true;
			}

			return false;
		}
	};
}