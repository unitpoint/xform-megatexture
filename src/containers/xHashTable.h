#ifndef __X_HASH_TABLE_H__
#define __X_HASH_TABLE_H__

/*
===============================================================================

	General hash table. Slower than idHashIndex but it can also be used for
	linked lists and other data structures than just indexes or arrays.

===============================================================================
*/

template<class T>
struct xHashTableKey
{
  T value;

  xHashTableKey(const T& k): value(k){}
  int Cmp(const xHashTableKey& b) const { return value < b.value ? -1 : (value > b.value ? 1 : 0); }
  int Hash() const { return (int)value; }
};

typedef xHashTableKey<int> xHashTableIndexKey;
typedef xHashTableKey<float> xHashTableFloatKey;
typedef xHashTableKey<void*> xHashTablePtrKey;
typedef xString xHashTableStringKey;

#define ITERATE_CONTINUE      0
#define ITERATE_BREAK         1
#define ITERATE_DELETE_ITEM   2

struct xHashLabel { int value; };
struct xHashKeyLabel { int value; };

template< class KeyType, class Type >
class xHashTable
{
public:

  typedef int (*IterateFunc)(const KeyType& k, Type& v, void * params);
  
  xHashTable(int newtablesize = 256);
	xHashTable(const xHashTable<KeyType, Type> &map);
	~xHashTable();

					// returns total size of allocated memory
	size_t			Allocated() const;
					// returns total size of allocated memory including size of hash table type
	size_t			Size() const;

	Type *    Get(const KeyType& key) const;
	Type *    Set(const KeyType& key, const Type& value);
  void			Add(const KeyType& key, const Type& value);
	bool			Remove(const KeyType& key);

  void      Grow();
	void			Clear();
	void			DeleteContents();

					// the entire contents can be itterated over, but note that the
					// exact index for a given element may change when new elements are added
	int				Count() const;
	Type *    Value(int index) const;

	int				Spread() const;
  Type *    ForEach(IterateFunc f, void * params);

  bool      First(const KeyType& key, xHashLabel& label, const KeyType *& foundKey, const Type *& foundValue) const;
  bool      First(const KeyType& key, xHashKeyLabel& keyLabel, xHashLabel& label, const KeyType *& foundKey, const Type *& foundValue) const;
  bool      Next(xHashLabel& label, const KeyType *& foundKey, const Type *& foundValue) const;

  bool      FirstKey(xHashKeyLabel& keyLabel, xHashLabel& label, const KeyType *& foundKey, const Type *& foundValue) const;
  bool      NextKey(xHashKeyLabel& keyLabel, xHashLabel& label, const KeyType *& foundKey, const Type *& foundValue) const;
  bool      NextKey2(xHashKeyLabel& keyLabel, xHashLabel& label, const KeyType *& foundKey, const Type *& foundValue) const;

  bool      Remove(xHashKeyLabel& keyLabel, xHashLabel& label);

  void      ToList(xArray<Type*>& out);
  void      ToList(xArray<Type>& out);

protected:

	struct hashnode_t {
		KeyType key;
		Type		value;
		hashnode_t *next;

		hashnode_t(const KeyType& k, Type v, hashnode_t *n) : key(k), value(v), next(n) {};
	};

	hashnode_t **	heads;

	int				tablesize, initialtablesize;
	int				count;
	int				tablesizemask;

  bool      addingMode;
};

/*
================
xHashTable<KeyType, Type>::xHashTable
================
*/
template< class KeyType, class Type >
X_INLINE xHashTable<KeyType, Type>::xHashTable(int newtablesize) {

	assert(xMath::IsPowerOfTwo(newtablesize));

	tablesize = initialtablesize = newtablesize;
	assert(tablesize > 0);

	heads = new hashnode_t *[ tablesize ];
	memset(heads, 0, sizeof(*heads) * tablesize);

	count		= 0;

	tablesizemask = tablesize - 1;
}

/*
================
xHashTable<KeyType, Type>::xHashTable
================
*/
template< class KeyType, class Type >
X_INLINE xHashTable<KeyType, Type>::xHashTable(const xHashTable<KeyType, Type> &map) {
	int			i;
	hashnode_t	*node;
	hashnode_t	**prev;

	assert(map.tablesize > 0);

	tablesize		= map.tablesize;
  initialtablesize = map.initialtablesize;
	heads			= new hashnode_t *[ tablesize ];
	count		= map.count;
	tablesizemask	= map.tablesizemask;

	for(i = 0; i < tablesize; i++) {
		if (!map.heads[ i ]) {
			heads[ i ] = NULL;
			continue;
		}

		prev = &heads[ i ];
		for(node = map.heads[ i ]; node != NULL; node = node->next) {
			*prev = new hashnode_t(node->key, node->value, NULL);
			prev = &(*prev)->next;
		}
	}
}

