#ifndef __X_ARRAY_H__
#define __X_ARRAY_H__

#pragma once

/*
===============================================================================

	List template
	Does not allocate memory until the first item is added.

===============================================================================
*/


/*
================
xListSortCompare<type>
================
*/
#ifdef __INTEL_COMPILER
// the intel compiler doesn't do the right thing here
template< class type >
X_INLINE int xListSortCompare(const type *a, const type *b) {
	assert(0);
	return 0;
}
#else
template< class type >
X_INLINE int xListSortCompare(const type *a, const type *b) {
	return *a - *b;
}
#endif

/*
================
xListNewElement<type>
================
*/
template< class type >
X_INLINE type *xListNewElement() {
	return new type;
}

/*
================
xSwap<type>
================
*/
template< class type >
X_INLINE void xSwap(type &a, type &b) {
	type c = a;
	a = b;
	b = c;
}

template< class type >
class xArray {
public:

	typedef int		cmp_t(const type *, const type *);
	typedef type	new_t();

	xArray(int newgranularity = 4);
	xArray(const xArray<type> &other);
	~xArray<type>();

	void			Clear();										// clear the list
	int				Count() const;									// returns number of elements in list
	int				NumAllocated() const;							// returns number of elements allocated for
	void			SetGranularity(int newgranularity);				// set new granularity
	int				Granularity() const;						// get the current granularity

	size_t			Allocated() const;							// returns total size of allocated memory
	size_t			Size() const;									// returns total size of allocated memory including size of list type
	size_t			MemoryUsed() const;							// returns size of the used elements in the list

	xArray<type> &	operator=(const xArray<type> &other);
	const type &	operator[](int index) const;
	type &			operator[](int index);

  const type &	Get(int index) const { return (*this)[index]; }
  type &			Get(int index){ return (*this)[index]; }

  bool      operator == (const xArray<type>& other);
  bool      operator != (const xArray<type>& other){ return !(*this == other); }

	void			Condense();									// resizes list to exactly the number of elements it contains
	void			Resize(int newsize);								// resizes list to the given number of elements
	void			Resize(int newsize, int newgranularity	);			// resizes list and sets new granularity
	void			SetCount(int newnum, bool resize = true);			// set number of elements in list and resize to exactly this number if necessary
	void			AssureSize(int newSize);							// assure list has given number of elements, but leave them uninitialized
	void			AssureSize(int newSize, const type &initValue);	// assure list has given number of elements and initialize any new elements
	void			AssureSizeAlloc(int newSize, new_t *allocator);	// assure the pointer list has the given number of elements and allocate any new elements

	type *			Ptr();										// returns a pointer to the list
	const type *	Ptr() const;									// returns a pointer to the list
	type &			Alloc();										// returns reference to a new data element at the end of the list
	int				Append(const type & obj);							// append element
	int				Append(const xArray<type> &other);				// append list
	int				AddUnique(const type & obj);						// add unique element
	int				Insert(const type & obj, int index = 0);			// insert the element at the given index
	int				FindIndex(const type & obj) const;				// find the index for the given element
	type *			Find(const type & obj) const;						// find pointer to the given element
	int				FindNull() const;								// find the index for the first NULL pointer in the list
	int				IndexOf(const type * obj) const;					// returns the index for the pointer to an element in the list
	bool			RemoveIndex(int index);							// remove the element at the given index
	bool			Remove(const type & obj);							// remove the element
	void			Sort(cmp_t *compare = (cmp_t *)&xListSortCompare<type>);
	void			SortSubSection(int startIndex, int endIndex, cmp_t *compare = (cmp_t *)&xListSortCompare<type>);
	void			Swap(xArray<type> &other);						// swap the contents of the lists
  void      Reverse();
	void			DeleteContents(bool clear);						// delete the contents of the list

private:
	int				count;
	int				size;
	int				granularity;
	type *			list;
};

/*
================
xArray<type>::xArray(int)
================
*/
template< class type >
X_INLINE xArray<type>::xArray(int newgranularity) {
	assert(newgranularity > 0);

	list		= NULL;
	granularity	= newgranularity;
	Clear();
}

