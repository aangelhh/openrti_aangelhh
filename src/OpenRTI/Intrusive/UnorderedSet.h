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

#ifndef OpenRTI_Intrusive_UnorderedSet_h
#define OpenRTI_Intrusive_UnorderedSet_h

#include <cassert>
#include <vector>

#include "OpenRTIConfig.h"
#include "Export.h"

#include "DeleteMethod.h"
#include "KeyAccess.h"
#include "List.h"
#include "UnorderedSetKeyTraits.h"

namespace OpenRTI {
namespace Intrusive {

template<typename Key,
         typename Link,
         typename DeleteMethod = DeleteNoop,
         typename KeyAccess = DefaultKeyAccess<Link>, // Kind of placeholder class
         typename KeyTraits = UnorderedSetKeyTraits<Key, Hash<Key> > >
class UnorderedSet;


template<typename Key,
         typename Link,
         typename DeleteMethod,
         typename KeyAccess,
         typename KeyTraits>
class UnorderedSet {
public:
  typedef Key key_type;
  typedef Link link_type;
  typedef typename link_type::value_type value_type;
  typedef std::size_t size_type;

  typedef typename KeyTraits::hasher hasher;
  typedef typename KeyTraits::key_equal key_equal;

  typedef value_type &reference;
  typedef value_type const &const_reference;
  typedef value_type *pointer;
  typedef value_type const *const_pointer;

  typedef typename link_type::iterator iterator;
  typedef typename link_type::const_iterator const_iterator;
  typedef typename link_type::reverse_iterator reverse_iterator;
  typedef typename link_type::const_reverse_iterator const_reverse_iterator;

  UnorderedSet(size_type bucketCount = 128) :
    _implementation(bucketCount)
  { }
  UnorderedSet(UnorderedSet const &unorderedSet) :
    _implementation(unorderedSet._implementation._localListVector.size())
  { OpenRTIAssert(unorderedSet.empty()); }
#if 201103L <= __cplusplus || 200610L <= __cpp_rvalue_reference
  UnorderedSet(UnorderedSet &&unorderedSet)
  { swap(unorderedSet); }
#endif
  ~UnorderedSet()
  { DeleteMethod::destroy(*this); OpenRTIAssert(empty()); }

  UnorderedSet &operator=(UnorderedSet const &unorderedSet)
  { OpenRTIAssert(empty()); OpenRTIAssert(unorderedSet.empty()); _implementation._localListVector.resize(unorderedSet.bucket_count()); return *this; }
#if 201103L <= __cplusplus || 200610L <= __cpp_rvalue_reference
  UnorderedSet &operator=(UnorderedSet &&unorderedSet)
  { swap(unorderedSet); return *this; }
#endif

  /// returns true if the set is empty, is O(1).
  bool empty() const
  { return _chainList().empty(); }
  // Truely O(n)!!
  size_type size() const
  { return _chainList().size(); }
  /// returns true if the set contains at most one element
  bool single() const
  { return _chainList().single(); }
  /// returns true if the set contains exactly one element
  bool unique() const
  { return _chainList().unique(); }

  /// Returns the begin iterator, is O(1)
  const_iterator cbegin() const
  { return _chainList().cbegin(); }
  const_iterator begin() const
  { return _chainList().begin(); }
  iterator begin()
  { return _chainList().begin(); }

  /// Returns the end iterator, is O(1)
  const_iterator cend() const
  { return _chainList().cend(); }
  const_iterator end() const
  { return _chainList().end(); }
  iterator end()
  { return _chainList().end(); }

  /// Returns the reverse begin iterator, is O(1)
  const_reverse_iterator crbegin() const
  { return _chainList().crbegin(); }
  const_reverse_iterator rbegin() const
  { return _chainList().rbegin(); }
  reverse_iterator rbegin()
  { return _chainList().rbegin(); }

  /// Returns the reverse end iterator, is O(1)
  const_reverse_iterator crend() const
  { return _chainList().crend(); }
  const_reverse_iterator rend() const
  { return _chainList().rend(); }
  reverse_iterator rend()
  { return _chainList().rend(); }

  /// Returns the first item, is O(1)
  const_reference front() const
  { return _chainList().front(); }
  reference front()
  { return _chainList().front(); }

  /// Returns the last item, is O(1)
  const_reference back() const
  { return _chainList().back(); }
  reference back()
  { return _chainList().back(); }

  /// Insert an item into the unordered set, is O(1)
  iterator insert(pointer value)
  { OpenRTIAssert(value != NULL); return insert(*value); }
  iterator insert(reference value)
  { return _insert(value, bucket(value)); }

