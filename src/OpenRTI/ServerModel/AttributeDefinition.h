/* -*-c++-*- OpenRTI - Copyright (C) 2009-2025 Mathias Froehlich
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

#ifndef OpenRTI_ServerModel_AttributeDefinition_h
#define OpenRTI_ServerModel_AttributeDefinition_h

#include "OpenRTIConfig.h"

#include "Intrusive.h"

#include "Handle.h"
#include "Message.h"

namespace OpenRTI {
namespace ServerModel {

class ClassAttribute;
class ObjectClass;

class OPENRTI_LOCAL AttributeDefinition :
    public Intrusive::UnorderedSetLink<AttributeDefinition, Intrusive::ParentTag<ObjectClass> >,
    public Intrusive::UnorderedSetLink<AttributeDefinition, Intrusive::ParentTag<ObjectClass, 1> >
{
public:
  AttributeDefinition(ObjectClass& objectClass, AttributeHandle const& attributeHandle, std::string const& name);
  ~AttributeDefinition();

  ObjectClass const& getObjectClass() const
  { return _objectClass; }
  ObjectClass& getObjectClass()
  { return _objectClass; }

  AttributeHandle const& getAttributeHandle() const
  { return _attributeHandle; }

  std::string const& getName() const
  { return _name; }

  OrderType getOrderType() const
  { return _orderType; }
  void setOrderType(OrderType orderType);

  TransportationType getTransportationType() const
  { return _transportationType; }
  void setTransportationType(TransportationType transportationType);

  /// List of ClassAttribute instances belonging to this AttributeDefinition
  typedef Intrusive::List<Intrusive::ListLink<ClassAttribute, Intrusive::ParentTag<AttributeDefinition> > > ClassAttributeList;
  /// Get the list of ClassAttribute instances
  ClassAttributeList const& getClassAttributeList() const
  { return _classAttributeList; }
  ClassAttributeList& getClassAttributeList()
  { return _classAttributeList; }

  // FIXME temporarily in this way
  DimensionHandleSet _dimensionHandleSet;

  template<typename Link>
  struct IntrusiveKey;

private:
#if 201103L <= __cplusplus
  AttributeDefinition(AttributeDefinition const&) = delete;
  AttributeDefinition(AttributeDefinition&&) = delete;
  AttributeDefinition& operator=(AttributeDefinition const&) = delete;
  AttributeDefinition& operator=(AttributeDefinition&&) = delete;
#else
  AttributeDefinition(AttributeDefinition const&);
  AttributeDefinition& operator=(AttributeDefinition const&);
#if 200610L <= __cpp_rvalue_reference
  AttributeDefinition(AttributeDefinition&&);
  AttributeDefinition& operator=(AttributeDefinition&&);
#endif
#endif

  ObjectClass& _objectClass;

  AttributeHandle const _attributeHandle;

  std::string const _name;

  OrderType _orderType;

  TransportationType _transportationType;

  /// List of ClassAttribute instances belonging to this AttributeDefinition
  friend class ClassAttribute;
  /// Insert classAttribute into classAttributeList
  void _insertClassAttributeList(ClassAttribute& classAttribute);
  /// Unlink classAttribute from classAttributeList
  void _unlinkClassAttributeList(ClassAttribute& classAttribute);
  ClassAttributeList _classAttributeList;
};

template<>
struct AttributeDefinition::IntrusiveKey<Intrusive::UnorderedSetLink<AttributeDefinition, Intrusive::ParentTag<ObjectClass> > > {
  static AttributeHandle const& get(AttributeDefinition const& attributeDefinition)
  { return attributeDefinition.getAttributeHandle(); }
};

template<>
struct AttributeDefinition::IntrusiveKey<Intrusive::UnorderedSetLink<AttributeDefinition, Intrusive::ParentTag<ObjectClass, 1> > > {
  static std::string const& get(AttributeDefinition const& attributeDefinition)
  { return attributeDefinition.getName(); }
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_AttributeDefinition_h
