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

#ifndef OpenRTI_Intrusive_UnorderedSetKeyTraits_h
#define OpenRTI_Intrusive_UnorderedSetKeyTraits_h

#include <functional>

#include "OpenRTIConfig.h"
#include "Export.h"

namespace OpenRTI {

template<typename T>
struct OPENRTI_LOCAL Hash;

template<typename T>
struct OPENRTI_LOCAL Hash<const T> : Hash<T> {
};

#if 201103L <= __cplusplus
template<>
struct OPENRTI_LOCAL Hash<std::string> : std::hash<std::string> {
};
#else
template<>
struct OPENRTI_LOCAL Hash<std::string> {
  std::size_t operator()(const std::string& value) const
  {
    std::size_t hash = 0;
    for (std::string::const_iterator i = value.begin(), e = value.end(); i != e; ++i)
      hash = 5*hash + static_cast<std::size_t>(*i);
    return hash;
  }
};
#endif

template<typename T>
struct OPENRTI_LOCAL Hash<std::vector<T> > {
  std::size_t operator()(const std::vector<T>& value) const
  {
    std::size_t hash = 0;
    for (typename std::vector<T>::const_iterator i = value.begin(), e = value.end(); i != e; ++i)
      hash = 5*hash + Hash<T>()(*i);
    return hash;
  }
};

namespace Intrusive {

template<typename Key,
         typename Hasher = Hash<Key>,
         typename KeyEqual = std::equal_to<Key> >
struct UnorderedSetKeyTraits;

template<typename Key, typename Hasher, typename KeyEqual>
struct UnorderedSetKeyTraits {
  /// The type of the sort key
  typedef Key key_type;
  /// The hash function that is used
  typedef Hasher hasher;
  /// The equal function that is used
  typedef KeyEqual key_equal;
};

} // namespace Intrusive
} // namespace OpenRTI

#endif