  /// Remove from the set and delete the item
  iterator erase(iterator i)
  { OpenRTIAssert(!empty()); erase(*i++); return i; }
  reverse_iterator erase(reverse_iterator i)
  { OpenRTIAssert(!empty()); erase(*i++); return i; }
  static void erase(pointer value)
  { OpenRTIAssert(value != NULL); delete value; }
  static void erase(reference value)
  { erase(&value); }

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

  /// Unlinks the item from the set. Does not delete the item.
  iterator unlink(iterator i)
  { OpenRTIAssert(!empty()); unlink(*i++); return i; }
  reverse_iterator unlink(reverse_iterator i)
  { OpenRTIAssert(!empty()); unlink(*i++); return i; }
  static void unlink(pointer value)
  { OpenRTIAssert(value != NULL); unlink(*value); }
  static void unlink(reference value)
  { OpenRTIAssert(is_linked(value)); Methods::unlink(value); }

  void unlink()
  {
    reverse_iterator i = rbegin();
    while (i != rend())
      i = unlink(i);
  }

  /// Unlinks the first or last item, is O(1)
  void unlink_front()
  { OpenRTIAssert(!empty()); unlink(*begin()); }
  void unlink_back()
  { OpenRTIAssert(!empty()); unlink(*rbegin()); }

  /// Finds the first item with key, is O(N) in the worst case, typically O(1)
  const_iterator find(key_type const &key) const
  { return _lower_bound(key, bucket(key)); }
  iterator find(key_type const &key)
  { return _lower_bound(key, bucket(key)); }

  /// Returns a reference to an item with key, is O(N) in the worst case, typically O(1)
  const_reference operator[](key_type const &key) const
  { return *find(key); }
  reference operator[](key_type const &key)
  { return *find(key); }

  /// Returns something like std::make_pair(lower_bound(key), upper_bound(key))
  std::pair<const_iterator, const_iterator> equal_range(key_type const &key) const
  { return _equal_range(key, bucket(key)); }
  std::pair<iterator, iterator> equal_range(key_type const &key)
  { return _equal_range(key, bucket(key)); }

  /// Returns if at least one element matching key is in the set, is O(N) in the worst case, typically O(1)
  bool contains(key_type const &key) const
  { return find(key) != end(); }
  /// Returns the number of elements matching key in the set, is O(N) in the worst case, typically O(count)
  size_type count(key_type const &key) const
  { return _count(key, bucket(key)); }

  /// Returns the hash for key
  size_type hash(key_type const &key) const
  { return hash_function()(key); }
  /// Returns the hash for value
  size_type hash(const_reference value) const
  { typedef typename _KeyProxy<KeyAccess>::type key_access_type; return hash(key_access_type::get(value)); }

  /// Returns the bucket index for key
  size_type bucket(key_type const &key) const
  { return hash(key) % bucket_count(); }
  /// Returns the bucket index for value
  size_type bucket(const_reference value) const
  { return hash(value) % bucket_count(); }

  size_type bucket_count() const
  { return _implementation._localListVector.size(); }
  size_type bucket_size(size_type bucket_index) const
  { return _localList(bucket_index).size(); }
  /// Reserves space for at least the specified number of elements.
  /// This regenerates the hash table.
  void rehash(size_type count)
  {
    for (typename _ChainList::iterator i = _chainList().begin(); i != _chainList().end(); ++i)
      _LocalList::unlink(*i);
    _implementation._localListVector.resize(count);
    for (typename _ChainList::iterator i = _chainList().begin(); i != _chainList().end(); ++i) {
      // Insert into the bucket. Since the total list is already
      // grouped by key, we do not need to care about that again.
      _localList(bucket(*i)).push_back(*i);
    }
  }

  /// Swaps the two instances, is O(1)
  void swap(UnorderedSet &unorderedSet)
  {
    std::swap(hash_function(), unorderedSet.hash_function());
    std::swap(key_eq(), unorderedSet.key_eq());
    _chainList().swap(unorderedSet._chainList());
    _implementation._localListVector.swap(unorderedSet._implementation._localListVector);
  }

  /// Convert a value to an iterator, is O(1)
  static iterator it(reference t)
  { OpenRTIAssert(is_linked(t)); return _ChainList::it(t); }
  static const_iterator it(const_reference t)
  { OpenRTIAssert(is_linked(t)); return _ChainList::it(t); }
  static reverse_iterator rit(reference t)
  { OpenRTIAssert(is_linked(t)); return _ChainList::rit(t); }
  static const_reverse_iterator rit(const_reference t)
  { OpenRTIAssert(is_linked(t)); return _ChainList::rit(t); }

  /// Returns if the value is linked.
  static bool is_linked(const_reference t)
  { return Methods::is_linked(t); }

  hasher const &hash_function() const
  { return static_cast<hasher const&>(_implementation); }
  hasher &hash_function()
  { return static_cast<hasher&>(_implementation); }