/*
================
xArray<type>::xArray(const xArray<type> &other)
================
*/
template< class type >
X_INLINE xArray<type>::xArray(const xArray<type> &other) {
	list = NULL;
	*this = other;
}

/*
================
xArray<type>::~xArray<type>
================
*/
template< class type >
X_INLINE xArray<type>::~xArray() {
	Clear();
}

/*
================
xArray<type>::Clear

Frees up the memory allocated by the list.  Assumes that type automatically handles freeing up memory.
================
*/
template< class type >
X_INLINE void xArray<type>::Clear() {
	if (list) {
		delete[] list;
	}

	list	= NULL;
	count		= 0;
	size	= 0;
}

/*
================
xArray<type>::DeleteContents

Calls the destructor of all elements in the list.  Conditionally frees up memory used by the list.
Note that this only works on lists containing pointers to objects and will cause a compiler error
if called with non-pointers.  Since the list was not responsible for allocating the object, it has
no information on whether the object still exists or not, so care must be taken to ensure that
the pointers are still valid when this function is called.  Function will set all pointers in the
list to NULL.
================
*/
template< class type >
X_INLINE void xArray<type>::DeleteContents(bool clear) {
	int i;

	for(i = 0; i < count; i++) {
		delete list[ i ];
		list[ i ] = NULL;
	}

	if (clear) {
		Clear();
	} else {
		memset(list, 0, size * sizeof(type));
	}
}

/*
================
xArray<type>::Allocated

return total memory allocated for the list in bytes, but doesn't take into account additional memory allocated by type
================
*/
template< class type >
X_INLINE size_t xArray<type>::Allocated() const {
	return size * sizeof(type);
}

/*
================
xArray<type>::Size

return total size of list in bytes, but doesn't take into account additional memory allocated by type
================
*/
template< class type >
X_INLINE size_t xArray<type>::Size() const {
	return sizeof(xArray<type>) + Allocated();
}

/*
================
xArray<type>::MemoryUsed
================
*/
template< class type >
X_INLINE size_t xArray<type>::MemoryUsed() const {
	return count * sizeof(*list);
}

/*
================
xArray<type>::Count

Returns the number of elements currently contained in the list.
Note that this is NOT an indication of the memory allocated.
================
*/
template< class type >
X_INLINE int xArray<type>::Count() const {
	return count;
}

/*
================
xArray<type>::NumAllocated

Returns the number of elements currently allocated for.
================
*/
template< class type >
X_INLINE int xArray<type>::NumAllocated() const {
	return size;
}

/*
================
xArray<type>::SetCount

Resize to the exact size specified irregardless of granularity
================
*/
template< class type >
X_INLINE void xArray<type>::SetCount(int newnum, bool resize) {
	assert(newnum >= 0);
	if (resize || newnum > size) {
		Resize(newnum);
	}
	count = newnum;
}

/*
================
xArray<type>::SetGranularity

Sets the base size of the array and resizes the array to match.
================
*/
template< class type >
X_INLINE void xArray<type>::SetGranularity(int newgranularity) {
	int newsize;

	assert(newgranularity > 0);
	granularity = newgranularity;

	if (list) {
		// resize it to the closest level of granularity
		newsize = count + granularity - 1;
		newsize -= newsize % granularity;
		if (newsize != size) {
			Resize(newsize);
		}
	}
}

/*
================
xArray<type>::Granularity

Get the current granularity.
================
*/
template< class type >
X_INLINE int xArray<type>::Granularity() const {
	return granularity;
}

/*
================
xArray<type>::Condense

Resizes the array to exactly the number of elements it contains or frees up memory if empty.
================
*/
template< class type >
X_INLINE void xArray<type>::Condense() {
	if (list) {
		if (count) {
			Resize(count);
		} else {
			Clear();
		}
	}
}

