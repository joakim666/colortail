/*
    colortail -- output last part of file(s) in color.
    Copyright (C) 2009  Joakim Andersson <joakim@pjek.se>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _List_h_
#define _List_h_

// Based on list.h by Timothy A. Budd

#include <assert.h>
#include "Iterator.h"


template <class T> class Link;
template <class T> class ListIterator;

// class List
template <class T> class List
{
  protected:
   Link<T>* m_first;
   friend class ListIterator<T>;
   
  public:
   // constructors
   List();
   List(const List<T> &source);
   virtual ~List();

   // operations
   void add(T value);
   void add_last(T value);
   virtual void delete_all_values();
   List* duplicate() const;
   T first_element() const;
   virtual int includes(T value) const;
   int is_empty() const;
   virtual void remove_first();
};


// class Link
template <class T> class Link
{
  private:
   // constructor
   Link(T link_value, Link* next);
   Link(const Link&);

   // duplicate
   Link* duplicate() const;

   // data areas
   T m_value;
   Link* m_next;

   // friends
   friend class List<T>;
   friend class ListIterator<T>;

  public:
   // insert a new element following the current value
   Link<T>* insert(T val);
};


// class ListIterator
template <class T> class ListIterator : public Iterator<T>
{
  protected:
   // data areas
   Link<T>* m_current;
   Link<T>* m_previous;
   List<T>& m_the_list;

  public:
   // constructor
   ListIterator(List<T>& list);
   ListIterator(const ListIterator&);

   // iterator protocol
   virtual int init();
   virtual T operator ()();
   virtual int operator !();
   virtual int operator ++();
   virtual void operator =(T value);

   // new methods specific to list iterators
   void remove_current();
   void add_before(T new_value);
   void add_after(T new_value);
};

// class List implementation

template <class T> List<T>::List() : m_first(0)
{
   // no further initialization
}

template <class T> List<T>::~List()
{
   // empty all elements from the list
   delete_all_values();
}


template <class T> void List<T>::add(T value)
{
   // add a new value to the front of a linked list
   m_first = new Link<T>(value, m_first);
   assert(m_first != 0);
}

template <class T> void List<T>::add_last(T value)
{
   // add a new value to the read of a linked list

   // check if there isn't a first element
   if (m_first == 0)
   {
      // the list is empty
      // insert first in list
      m_first = new Link<T>(value, m_first);
      assert (m_first != 0);
   }
   else
   {
      // the list contains elements
      // find the last element
      Link<T> *tmp =  m_first;
      while (tmp->m_next)
      {
	 tmp = tmp->m_next;
      }

      // at last position in list, add the new element after tmp
      tmp->m_next = new Link<T>(value, 0);
      assert (tmp->m_next);
   }
}      

template <class T> void List<T>::delete_all_values()
{
   // clear all items from the list
   Link<T> *next;

   for (Link<T> *p = m_first ; p != 0 ; p = next)
   {
      // delete the element pointed to by p
      next = p->m_next;
      p->m_next = 0;
      delete p;
   }

   // mark that the list contains no elements
   m_first = 0;
}

template <class T> List<T>* List<T>::duplicate() const
{
   List<T> *newlist = new List<T>;
   assert (newlist != 0);

   // copy list
   if (m_first)
   {
      newlist->m_first = m_first->duplicate();
   }

   // return the new list
   return newlist;
}

template <class T> List<T>::List(const List<T> &source)
{
   // duplicate elements from source list
   if (source.is_empty())
   {
      m_first = 0;
   }
   else
   {
      Link<T> *first_link = source.m_first;
      m_first = first_link->duplicate();
   }
}

template <class T> T List<T>::first_element() const
{
   // return first value in list
   assert (m_first != 0);
   return m_first->m_value;
}

template <class T> int List<T>::includes(T v) const
{
   // loop to test each element
   for (Link<T> *p = m_first ; p ; p = p->m_next)
   {
      if (v == p->m_value)
      {
	 return 1;
      }
   }

   // not found
   return 0;
}

template <class T> int List<T>::is_empty() const
{
   // test to see if the list is empty
   // list is empty if the pointer to the first link is null
   return m_first == 0;
}

template <class T> void List<T>::remove_first()
{
   // make sure there is a first element
   assert (m_first != 0);

   // save pointer to the removed node
   Link<T> *p = m_first;

   // reassign the m_first node
   m_first = p->m_next;

   // recover memory used by the first element
   delete p;
}


// class Link implementation

template <class T> Link<T>* Link<T>::insert(T val)
{
   // insert a new link after the current node
   m_next = new Link<T>(val, m_next);

   // check that allocation was successful
   assert (m_next != 0);

   return m_next;
}

template <class T> Link<T>::Link(T val, Link<T> *nxt)
   : m_value(val), m_next(nxt)
{
   // create and initialize a new link field
}

template <class T> Link<T>::Link(const Link<T> &source)
   : m_value(source.m_value), m_next(source.m_next)
{
}

template <class T> Link<T>* Link<T>::duplicate() const
{
   Link<T> *newlink;

   // if there is a next field. copy remainder of list
   if (m_next != 0)
   {
      newlink = new Link<T>(m_value, m_next->duplicate());
   }
   else
   {
      newlink = new Link<T>(m_value, 0);
   }

   return newlink;
}


// class ListIterator implementation

template <class T> ListIterator<T>::ListIterator(List<T> &aList)
   : m_the_list(aList)
{
   // create and initialize a new list
   init();
}

template <class T> ListIterator<T>::ListIterator(const ListIterator<T> &x)
   : m_the_list(x.m_the_list)
{
   init();
}

template <class T> int ListIterator<T>::init()
{
   // set the iterator to the first element in the list
   m_previous = 0;
   m_current = m_the_list.m_first;

   return m_current != 0;
}

template <class T> T ListIterator<T>::operator ()()
{
   // return the value of current element

   // check to see if there is a current element
   assert (m_current != 0);

   // return value associated woth current element
   return m_current->m_value;
}

template <class T> int ListIterator<T>::operator !()
{
   // test for termination of the iterator
   // if current link references a removed value,
   // update current to point to next position
   if (m_current == 0)
   {
      if (m_previous != 0)
      {
	 m_current = m_previous->m_next;
      }
   }

   // now see if current link is valid
   return m_current != 0;
}

template <class T> int ListIterator<T>::operator ++()
{
   // move current pointer to next element
   // special processing if current link is deleted

   if (m_current == 0)
   {
      if (m_previous == 0)
      {
	 m_current = m_the_list.m_first;
      }
      else
      {
	 m_current = m_previous->m_next;
      }
   }
   else
   {
      // advance pointer
      m_previous = m_current;
      m_current = m_current->m_next;
   }

   // return true if we have a valid current element
   return m_current != 0;
}

template <class T> void ListIterator<T>::operator =(T val)
{
   // modify value of current element
   assert (m_current != 0);

   // modify value of the current link
   m_current->m_value = val;
}

template <class T> void ListIterator<T>::remove_current()
{
   // remove the current element from a list
   // make sure there is a current element
   assert (m_current != 0);

   // case 1, removing first element
   if (m_previous == 0)
   {
      m_the_list.m_first = m_current->m_next;
   }

   // case 2, not removing the first element
   else
   {
      m_previous->m_next = m_current->m_next;
   }

   // delete current node
   delete m_current;

   // and set current point to null
   m_current = 0;
}

template <class T> void ListIterator<T>::add_before(T val)
{
   // add a new element to list before current value
   // case 1, not at start
   if (m_previous)
   {
      m_previous = m_previous->insert(val);
   }

   // case 2, at start of list
   else
   {
      m_the_list.add(val);
      m_previous = m_the_list.m_first;
      m_current = m_previous->m_next;
   }
}

template <class T> void ListIterator<T>::add_after(T val)
{
   // add a new element to list after current value
   // case 1, not at start
   if (m_current != 0)
   {
      m_current->insert(val);
   }

   // case 2, at end of list
   else if (m_previous != 0)
   {
      m_current = m_previous->insert(val);
   }

   // case 3, start of list
   else
   {
      m_the_list.add(val);
   }
}

#endif





