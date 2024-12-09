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

#include "AttributeDefinition.h"

namespace OpenRTI {
namespace ServerModel {

AttributeDefinition::AttributeDefinition(ObjectClass& objectClass, AttributeHandle const& attributeHandle, std::string const& name) :
  IntrusiveUnorderedMap<AttributeHandle const, AttributeDefinition>::Hook(attributeHandle),
  IntrusiveUnorderedMap<std::string const, AttributeDefinition>::Hook(name),
  _objectClass(objectClass),
  _orderType(RECEIVE),
  _transportationType(RELIABLE)
{
}

AttributeDefinition::~AttributeDefinition()
{
  _classAttributeList.clear();

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

} // namespace ServerModel
} // namespace OpenRTI