/*
================
xArray<type>::Resize

Allocates memory for the amount of elements requested while keeping the contents intact.
Contents are copied using their = operator so that data is correnctly instantiated.
================
*/
template< class type >
X_INLINE void xArray<type>::Resize(int newsize) {
	type	*temp;
	int		i;

	assert(newsize >= 0);

	// free up the list if no data is being reserved
	if (newsize <= 0) {
		Clear();
		return;
	}

	if (newsize == size) {
		// not changing the size, so just exit
		return;
	}

	temp	= list;
	size	= newsize;
	if (size < count) {
		count = size;
	}

  int minGranularity = size >> 1;
  if(minGranularity < 2)
    minGranularity = 2;
  if(granularity < minGranularity)
    granularity = minGranularity;
  else{
    int maxGranularity = size << 1;
    if(granularity > maxGranularity)
      granularity = maxGranularity;
  }

	// copy the old list into our new one
	list = new type[ size ];
	for(i = 0; i < count; i++) {
		list[ i ] = temp[ i ];
	}

	// delete the old list if it exists
	if (temp) {
		delete[] temp;
	}
}

/*
================
xArray<type>::Resize

Allocates memory for the amount of elements requested while keeping the contents intact.
Contents are copied using their = operator so that data is correnctly instantiated.
================
*/
template< class type >
X_INLINE void xArray<type>::Resize(int newsize, int newgranularity) {
	type	*temp;
	int		i;

	assert(newsize >= 0);

	assert(newgranularity > 0);
	granularity = newgranularity;

	// free up the list if no data is being reserved
	if (newsize <= 0) {
		Clear();
		return;
	}

	temp	= list;
	size	= newsize;
	if (size < count) {
		count = size;
	}

	// copy the old list into our new one
	list = new type[ size ];
	for(i = 0; i < count; i++) {
		list[ i ] = temp[ i ];
	}

	// delete the old list if it exists
	if (temp) {
		delete[] temp;
	}
}

/*
================
xArray<type>::AssureSize

Makes sure the list has at least the given number of elements.
================
*/
template< class type >
X_INLINE void xArray<type>::AssureSize(int newSize) {
	int newNum = newSize;

	if (newSize > size) {

		if (granularity == 0) {	// this is a hack to fix our memset classes
			granularity = 16;
		}
		newSize += granularity - 1;
		newSize -= newSize % granularity;
		Resize(newSize);
	}

	count = newNum;
}

/*
================
xArray<type>::AssureSize

Makes sure the list has at least the given number of elements and initialize any elements not yet initialized.
================
*/
template< class type >
X_INLINE void xArray<type>::AssureSize(int newSize, const type &initValue) {
	int newNum = newSize;

	if (newSize > size) {

		if (granularity == 0) {	// this is a hack to fix our memset classes
			granularity = 16;
		}

		newSize += granularity - 1;
		newSize -= newSize % granularity;
		count = size;
		Resize(newSize);

		for (int i = count; i < newSize; i++) {
			list[i] = initValue;
		}
	}

	count = newNum;
}

/*
================
xArray<type>::AssureSizeAlloc

Makes sure the list has at least the given number of elements and allocates any elements using the allocator.

NOTE: This function can only be called on lists containing pointers. Calling it
on non-pointer lists will cause a compiler error.
================
*/
template< class type >
X_INLINE void xArray<type>::AssureSizeAlloc(int newSize, new_t *allocator) {
	int newNum = newSize;

	if (newSize > size) {

		if (granularity == 0) {	// this is a hack to fix our memset classes
			granularity = 16;
		}

		newSize += granularity - 1;
		newSize -= newSize % granularity;
		count = size;
		Resize(newSize);

		for (int i = count; i < newSize; i++) {
			list[i] = (*allocator)();
		}
	}

	count = newNum;
}

/*
================
xArray<type>::operator=

Copies the contents and size attributes of another list.
================
*/
template< class type >
X_INLINE xArray<type> &xArray<type>::operator=(const xArray<type> &other) {
	int	i;

	Clear();

	count			= other.count;
	size		= other.size;
	granularity	= other.granularity;

	if (size) {
		list = new type[ size ];
		for(i = 0; i < count; i++) {
			list[ i ] = other.list[ i ];
		}
	}

	return *this;
}

/*
================
xArray<type>::operator[] const

Access operator.  Index must be within range or an assert will be issued in debug builds.
Release builds do no range checking.
================
*/
template< class type >
X_INLINE const type &xArray<type>::operator[](int index) const {
	assert(index >= 0);
	assert(index < count);

	return list[ index ];
}

