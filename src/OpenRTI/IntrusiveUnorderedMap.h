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

#ifndef OpenRTI_IntrusiveUnroderedMap_h
#define OpenRTI_IntrusiveUnroderedMap_h

#include <string>
#include <vector>

#include "Exception.h"
#include "IntrusiveList.h"

#include "Intrusive.h"

namespace OpenRTI {

template<typename, unsigned>
struct IntrusiveUnorderedSetTag;

template<typename Key, typename T, unsigned tag>
class UnorderedSetLinkWithKey : public Intrusive::UnorderedSetLink<T, IntrusiveUnorderedSetTag<Key, tag> > {
public:
  UnorderedSetLinkWithKey() :
    _key()
  { }
  UnorderedSetLinkWithKey(const Key& key) :
    _key(key)
  { }

  bool is_linked() const
  { return Intrusive::UnorderedSetLink<T, IntrusiveUnorderedSetTag<Key, tag> >::is_linked(); }

  const Key& getKey() const
  { return _key; }

  /// For the UnorderedSet implementation to get the key value.
  struct KeyAccess {
    static Key const& get(T const& value)
    { return static_cast<UnorderedSetLinkWithKey const&>(value).getKey(); }
  };

protected:
  void setKey(const Key& key)
  { OpenRTIAssert(!is_linked()); _key = key; }

private:
  Key _key;
};

template<typename Key, typename T, typename _Hash = Hash<Key>, unsigned tag = 0>
class OPENRTI_LOCAL IntrusiveUnorderedMap :
    public Intrusive::UnorderedSet<Key,
                                   Intrusive::UnorderedSetLink<T, IntrusiveUnorderedSetTag<Key, tag> >,
                                   Intrusive::DeleteNoop,
                                   typename UnorderedSetLinkWithKey<Key, T, tag>::KeyAccess,
                                   Intrusive::UnorderedSetKeyTraits<Key, _Hash>
                                   > {
  typedef Intrusive::UnorderedSet<Key,
                                  Intrusive::UnorderedSetLink<T, IntrusiveUnorderedSetTag<Key, tag> >,
                                  Intrusive::DeleteNoop,
                                  typename UnorderedSetLinkWithKey<Key, T, tag>::KeyAccess,
                                  Intrusive::UnorderedSetKeyTraits<Key, _Hash>
                                  > _UnorderedSet;
public:
  IntrusiveUnorderedMap(size_t bucketCount = 128) :
    _UnorderedSet(bucketCount)
  { }
  IntrusiveUnorderedMap(IntrusiveUnorderedMap const &unorderedSet) :
    _UnorderedSet(unorderedSet)
  { }

  typedef UnorderedSetLinkWithKey<Key, T, tag> Hook;
};

} // namespace OpenRTI

#endif