/*
================
xHashTable<KeyType, Type>::~xHashTable<KeyType, Type>
================
*/
template< class KeyType, class Type >
X_INLINE xHashTable<KeyType, Type>::~xHashTable() {
	Clear();
	delete[] heads;
}

/*
================
xHashTable<KeyType, Type>::Allocated
================
*/
template< class KeyType, class Type >
X_INLINE size_t xHashTable<KeyType, Type>::Allocated() const {
	return sizeof(heads) * tablesize + sizeof(*heads) * count;
}

/*
================
xHashTable<KeyType, Type>::Size
================
*/
template< class KeyType, class Type >
X_INLINE size_t xHashTable<KeyType, Type>::Size() const {
	return sizeof(xHashTable<KeyType, Type>) + sizeof(heads) * tablesize + sizeof(*heads) * count;
}

/*
================
xHashTable<KeyType, Type>::Set
================
*/
template< class KeyType, class Type >
X_INLINE Type * xHashTable<KeyType, Type>::Set(const KeyType& key, const Type& value)
{
#if 0
  if((count>>1) > tablesize)
    Grow();
#endif

	hashnode_t *node, **nextPtr;
	int hash, s;

	hash = key.Hash() & tablesizemask;
	for(nextPtr = &(heads[hash]), node = *nextPtr; node != NULL; nextPtr = &(node->next), node = *nextPtr) {
		s = node->key.Cmp(key);
		if (s == 0) {
			node->value = value;
			return &node->value;
		}
		if (s > 0) {
			break;
		}
	}

	count++;

  hashnode_t * newNode = new hashnode_t(key, value, heads[ hash ]);
	*nextPtr = newNode;
	newNode->next = node;
  return &newNode->value;
}

/*
================
xHashTable<KeyType, Type>::Add
================
*/
template< class KeyType, class Type >
X_INLINE void xHashTable<KeyType, Type>::Add(const KeyType& key, const Type& value)
{
  addingMode = true;

#if 0
  if((count>>1) > tablesize)
    Grow();
#endif

	hashnode_t *node, **nextPtr;
	int hash, s;

	hash = key.Hash() & tablesizemask;
	for(nextPtr = &(heads[hash]), node = *nextPtr; 0 && node != NULL; nextPtr = &(node->next), node = *nextPtr) {
		s = node->key.Cmp(key);
		if (s >= 0) {
			break;
		}
	}

	count++;

	*nextPtr = new hashnode_t(key, value, heads[ hash ]);
	(*nextPtr)->next = node;
}

/*
================
xHashTable<KeyType, Type>::Get
================
*/
template< class KeyType, class Type >
X_INLINE Type * xHashTable<KeyType, Type>::Get(const KeyType& key) const {
	hashnode_t *node;
	int hash, s;

	hash = key.Hash() & tablesizemask;
	for(node = heads[ hash ]; node != NULL; node = node->next) {
		s = node->key.Cmp(key);
		if(s == 0)
			return &node->value;
		
		if(s > 0)
			break;
	}
  return NULL;
}

/*
================
xHashTable<KeyType, Type>::First
================
*/
template< class KeyType, class Type >
X_INLINE bool xHashTable<KeyType, Type>::First(const KeyType& key, xHashLabel& label, const KeyType *& foundKey, const Type *& foundValue) const 
{
	int hash = key.Hash() & tablesizemask;
  for(hashnode_t * node = heads[hash]; node; node = node->next){
    if(node->key.Cmp(key) == 0){
      label.value = (int)node;
      foundKey = &node->key;
      foundValue = &node->value;
      return true;
    }
  }
  return false;
}

/*
================
xHashTable<KeyType, Type>::First
================
*/
template< class KeyType, class Type >
X_INLINE bool xHashTable<KeyType, Type>::First(const KeyType& key, xHashKeyLabel& keyLabel, xHashLabel& label, const KeyType *& foundKey, const Type *& foundValue) const 
{
	int hash = key.Hash() & tablesizemask;
  for(hashnode_t * node = heads[hash]; node; node = node->next){
    if(node->key.Cmp(key) == 0){
      keyLabel.value = hash;
      label.value = (int)node;
      foundKey = &node->key;
      foundValue = &node->value;
      return true;
    }
  }
  return false;
}