/*
================
xArray<type>::operator[]

Access operator.  Index must be within range or an assert will be issued in debug builds.
Release builds do no range checking.
================
*/
template< class type >
X_INLINE type &xArray<type>::operator[](int index) {
	assert(index >= 0);
	assert(index < count);

	return list[ index ];
}

template< class type >
X_INLINE bool xArray<type>::operator == (const xArray<type>& other)
{
  if(count != other.count)
    return false;

  for(int i = 0; i < count; i++)
    if(list[i] != other.list[i])
      return false;

  return true;
}

/*
================
xArray<type>::Ptr

Returns a pointer to the begining of the array.  Useful for iterating through the list in loops.

Note: may return NULL if the list is empty.

FIXME: Create an iterator template for this kind of thing.
================
*/
template< class type >
X_INLINE type *xArray<type>::Ptr() {
	return list;
}

/*
================
xArray<type>::Ptr

Returns a pointer to the begining of the array.  Useful for iterating through the list in loops.

Note: may return NULL if the list is empty.

FIXME: Create an iterator template for this kind of thing.
================
*/
template< class type >
const X_INLINE type *xArray<type>::Ptr() const {
	return list;
}

/*
================
xArray<type>::Alloc

Returns a reference to a new data element at the end of the list.
================
*/
template< class type >
X_INLINE type &xArray<type>::Alloc() {
	if (!list) {
		Resize(granularity);
	}

	if (count == size) {
		Resize(size + granularity);
	}

	return list[ count++ ];
}

/*
================
xArray<type>::Append

Increases the size of the list by one element and copies the supplied data into it.

Returns the index of the new element.
================
*/
template< class type >
X_INLINE int xArray<type>::Append(const type & value) {
	if (!list) {
		Resize(granularity);
	}
  const type * obj = &value;

	if (count == size) {
		if (granularity == 0) {	// this is a hack to fix our memset classes
			granularity = 16;
		}
		int newsize = size + granularity;
    newsize -= newsize % granularity;

    if(obj >= list && obj < list + count){
      int i = obj - list;
      Resize(newsize);
      obj = list + i;
    }else
		  Resize(newsize);
	}

	list[count] = *obj;
	return count++;
}


/*
================
xArray<type>::Insert

Increases the size of the list by at leat one element if necessary 
and inserts the supplied data into it.

Returns the index of the new element.
================
*/
template< class type >
X_INLINE int xArray<type>::Insert(const type & value, int index) {
	if (!list) {
		Resize(granularity);
	}
  const type * obj = &value;

	if (count == size) {
		if (granularity == 0) {	// this is a hack to fix our memset classes
			granularity = 16;
		}

    int newsize = size + granularity;
    newsize -= newsize % granularity;

    if(obj >= list && obj < list + count){
      int i = obj - list;
      Resize(newsize);
      obj = list + i;
    }else
      Resize(newsize);
	}

	if (index < 0) {
		index = 0;
	}
	else if (index > count) {
		index = count;
	}
	for (int i = count; i > index; --i) {
		list[i] = list[i-1];
	}
	count++;
	list[index] = *obj;
	return index;
}

/*
================
xArray<type>::Append

adds the other list to this one

Returns the size of the new combined list
================
*/
template< class type >
X_INLINE int xArray<type>::Append(const xArray<type> &other) {
	if (!list) {
		if (granularity == 0) {	// this is a hack to fix our memset classes
			granularity = 16;
		}
		Resize(granularity);
	}

	int n = other.Count();
	for (int i = 0; i < n; i++) {
		Append(other[i]);
	}

	return Count();
}

/*
================
xArray<type>::AddUnique

Adds the data to the list if it doesn't already exist.  Returns the index of the data in the list.
================
*/
template< class type >
X_INLINE int xArray<type>::AddUnique(const type & obj) {
	int index;

	index = FindIndex(obj);
	if (index < 0) {
		index = Append(obj);
	}

	return index;
}

/*
================
xArray<type>::FindIndex

Searches for the specified data in the list and returns it's index.  Returns -1 if the data is not found.
================
*/
template< class type >
X_INLINE int xArray<type>::FindIndex(const type & obj) const {
	int i;

	for(i = 0; i < count; i++) {
		if (list[ i ] == obj) {
			return i;
		}
	}

	// Not found
	return -1;
}

