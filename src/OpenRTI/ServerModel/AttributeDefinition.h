/* -*-c++-*- OpenRTI - Copyright (C) 2009-2024 Mathias Froehlich
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

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

#include "Handle.h"
#include "Message.h"
#include "ServerModel.h"

namespace OpenRTI {
namespace ServerModel {

class ObjectClass;

class OPENRTI_LOCAL AttributeDefinition :
    public IntrusiveUnorderedMap<AttributeHandle, AttributeDefinition>::Hook,
    public IntrusiveUnorderedMap<std::string, AttributeDefinition>::Hook
{
public:
  typedef IntrusiveUnorderedMap<AttributeHandle, AttributeDefinition> HandleMap;
  typedef IntrusiveUnorderedMap<std::string, AttributeDefinition> NameMap;

  AttributeDefinition(ObjectClass& objectClass);
  ~AttributeDefinition();

  ObjectClass const& getObjectClass() const
  { return _objectClass; }
  ObjectClass& getObjectClass()
  { return _objectClass; }

  AttributeHandle const& getAttributeHandle() const
  { return IntrusiveUnorderedMap<AttributeHandle, AttributeDefinition>::Hook::getKey(); }
  void setAttributeHandle(AttributeHandle const& attributeHandle);

  std::string const& getName() const
  { return IntrusiveUnorderedMap<std::string, AttributeDefinition>::Hook::getKey(); }
  void setName(std::string const& name);

  OrderType getOrderType() const
  { return _orderType; }
  void setOrderType(OrderType orderType);

  TransportationType getTransportationType() const
  { return _transportationType; }
  void setTransportationType(TransportationType transportationType);

  /// List of ClassAttribute instances belonging to this AttributeDefinition
  typedef IntrusiveList<ClassAttribute, 0> ClassAttributeList;
  /// Get the list of ClassAttribute instances
  ClassAttributeList const& getClassAttributeList() const
  { return _classAttributeList; }
  ClassAttributeList& getClassAttributeList()
  { return _classAttributeList; }
  void insert(ClassAttribute& classAttribute)
  { _classAttributeList.push_back(classAttribute); }

  // FIXME temporarily in this way
  DimensionHandleSet _dimensionHandleSet;

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

  OrderType _orderType;

  TransportationType _transportationType;

  /// List of ClassAttribute instances belonging to this AttributeDefinition
  ClassAttributeList _classAttributeList;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_AttributeDefinition_h
