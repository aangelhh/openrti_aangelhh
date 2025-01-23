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

#include "AttributeDefinition.h"

#include "ClassAttribute.h"
#include "ObjectClass.h"

namespace OpenRTI {
namespace ServerModel {

AttributeDefinition::AttributeDefinition(ObjectClass& objectClass, AttributeHandle const& attributeHandle, std::string const& name) :
  _objectClass(objectClass),
  _attributeHandle(attributeHandle),
  _name(name),
  _orderType(RECEIVE),
  _transportationType(RELIABLE)
{
  _objectClass._insertAttributeHandleAttributeDefinitionMap(*this);
  _objectClass._insertAttributeNameAttributeDefinitionMap(*this);
}

AttributeDefinition::~AttributeDefinition()
{
  _classAttributeList.clear();

  _objectClass._unlinkAttributeNameAttributeDefinitionMap(*this);
  _objectClass._unlinkAttributeHandleAttributeDefinitionMap(*this);

  OpenRTIAssert(_classAttributeList.empty());
}

void
AttributeDefinition::setOrderType(OrderType orderType)
{
  _orderType = orderType;
}

void
AttributeDefinition::setTransportationType(TransportationType transportationType)
{
  _transportationType = transportationType;
}

void
AttributeDefinition::insert(ClassAttribute& classAttribute)
{
  _classAttributeList.push_back(classAttribute);
}

} // namespace ServerModel
} // namespace OpenRTI
