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

#ifndef OpenRTI_Intrusive_UnorderedSetLink_h
#define OpenRTI_Intrusive_UnorderedSetLink_h

#include <cassert>

#include "OpenRTIConfig.h"
#include "Export.h"

#include "ListLink.h"

namespace OpenRTI {
namespace Intrusive {

template<typename T, typename Tag>
class UnorderedSetLink;

template<typename T, typename Tag>
class UnorderedSetLink :
    public ListLink<T, ChainTag<Tag> >,
    public ListLink<T, LocalTag<Tag> >
{
public:
  typedef T value_type;
  typedef Tag tag_type;

  UnorderedSetLink()
  { }
  UnorderedSetLink(UnorderedSetLink const &)
  { }
#if 201103L <= __cplusplus || 200610L <= __cpp_rvalue_reference
  UnorderedSetLink(UnorderedSetLink &&)
  { }
#endif
  ~UnorderedSetLink()
  { Methods::unlink(*this); }

  UnorderedSetLink &operator=(UnorderedSetLink const&)
  { return *this; }
#if 201103L <= __cplusplus || 200610L <= __cpp_rvalue_reference
  UnorderedSetLink &operator=(UnorderedSetLink &&)
  { return *this; }
#endif

  /// Those two should be public accessible
  bool is_linked() const
  { return Methods::is_linked(*this); }
  void unlink()
  { Methods::unlink(*this); }

  typedef ListLink<T, ChainTag<Tag> > chain_link_type;
  typedef ListLink<T, LocalTag<Tag> > local_link_type;

  typedef typename chain_link_type::iterator iterator;
  typedef typename chain_link_type::const_iterator const_iterator;
  typedef typename chain_link_type::reverse_iterator reverse_iterator;
  typedef typename chain_link_type::const_reverse_iterator const_reverse_iterator;

  /// Conversion functions between link and value.
  struct Access {
    static value_type const *to_pointer(UnorderedSetLink const *link)
    { return static_cast<value_type const*>(link); }
    static value_type *to_pointer(UnorderedSetLink *link)
    { return static_cast<value_type*>(link); }
    static value_type const &to_reference(UnorderedSetLink const &link)
    { return static_cast<value_type const&>(link); }
    static value_type &to_reference(UnorderedSetLink &link)
    { return static_cast<value_type&>(link); }
  };

  /// Methods on the link
  struct Methods {
    /// True if linked.
    static bool is_linked(UnorderedSetLink const &link)
    {
      OpenRTIAssert((chain_link_type::Methods::is_linked(link) == local_link_type::Methods::is_linked(link)));
      return chain_link_type::Methods::is_linked(link);
    }
    /// Unlink needs to go through here, since we want to unlink both or none.
    /// This holds except for internal purpose.
    static void unlink(UnorderedSetLink &link)
    { chain_link_type::Methods::unlink(link); local_link_type::Methods::unlink(link); }
  };
};

} // namespace Intrusive
} // namespace OpenRTI

#endif
