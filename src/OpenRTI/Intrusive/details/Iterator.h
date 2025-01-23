/* -*-c++-*- OpenRTI - Copyright (C) 2013-2025 Mathias Froehlich
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

#ifndef OpenRTI_Intrusive_details_Iterator_h
#define OpenRTI_Intrusive_details_Iterator_h

#include <iterator>
#include "Enums.h"
#include "Exception.h"

namespace OpenRTI {
namespace Intrusive {
namespace details {

/// Default bidirectional iterator for our containers.
/// The two components to factor out are the cast operations
/// from the embedded container nodes to the final datatype
/// using the Access template and the increment and decrement
/// operations using the Advance template parameter.
template<typename T, typename Link, typename Access, Side Forward, Side Backward>
class Iterator {
public:
  typedef std::bidirectional_iterator_tag iterator_category;
  typedef T value_type;
  typedef value_type *pointer;
  typedef value_type &reference;
  typedef std::ptrdiff_t difference_type;

  Iterator() :
    _link(NULL)
  { }
  Iterator(Link &link) :
    _link(&link)
  { }
  template<typename _T, typename _Link>
  Iterator(Iterator<_T, _Link, Access, Forward, Backward> const &iterator) :
    _link(iterator._link)
  { }

  template<typename _T, typename _Link>
  Iterator& operator=(Iterator<_T, _Link, Access, Forward, Backward> const &iterator)
  { _link = iterator._link; return *this; }

  Iterator& operator++()
  { OpenRTIAssert(_link); _link = Link::Methods::next(*_link, Forward); return *this; }
  Iterator operator++(int)
  { Iterator iterator(*this); operator++(); return iterator; }
  Iterator& operator--()
  { OpenRTIAssert(_link); _link = Link::Methods::next(*_link, Backward); return *this; }
  Iterator operator--(int)
  { Iterator iterator(*this); operator--(); return iterator; }

  template<typename _T, typename _Link>
  bool operator==(Iterator<_T, _Link, Access, Forward, Backward> const &iterator) const
  { OpenRTIAssert(_link && iterator._link); return _link == iterator._link; }
  template<typename _T, typename _Link>
  bool operator!=(Iterator<_T, _Link, Access, Forward, Backward> const &iterator) const
  { OpenRTIAssert(_link && iterator._link); return _link != iterator._link; }

  reference operator*() const
  { return Access::to_reference(*_link); }
  pointer operator->() const
  { return Access::to_pointer(_link); }

  pointer get() const
  { return Access::to_pointer(_link); }

  // convenience methods not required by the bidirectional iterator
  template<typename D>
  Iterator& operator+=(D const &d)
  { std::advance(*this, d); return *this; }
  template<typename D>
  Iterator& operator-=(D const &d)
  { std::advance(*this, -static_cast<std::ptrdiff_t>(d)); return *this; }
  template<typename D>
  Iterator operator+(D const &d) const
  { return Iterator(*this) += d; }
  template<typename D>
  Iterator operator-(D const &d) const
  { return Iterator(*this) -= d; }

private:
  template<typename, typename, typename, Side, Side>
  friend class Iterator;

  Link *_link;
};

} // namespace details
} // namespace Intrusive
} // namespace OpenRTI

#endif
