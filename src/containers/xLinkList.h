#ifndef __X_LINKLIST_H__
#define __X_LINKLIST_H__

#pragma once

/*
==============================================================================

Circular linked list template

==============================================================================
*/
template <class T>
class xLinkList
{
  xLinkList * head;
  xLinkList * next;
  xLinkList * prev;
  T * owner;

public:

  xLinkList * ListHead() const { return head; }
  xLinkList * NextNode() const { return next != head ? next : NULL; }
  xLinkList * PrevNode() const { return prev != head ? prev : NULL; }
  
  T * Owner() const { return owner; }
  void SetOwner(T * owner){ this->owner = owner; }

  T * Next() const { return next != head ? next->owner : NULL; }
  T * Prev() const { return prev != head ? prev->owner : NULL; }

  xLinkList()
  {
   head = this; 
   next = this;
   prev = this;
   this->owner = NULL;
  }
  ~xLinkList(){ Clear(); }

  bool IsListEmpty() const { return head->next == head; }
  bool InList() const { return head != this; }
  int  Count() const
  {
   int count = 0;
   for(xLinkList * node = head->next; node != head; node = node->next)
     count++;
    return count;
  }  
  void Clear()
  {
    if(head == this){
      while(next != this)
        next->Remove();
    }else
      Remove();
  }
  T * InsertBefore(xLinkList& node)
  {
    Remove();

    next = &node;
    prev = node.prev;
    node.prev = this;
    prev->next = this;
    head = node.head;

    return owner;
  }
  T * InsertAfter(xLinkList& node)
  {
    Remove();

    prev = &node;
    next = node.next;
    node.next = this;
    next->prev = this;
    head = node.head;

    return owner;
  }
  T * AddToEnd(xLinkList& node){ return InsertBefore(*node.head); }
  T * AddToFront(xLinkList& node){ return InsertAfter(*node.head); }

  T * Remove()
  {
    prev->next = next;
    next->prev = prev;

    next = this;
    prev = this;
    head = this;

    return owner;
  }
};

// =======================================================

template <class T>
class xSimpleList
{
public:

  T * list;

  xSimpleList(){ list = NULL; }
  ~xSimpleList(){ delete list; }

  static void DeleteNextItems(T * item)
  {
    for(T * next; item; item = next){
      next = item->next;
      item->next = NULL;
      delete item;
    }
  }

  void Clear(){ delete list; list = NULL; }

  int Count() const
  {
    int count = 0;
    for(T * cur = list; cur; count++)
      cur = cur->next;
    return count;
  }

  int Index(const T * item) const
  {
    const T * cur = list;
    for(int i = 0; cur; i++, cur = cur->next)
      if(cur == item)
        return i;
    return -1;
  }

  T * Item(int i) const
  {
    T * cur = list;
    for(; i > 0 && cur; i--)
      cur = cur->next;
    return cur;
  }
  T * LastItem() const
  {
    T * cur = list;
    if(cur){
      while(cur->next)
        cur = cur->next;
    }
    return cur;
  }
  T ** LastPos()
  {
    T ** pos = &list;
    while(*pos)
      pos = &(pos[0]->next);
    return pos;
  }
  static void AddToLastPos(T **& pos, T * item)
  {
    *pos = item;
    pos = &item->next;
  }  

  T * operator[](int i) const { return Item(i); }

  T * Remove(T * item)
  {
    T * cur = list, * prev = NULL;
    for(; cur; prev = cur, cur = cur->next)
      if(cur == item){
        if(prev)
          prev->next = cur->next;
        else
          list = cur->next;
        cur->next = NULL;
        return cur;
      }
    assert(0);
    return NULL;
  }

  T * RemoveIndex(int i)
  {
    T * cur = list, * prev = NULL;
    for(; i > 0 && cur; i--){
      prev = cur;
      cur = cur->next;
    }
    if(cur){
      if(prev)
        prev->next = cur->next;
      else
        list = cur->next;
      cur->next = NULL;
    }
    return cur;
  }

  T * RemoveFirst()
  {
    if(!list)
      return NULL;

    T * item = list;
    list = item->next;
    item->next = NULL;
    return item;
  }

  T * AddToFront(T * item)
  {
    item->next = list;
    list = item;
    return item;
  }

  T * AddToEnd(T * item)
  {
    item->next = NULL;

    if(!list){    
      list = item;
      return item;
    }
    T * cur = list;
    while(cur->next)
      cur = cur->next;

    cur->next = item;
    return item;
  }
  void AddToEnd(xSimpleList<T>& l)
  {
    if(!l.list)
      return;

    if(!list){
      list = l.list;
    }else{
      T * cur = list;
      while(cur->next)
        cur = cur->next;
      cur->next = l.list;
    }
    l.list = NULL;
  }
};

#endif