  key_equal const &key_eq() const
  { return static_cast<key_equal const&>(_implementation); }
  key_equal &key_eq()
  { return static_cast<key_equal&>(_implementation); }

private:
  typedef List<typename link_type::chain_link_type> _ChainList;
  typedef List<typename link_type::local_link_type> _LocalList;
  typedef std::vector<_LocalList> _LocalListVector;

  _ChainList const &_chainList() const
  { return _implementation._chainList; }
  _ChainList &_chainList()
  { return _implementation._chainList; }

  _LocalList const &_localList(size_type n) const
  { return _implementation._localListVector[n]; }
  _LocalList &_localList(size_type n)
  { return _implementation._localListVector[n]; }

  iterator _insert(reference value, std::size_t bucket_index)
  {
    if (bucket_count() <= bucket_index)
      rehash(1 + bucket_index);

    _LocalList &localList = _localList(bucket_index);
    // First insert into the bucket.
    // Group entries with the same key.
    typename _LocalList::reverse_iterator i = localList.rbegin();
    while (i != localList.rend()) {
      if (_equal(value, *i))
        break;
      ++i;
    }
    localList.insert(i, value);

    // And put them into the global list, also grouped by key.
    typename _ChainList::reverse_iterator j;
    if (i != localList.rend())
      j = _ChainList::rit(*i);
    else
      j = _chainList().rbegin();
    _chainList().insert(j, value);
    return _ChainList::it(value);
  }

  const_iterator _lower_bound(key_type const &key, std::size_t bucket_index) const
  {
    if (bucket_count() <= bucket_index)
      return end();
    _LocalList const &localList = _localList(bucket_index);
    for (typename _LocalList::const_iterator i = localList.begin(), e = localList.end();
         i != e; ++i) {
      if (_equal(key, *i))
        return _ChainList::it(*i);
    }
    return end();
  }
  iterator _lower_bound(key_type const &key, std::size_t bucket_index)
  {
    if (bucket_count() <= bucket_index)
      return end();
    _LocalList &localList = _localList(bucket_index);
    for (typename _LocalList::iterator i = localList.begin(), e = localList.end();
         i != e; ++i) {
      if (_equal(key, *i))
        return _ChainList::it(*i);
    }
    return end();
  }
  const_iterator _upper_bound(key_type const &key, std::size_t bucket_index) const
  {
    if (bucket_count() <= bucket_index)
      return end();
    _LocalList const &localList = _localList(bucket_index);
    for (typename _LocalList::const_reverse_iterator i = localList.rbegin(), e = localList.rend();
         i != e; ++i) {
      if (_equal(key, *i))
        return ++_ChainList::it(*i);
    }
    return end();
  }
  iterator _upper_bound(key_type const &key, std::size_t bucket_index)
  {
    if (bucket_count() <= bucket_index)
      return end();
    _LocalList &localList = _localList(bucket_index);
    for (typename _LocalList::reverse_iterator i = localList.rbegin(), e = localList.rend();
         i != e; ++i) {
      if (_equal(key, *i))
        return ++_ChainList::it(*i);
    }
    return end();
  }
  std::pair<const_iterator, const_iterator> _equal_range(key_type const &key, size_type bucket_index) const
  { return std::pair<const_iterator, const_iterator>(_lower_bound(key, bucket_index), _upper_bound(key, bucket_index)); }
  std::pair<iterator, iterator> _equal_range(key_type const &key, size_type bucket_index)
  { return std::pair<iterator, iterator>(_lower_bound(key, bucket_index), _upper_bound(key, bucket_index)); }
  size_type _count(key_type const &key, size_type bucket_index) const
  { return std::distance(_lower_bound(key, bucket_index), _upper_bound(key, bucket_index)); }

  bool _equal(key_type const &left, key_type const &right) const
  { return key_eq()(left, right); }
  bool _equal(key_type const &key, const_reference t) const
  { typedef typename _KeyProxy<KeyAccess>::type key_access_type; return _equal(key, key_access_type::get(t)); }
  bool _equal(const_reference left, const_reference right) const
  { typedef typename _KeyProxy<KeyAccess>::type key_access_type; return _equal(key_access_type::get(left), right); }

  typedef typename link_type::Methods Methods;

  template<typename K>
  struct _KeyProxy {
    typedef K type;
  };
  template<typename L>
  struct _KeyProxy<DefaultKeyAccess<L> > {
    typedef typename value_type::template IntrusiveKey<Link> type;
  };

  struct _Implementation : hasher, key_equal {
    _Implementation(size_type bucketCount) : _localListVector(bucketCount) {}

    _ChainList _chainList;
    _LocalListVector _localListVector;
  };

  _Implementation _implementation;
};

} // namespace Intrusive
} // namespace OpenRTI

#endif
