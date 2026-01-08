/* -*-c++-*- OpenRTI - Copyright (C) 2013-2026 Mathias Froehlich
 *
 * This file is part of OpenRTI.
 *
 * OpenRTI is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * OpenRTI is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OpenRTI.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef OpenRTI_Intrusive_List_h
#define OpenRTI_Intrusive_List_h

#include <cassert>

#include "OpenRTIConfig.h"
#include "Export.h"

#include "DeleteMethod.h"
#include "ListLink.h"

namespace OpenRTI {
namespace Intrusive {

template<typename Link,
         typename DeleteMethod = DeleteNoop>
class List;

/// Final list class.
/// Is used to shield implementation details away from the api.
/// References the building blocks below.
template<typename Link, typename DeleteMethod>
class List {
public:
  typedef Link link_type;
  typedef typename link_type::value_type value_type;
  typedef std::size_t size_type;
  typedef value_type &reference;
  typedef value_type const &const_reference;
  typedef value_type *pointer;
  typedef value_type const *const_pointer;

  typedef typename link_type::iterator iterator;
  typedef typename link_type::const_iterator const_iterator;
  typedef typename link_type::reverse_iterator reverse_iterator;
  typedef typename link_type::const_reverse_iterator const_reverse_iterator;

  List()
  { }
  List(List const &list)
  { OpenRTIAssert(list.empty()); }
#if 201103L <= __cplusplus || 200610L <= __cpp_rvalue_reference
  List(List &&list)
  { swap(list); }
#endif
  ~List()
  { DeleteMethod::destroy(*this); OpenRTIAssert(empty()); }

  List &operator=(List const &list)
  { OpenRTIAssert(empty()); OpenRTIAssert(list.empty()); return *this; }
#if 201103L <= __cplusplus || 200610L <= __cpp_rvalue_reference
  List &operator=(List &&list)
  { swap(list); return *this; }
#endif

  /// returns true if the list is empty, is O(1).
  bool empty() const
  { return Methods::empty(_sentinel); }
  /// The number of entries in the set, is O(n).
  size_type size() const
  { return Methods::size(_sentinel); }

  /// returns true if the list contains at most one element, is O(1)
  bool single() const
  { return Methods::single(_sentinel); }
  /// returns true if the list contains exactly one element, is O(1)
  bool unique() const
  { return Methods::unique(_sentinel); }

  /// Returns the begin iterator, is O(1)
  const_iterator cbegin() const
  { return ++cend(); }
  const_iterator begin() const
  { return ++end(); }
  iterator begin()
  { return ++end(); }

  /// Returns the end iterator, is O(1)
  const_iterator cend() const
  { return const_iterator(_sentinel); }
  const_iterator end() const
  { return cend(); }
  iterator end()
  { return iterator(_sentinel); }

  /// Returns the reverse begin iterator, is O(1)
  const_reverse_iterator crbegin() const
  { return ++crend(); }
  const_reverse_iterator rbegin() const
  { return ++rend(); }
  reverse_iterator rbegin()
  { return ++rend(); }

  /// Returns the reverse end iterator, is O(1)
  const_reverse_iterator crend() const
  { return const_reverse_iterator(_sentinel); }
  const_reverse_iterator rend() const
  { return crend(); }
  reverse_iterator rend()
  { return reverse_iterator(_sentinel); }

  /// Access to the first item in the list, is O(1).
  const_reference front() const
  { OpenRTIAssert(!empty()); return *begin(); }
  reference front()
  { OpenRTIAssert(!empty()); return *begin(); }

  /// Access to the last item in the list, is O(1).
  const_reference back() const
  { OpenRTIAssert(!empty()); return *rbegin(); }
  reference back()
  { OpenRTIAssert(!empty()); return *rbegin(); }

  /// Insert an item into the list, is O(1)
  iterator insert(iterator const&i, pointer value)
  { OpenRTIAssert(value != NULL); return insert(i, *value); }
  iterator insert(iterator const&i, reference value)
  { Methods::insert(*i, details::SideLeft, value); return iterator(value); }
  reverse_iterator insert(reverse_iterator const&i, pointer value)
  { OpenRTIAssert(value != NULL); return insert(i, *value); }
  reverse_iterator insert(reverse_iterator const&i, reference value)
  { Methods::insert(*i, details::SideRight, value); return reverse_iterator(value); }

  /// Insert an item into the list before begin(), is O(1)
  void push_front(pointer t)
  { push_front(*t); }
  void push_front(reference t)
  { insert(begin(), t); }
  /// Insert an item into the list before end(), is O(1)
  void push_back(pointer t)
  { push_back(*t); }
  void push_back(reference t)
  { insert(end(), t); }

  /// Remove from the list and delete the item, is O(1)
  iterator erase(iterator i)
  { OpenRTIAssert(!empty()); erase(*i++); return i; }
  reverse_iterator erase(reverse_iterator i)
  { OpenRTIAssert(!empty()); erase(*i++); return i; }
  static void erase(pointer value)
  { OpenRTIAssert(value != NULL); delete value; }
  static void erase(reference value)
  { erase(&value); }

  /// Remove and delete all entries, is O(n).
  void clear()
  {
    reverse_iterator i = rbegin();
    while (i != rend())
      i = erase(i);
  }

  /// Removes the front/back item of this list, is O(1).
  /// Deletes the item.
  void pop_front()
  { OpenRTIAssert(!empty()); erase(*begin()); }
  void pop_back()
  { OpenRTIAssert(!empty()); erase(*rbegin()); }

  /// Unlinks the front/back item of this list, is O(1).
  /// Does not delete the item.
  iterator unlink(iterator i)
  { OpenRTIAssert(!empty()); unlink(*i++); return i; }
  reverse_iterator unlink(reverse_iterator i)
  { OpenRTIAssert(!empty()); unlink(*i++); return i; }
  static void unlink(pointer value)
  { OpenRTIAssert(value != NULL); unlink(*value); }
  static void unlink(reference value)
  { OpenRTIAssert(is_linked(value)); Methods::unlink(value); }

  /// Unlinks all entries, is O(n).
  void unlink()
  {
    reverse_iterator i = rbegin();
    while (i != rend())
      i = unlink(i);
  }

  /// Unlinks the front/back item of this list, is O(1).
  /// Does not delete the item.
  void unlink_front()
  { OpenRTIAssert(!empty()); unlink(*begin()); }
  void unlink_back()
  { OpenRTIAssert(!empty()); unlink(*rbegin()); }

  /// The usual family of splice methods
  /// Splice other into this before position, behaves undefined if other is this
  static void splice(iterator pos, List& other)
  { splice(pos, other.begin(), other.end()); }
  /// Splice first before position, behaves undefined if pos and [first, first + 1] overlaps
  static void splice(iterator pos, iterator first)
  { iterator next = first; std::advance(next, 1); splice(pos, first, next); }
  /// Splice [first, last) before position, behaves undefined if pos and [first, last] overlaps
  static void splice(iterator pos, iterator first, iterator last)
  { Methods::transfer(*pos, *first, *last); }

  /// Swaps the two instances, is O(1)
  void swap(List &list)
  { Methods::swap(_sentinel, list._sentinel); }

  /// Convert a value to an iterator, is O(1)
  static iterator it(reference t)
  { OpenRTIAssert(is_linked(t)); return iterator(t); }
  static const_iterator it(const_reference t)
  { OpenRTIAssert(is_linked(t)); return const_iterator(t); }
  static reverse_iterator rit(reference t)
  { OpenRTIAssert(is_linked(t)); return reverse_iterator(t); }
  static const_reverse_iterator rit(const_reference t)
  { OpenRTIAssert(is_linked(t)); return const_reverse_iterator(t); }

  /// Returns if the value is linked.
  static bool is_linked(const_reference value)
  { return Methods::is_linked(value); }

private:
  typedef typename link_type::Methods Methods;

  /// The lists sentinel.
  Link _sentinel;
};

} // namespace Intrusive
} // namespace OpenRTI

#endif