/*
================
xHashTable<KeyType, Type>::Next
================
*/
template< class KeyType, class Type >
X_INLINE bool xHashTable<KeyType, Type>::Next(xHashLabel& label, const KeyType *& foundKey, const Type *& foundValue) const 
{
  hashnode_t * node = ((hashnode_t*)label.value)->next;
  for(; node; node = node->next){
    if(node->key.Cmp(*foundKey) == 0){
      label.value = (int)node;
      foundKey = &node->key;
      foundValue = &node->value;
      return true;
    }
  }
  return false;
}

/*
================
xHashTable<KeyType, Type>::FirstKey
================
*/
template< class KeyType, class Type >
X_INLINE bool xHashTable<KeyType, Type>::FirstKey(xHashKeyLabel& keyLabel, xHashLabel& label, const KeyType *& foundKey, const Type *& foundValue) const 
{
	for(int i = 0; i < tablesize; i++){
    if(heads[i]){
      keyLabel.value = i;
      label.value = (int)heads[i];
      foundKey = &heads[i]->key;
      foundValue = &heads[i]->value;
      return true;
    }
  }
  return false;
}

/*
================
xHashTable<KeyType, Type>::NextKey
================
*/
template< class KeyType, class Type >
X_INLINE bool xHashTable<KeyType, Type>::NextKey(xHashKeyLabel& keyLabel, xHashLabel& label, const KeyType *& foundKey, const Type *& foundValue) const
{
  hashnode_t * node = ((hashnode_t*)label.value)->next;
  for(; node; node = node->next){
    if(node->key.Cmp(*foundKey) != 0){
      label.value = (int)node;
      foundKey = &node->key;
      foundValue = &node->value;
      return true;
    }
  }
  for(int i = keyLabel.value+1; i < tablesize; i++){
    if(heads[i]){
      keyLabel.value = i;
      label.value = (int)heads[i];
      foundKey = &heads[i]->key;
      foundValue = &heads[i]->value;
      return true;
    }
  }
  return false;
}

/*
================
xHashTable<KeyType, Type>::NextKey2
================
*/
template< class KeyType, class Type >
X_INLINE bool xHashTable<KeyType, Type>::NextKey2(xHashKeyLabel& keyLabel, xHashLabel& label, const KeyType *& foundKey, const Type *& foundValue) const
{
  for(int i = keyLabel.value+1; i < tablesize; i++){
    if(heads[i]){
      keyLabel.value = i;
      label.value = (int)heads[i];
      foundKey = &heads[i]->key;
      foundValue = &heads[i]->value;
      return true;
    }
  }
  return false;
}

/*
================
xHashTable<KeyType, Type>::Value

the entire contents can be itterated over, but note that the
exact index for a given element may change when new elements are added
================
*/
template< class KeyType, class Type >
X_INLINE Type * xHashTable<KeyType, Type>::Value(int index) const {
	hashnode_t	*node;
	int			count;
	int			i;

	if ((index < 0) || (index > count)) {
		assert(0);
		return NULL;
	}

	count = 0;
	for(i = 0; i < tablesize; i++) {
		for(node = heads[ i ]; node != NULL; node = node->next) {
			if (count == index) {
				return &node->value;
			}
			count++;
		}
	}

	return NULL;
}

template< class KeyType, class Type >
X_INLINE Type * xHashTable<KeyType, Type>::ForEach(IterateFunc f, void * params)
{
	hashnode_t * node, * prev, * next;
	for(int i = 0; i < tablesize; i++) {
    prev = NULL;
		for(node = heads[i]; node != NULL; prev = node, node = next){
      next = node->next;
      int flags = f(node->key, node->value, params);
      if(flags & ITERATE_DELETE_ITEM){
				if(prev)
					prev->next = node->next;
				else
					heads[i] = node->next;				

				count--;
				delete node;

        if(flags & ITERATE_BREAK)
          return NULL;

        continue;
      }
      if(flags & ITERATE_BREAK)
        return &node->value;
		}
	}
	return NULL;
}

template< class KeyType, class Type >
X_INLINE void xHashTable<KeyType, Type>::ToList(xArray<Type*>& out)
{
	for(int i = 0; i < tablesize; i++){
		for(hashnode_t * node = heads[i]; node != NULL; node = node->next)
      out.Append(&node->value);
	}
}

