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

#ifndef OpenRTI_Intrusive_ListLink_h
#define OpenRTI_Intrusive_ListLink_h

#include <cassert>

#include "OpenRTIConfig.h"
#include "Export.h"

#include "details/Iterator.h"
#include "details/ListNode.h"
#include "Tag.h"

namespace OpenRTI {
namespace Intrusive {

template<typename T, typename Tag>
class ListLink;

template<typename T, typename Tag>
class ListLink :
    private details::ListNode
{
public:
  typedef T value_type;
  typedef Tag tag_type;

  ListLink()
  { }
  ListLink(ListLink const&)
  { }
#if 201103L <= __cplusplus || 200610L <= __cpp_rvalue_reference
  ListLink(ListLink&&)
  { }
#endif
  ~ListLink()
  { Methods::unlink(*this); }

  ListLink &operator=(ListLink const&)
  { return *this; }
#if 201103L <= __cplusplus || 200610L <= __cpp_rvalue_reference
  ListLink &operator=(ListLink&&)
  { return *this; }
#endif

  bool is_linked(void) const
  { return Methods::is_linked(*this); }
  void unlink()
  { Methods::unlink(*this); }

  /// Conversion functions between link and value.
  struct Access {
    static value_type const *to_pointer(ListLink const *link)
    { return static_cast<value_type const*>(link); }
    static value_type *to_pointer(ListLink *link)
    { return static_cast<value_type*>(link); }
    static value_type const &to_reference(ListLink const &link)
    { return static_cast<value_type const&>(link); }
    static value_type &to_reference(ListLink &link)
    { return static_cast<value_type&>(link); }
  };

  typedef details::Iterator<value_type, ListLink, Access, details::SideRight, details::SideLeft> iterator;
  typedef details::Iterator<value_type const, ListLink const, Access, details::SideRight, details::SideLeft> const_iterator;
  typedef details::Iterator<value_type, ListLink, Access, details::SideLeft, details::SideRight> reverse_iterator;
  typedef details::Iterator<value_type const, ListLink const, Access, details::SideLeft, details::SideRight> const_reverse_iterator;

  /// Methods on the link
  struct Methods {
    /// True if linked.
    static bool is_linked(ListLink const &listLink)
    { return ListNode::Methods::is_linked(listLink); }
    /// True if empty.
    static bool empty(ListLink const &listLink)
    { return !is_linked(listLink); }
    /// The number of entries in the list, is O(n)!!
    static std::size_t size(ListLink const &listLink)
    { return ListNode::Methods::size(listLink); }
    /// returns true if the list contains at most one element
    static bool single(ListLink const &listLink)
    { return ListNode::Methods::single(listLink); }
    /// returns true if the list contains exactly one element
    static bool unique(ListLink const &listLink)
    { return ListNode::Methods::unique(listLink); }

    static ListLink const *next(ListLink const &link, details::Side side)
    { return static_cast<ListLink const*>(ListNode::Methods::next(link, side)); }
    static ListLink *next(ListLink &link, details::Side side)
    { return static_cast<ListLink*>(ListNode::Methods::next(link, side)); }

    static void unlink(ListLink &link)
    { ListNode::Methods::unlink(link); }
    static void insert(ListLink &link0, details::Side side, ListLink &link1)
    { ListNode::Methods::insert(link0, side, link1); }
    static void transfer(ListLink &position, ListLink &first, ListLink &last)
    { ListNode::Methods::transfer(position, first, last); }

    static void swap(ListLink &link, ListLink &list)
    { ListNode::Methods::swap(link, list); }
  };
};

} // namespace Intrusive
} // namespace OpenRTI

#endif