/*
================
xArray<type>::Find

Searches for the specified data in the list and returns it's address. Returns NULL if the data is not found.
================
*/
template< class type >
X_INLINE type *xArray<type>::Find(const type & obj) const {
	int i;

	i = FindIndex(obj);
	if (i >= 0) {
		return &list[ i ];
	}

	return NULL;
}

/*
================
xArray<type>::FindNull

Searches for a NULL pointer in the list.  Returns -1 if NULL is not found.

NOTE: This function can only be called on lists containing pointers. Calling it
on non-pointer lists will cause a compiler error.
================
*/
template< class type >
X_INLINE int xArray<type>::FindNull() const {
	int i;

	for(i = 0; i < count; i++) {
		if (list[ i ] == NULL) {
			return i;
		}
	}

	// Not found
	return -1;
}

/*
================
xArray<type>::IndexOf

Takes a pointer to an element in the list and returns the index of the element.
This is NOT a guarantee that the object is really in the list. 
================
*/
template< class type >
X_INLINE int xArray<type>::IndexOf(const type * objptr) const
{
	int index = objptr - list;
  return index >= 0 && index < count ? index : -1;
}

/*
================
xArray<type>::RemoveIndex

Removes the element at the specified index and moves all data following the element down to fill in the gap.
The number of elements in the list is reduced by one.  Returns false if the index is outside the bounds of the list.
Note that the element is not destroyed, so any memory used by it may not be freed until the destruction of the list.
================
*/
template< class type >
X_INLINE bool xArray<type>::RemoveIndex(int index) {
	int i;

	if ((index < 0) || (index >= count)) {
		return false;
	}

	assert(list != NULL);

	count--;
	for(i = index; i < count; i++) {
		list[ i ] = list[ i + 1 ];
	}

	return true;
}

/*
================
xArray<type>::Remove

Removes the element if it is found within the list and moves all data following the element down to fill in the gap.
The number of elements in the list is reduced by one.  Returns false if the data is not found in the list.  Note that
the element is not destroyed, so any memory used by it may not be freed until the destruction of the list.
================
*/
template< class type >
X_INLINE bool xArray<type>::Remove(const type & obj) {
	int index;

	index = FindIndex(obj);
	if (index >= 0) {
		return RemoveIndex(index);
	}
	
	return false;
}

/*
================
xArray<type>::Sort

Performs a qsort on the list using the supplied comparison function.  Note that the data is merely moved around the
list, so any pointers to data within the list may no longer be valid.
================
*/
template< class type >
X_INLINE void xArray<type>::Sort(cmp_t *compare) {
	if (!list) {
		return;
	}
	typedef int cmp_c(const void *, const void *);

	cmp_c *vCompare = (cmp_c *)compare;
	qsort((void *)list, (size_t)count, sizeof(type), vCompare);
}

/*
================
xArray<type>::SortSubSection

Sorts a subsection of the list.
================
*/
template< class type >
X_INLINE void xArray<type>::SortSubSection(int startIndex, int endIndex, cmp_t *compare) {
	if (!list) {
		return;
	}
	if (startIndex < 0) {
		startIndex = 0;
	}
	if (endIndex >= count) {
		endIndex = count - 1;
	}
	if (startIndex >= endIndex) {
		return;
	}
	typedef int cmp_c(const void *, const void *);

	cmp_c *vCompare = (cmp_c *)compare;
	qsort((void *)(&list[startIndex]), (size_t)(endIndex - startIndex + 1), sizeof(type), vCompare);
}

/*
================
xArray<type>::Swap

Swaps the contents of two lists
================
*/
template< class type >
X_INLINE void xArray<type>::Swap(xArray<type> &other) {
	xSwap(count, other.count);
	xSwap(size, other.size);
	xSwap(granularity, other.granularity);
	xSwap(list, other.list);
}

/*
================
xArray<type>::Reverse
================
*/
template< class type >
X_INLINE void xArray<type>::Reverse()
{
  int mid = count>>1;
  for(int i = 0, j = count-1; i < mid; i++, j--)
    xSwap(list[i], list[j]);
}

#endif // __X_ARRAY_H__
