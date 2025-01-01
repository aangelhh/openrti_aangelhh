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

#include "ObjectInstanceConnect.h"

#include "FederationConnect.h"

namespace OpenRTI {
namespace ServerModel {

ObjectInstanceConnect::ObjectInstanceConnect(ObjectInstance& objectInstance, FederationConnect& federationConnect) :
  IntrusiveUnorderedMap<ConnectHandle, ObjectInstanceConnect>::Hook(federationConnect.getConnectHandle()),
  _objectInstance(objectInstance),
  _federationConnect(federationConnect)
{
}

ObjectInstanceConnect::~ObjectInstanceConnect()
{
}

ConnectHandle const&
ObjectInstanceConnect::getConnectHandle() const
{
  // return getFederationConnect().getConnectHandle();
  return IntrusiveUnorderedMap<ConnectHandle, ObjectInstanceConnect>::Hook::getKey();
}

} // namespace ServerModel
} // namespace OpenRTI
