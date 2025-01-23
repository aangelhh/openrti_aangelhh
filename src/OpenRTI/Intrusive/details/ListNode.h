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

#ifndef OpenRTI_Intrusive_details_ListNode_h
#define OpenRTI_Intrusive_details_ListNode_h

#include <algorithm>
#include <cstddef>

#include "Enums.h"

#include "Exception.h"

namespace OpenRTI {
namespace Intrusive {
namespace details {

struct ListNode {

protected:
  ListNode()
  { Methods::link(*this, SideLeft, *this); }

  /// When deriving, don't call them
#if 201103L <= __cplusplus
  ListNode(ListNode const &) = delete;
  ListNode(ListNode&&) = delete;
#endif

  ~ListNode()
  { OpenRTIAssert(!is_linked()); }

  /// When deriving, don't call them
#if 201103L <= __cplusplus
  ListNode& operator=(ListNode const &) = delete;
  ListNode& operator=(ListNode &&) = delete;
#endif

  bool is_linked(void) const
  { return Methods::is_linked(*this); }
  void unlink(void)
  { Methods::unlink(*this); }

  // Basically the vtbl of this
  struct Methods {
    /// True if empty.
    static bool empty(ListNode const &node)
    { return !is_linked(node); }
    /// True if linked.
    static bool is_linked(ListNode const &node)
    { OpenRTIAssert(node._is_self_linked(SideLeft) == node._is_self_linked(SideRight)); return !node._is_self_linked(SideLeft); }
    /// The number of entries in the list, is O(n)!!
    static std::size_t size(ListNode const &node)
    {
      std::size_t size = 0;
      for (ListNode const* i = next(node, SideRight); i != &node; i = next(*i, SideRight))
        ++size;
      return size;
    }
    /// returns true if the list contains at most one element
    static bool single(ListNode const &list)
    { return next(list, SideLeft) == next(list, SideRight); }
    /// returns true if the list contains exactly one element
    static bool unique(ListNode const &list)
    { return is_linked(list) && single(list); }

    /// Iteration methods
    static ListNode const* next(ListNode const &node, Side side)
    { return node._nodes[side]; }
    static ListNode* next(ListNode &node, Side side)
    { return node._nodes[side]; }

    /// List manipulation methods

    /// link node0 to side of node1
    static void link(ListNode &node0, Side side, ListNode &node1)
    {
      Side other = opposite_side(side);
      node1._nodes[side] = &node0;
      node0._nodes[other] = &node1;
    }
    // unlink node from a linked list
    static void unlink(ListNode &node)
    {
      link(*next(node, SideLeft), SideLeft, *next(node, SideRight));
      link(node, SideLeft, node);
    }
    // insert node1 at side of node0
    static void insert(ListNode &node0, Side side, ListNode &node1)
    {
      // node1 must not be linked as we are not closing its open ends
      // else we have potentially dangling pointers to node1
      OpenRTIAssert(!is_linked(node1));
      link(*next(node0, side), side, node1);
      link(node0, opposite_side(side), node1);
    }
    // transfer [first, last) before position
    static void transfer(ListNode &position, ListNode &first, ListNode &last)
    { _transfer(*next(position, SideLeft), position, *next(first, SideLeft), first, *next(last, SideLeft), last); }

    /// Swap the list as such
    static void swap(ListNode &node0, ListNode &node1)
    {
      _swap(*next(node0, SideLeft), *next(node0, SideRight),
            *next(node1, SideLeft), *next(node1, SideRight));
      // Keep the above swaps above the below swaps, that swaps
      // back the links for the case of empty sentinels.
      _swap(node0, node0, node1, node1);
    }

  private:
    static void _transfer(ListNode &position0, ListNode &position1, ListNode &first0, ListNode &first1,
                          ListNode &last0, ListNode &last1)
    {
      std::swap(first1._nodes[SideLeft], position1._nodes[SideLeft]);
      std::swap(position0._nodes[SideRight], last0._nodes[SideRight]);

      std::swap(position1._nodes[SideLeft], last1._nodes[SideLeft]);
      std::swap(first0._nodes[SideRight], position0._nodes[SideRight]);
    }

    static void _swap(ListNode &prev0, ListNode &next0, ListNode &prev1, ListNode &next1)
    {
      std::swap(next0._nodes[SideLeft], next1._nodes[SideLeft]);
      std::swap(prev0._nodes[SideRight], prev1._nodes[SideRight]);
    }
  };

private:
  bool _is_self_linked(Side side) const
  { return _nodes[side] == this; }

  ListNode *_nodes[2];
};

} // namespace details
} // namespace Intrusive
} // namespace OpenRTI

#endif
