/*
    colortail -- output last part of file(s) in color.
    Copyright (C) 2009  Joakim Ek <joakim@pjek.se>

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

#ifndef _Iterator_h_
#define _Iterator_h_

// Based on iterator.h by Timothy A. Budd

// class Iterator
// defines the protocol to be used by all iterators
// subclasses must implement each of the five iterator methods

template <class T> class Iterator
{
  public:
   // initialization
   virtual int init() = 0;

   // test if there is a current element
   virtual int operator !() = 0;

   // current element
   virtual T operator ()() = 0;

   // find next element
   virtual int operator ++() = 0;

   // change current element
   virtual void operator =(T newValue) = 0;
};

#endif