template< class KeyType, class Type >
X_INLINE void xHashTable<KeyType, Type>::ToList(xArray<Type>& out)
{
	for(int i = 0; i < tablesize; i++){
		for(hashnode_t * node = heads[i]; node != NULL; node = node->next)
      out.Append(node->value);
	}
}

/*
================
xHashTable<KeyType, Type>::Remove
================
*/
template< class KeyType, class Type >
X_INLINE bool xHashTable<KeyType, Type>::Remove(const KeyType& key) {
	hashnode_t	**head;
	hashnode_t	*node;
	hashnode_t	*prev;
	int			hash;

	hash = key.Hash() & tablesizemask;
	head = &heads[ hash ];
	if (*head) {
		for(prev = NULL, node = *head; node != NULL; prev = node, node = node->next) {
			if (node->key.Cmp(key) == 0) {
				if (prev) {
					prev->next = node->next;
				} else {
					*head = node->next;
				}

				count--;
				delete node;
				return true;
			}
		}
	}

	return false;
}

template< class KeyType, class Type >
X_INLINE bool xHashTable<KeyType, Type>::Remove(xHashKeyLabel& keyLabel, xHashLabel& label)
{
	hashnode_t	*node;
	hashnode_t	*prev;

  hashnode_t	** head = &heads[keyLabel.value];
	if(*head)
		for(prev = NULL, node = *head; node != NULL; prev = node, node = node->next) {
			if (node == ((hashnode_t*)label.value)) {
				if (prev) {
					prev->next = node->next;
				} else {
					*head = node->next;
				}
				count--;
				delete node;
				return true;
			}
		}

	return false;
}

/*
================
xHashTable<KeyType, Type>::Grow
================
*/
template< class KeyType, class Type >
X_INLINE void xHashTable<KeyType, Type>::Grow()
{
  hashnode_t ** oldheads = heads;
  int oldtablesize = tablesize;

  tablesize <<= 1;
	tablesizemask = tablesize - 1;
	count = 0;

	heads = new hashnode_t *[ tablesize ];
	memset(heads, 0, sizeof(*heads) * tablesize);

	for(int i = 0; i < oldtablesize; i++) {
		hashnode_t * next = oldheads[ i ];
		while(next != NULL) {
			hashnode_t * node = next;
			next = next->next;
      
      if(!addingMode)
        Set(node->key, node->value);
      else
        Add(node->key, node->value);

			delete node;
		}
	}
  delete [] oldheads;
}

/*
================
xHashTable<KeyType, Type>::Clear
================
*/
template< class KeyType, class Type >
X_INLINE void xHashTable<KeyType, Type>::Clear() {
	int			i;
	hashnode_t	*node;
	hashnode_t	*next;

	for(i = 0; i < tablesize; i++) {
		next = heads[ i ];
		while(next != NULL) {
			node = next;
			next = next->next;
			delete node;
		}

		heads[ i ] = NULL;
	}

  if(tablesize != initialtablesize){
    delete [] heads;
    tablesize = initialtablesize;
	  heads = new hashnode_t *[ tablesize ];
	  memset(heads, 0, sizeof(*heads) * tablesize);
  }

  addingMode = false;
	count = 0;
}

/*
================
xHashTable<KeyType, Type>::DeleteContents
================
*/
template< class KeyType, class Type >
X_INLINE void xHashTable<KeyType, Type>::DeleteContents() {
	int			i;
	hashnode_t	*node;
	hashnode_t	*next;

	for(i = 0; i < tablesize; i++) {
		next = heads[ i ];
    heads[ i ] = NULL; // allow access over delete process in destructor of value
		while(next != NULL) {
			node = next;
			next = next->next;
			delete node->value;
			delete node;
		}		
	}

	count = 0;
}

/*
================
xHashTable<KeyType, Type>::Num
================
*/
template< class KeyType, class Type >
X_INLINE int xHashTable<KeyType, Type>::Count() const {
	return count;
}

/*
================
xHashTable<KeyType, Type>::Spread
================
*/
template< class KeyType, class Type >
int xHashTable<KeyType, Type>::Spread() const {
	int i, average, error, e;
	hashnode_t	*node;

	// if no items in hash
	if (!count) {
		return 100;
	}
	average = count / tablesize;
	error = 0;
	for (i = 0; i < tablesize; i++) {
		numItems = 0;
		for(node = heads[ i ]; node != NULL; node = node->next) {
			numItems++;
		}
		e = abs(numItems - average);
		if (e > 1) {
			error += e - 1;
		}
	}
	return 100 - (error * 100 / count);
}

#endif /* __X_HASH_TABLE_H__ */
