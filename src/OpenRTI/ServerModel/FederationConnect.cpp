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

#include "FederationConnect.h"

#include "Federate.h"
#include "Federation.h"
#include "NodeConnect.h"
#include "ObjectInstanceConnect.h"

namespace OpenRTI {
namespace ServerModel {

FederationConnect::FederationConnect(Federation& federation, NodeConnect& nodeConnect) :
  _federation(federation),
  _nodeConnect(nodeConnect),
  _federationHandle(federation.getFederationHandle()),
  _connectHandle(nodeConnect.getConnectHandle()),
  _active(false),
  _permitTimeRegulation(true)
{
}

FederationConnect::~FederationConnect()
{
  /// FIXME
  _objectInstanceConnectList.clear();
  _timeRegulatingFederateList.unlink();

  OpenRTIAssert(_objectInstanceConnectList.empty());
  OpenRTIAssert(_timeRegulatingFederateList.empty());
}

bool
FederationConnect::getIsParentConnect() const
{
  return getNodeConnect().getIsParentConnect();
}

bool
FederationConnect::getHasFederates() const
{
  return !_federateList.empty();
}

void
FederationConnect::setActive(bool active)
{
  _active = active;
}

bool
FederationConnect::getPermitTimeRegulation() const
{
  if (_permitTimeRegulation)
    return true;
  return getIsParentConnect();
}

void
FederationConnect::setPermitTimeRegulation(bool permitTimeRegulation)
{
  _permitTimeRegulation = permitTimeRegulation;
}

void
FederationConnect::insert(Federate& federate)
{
  OpenRTIAssert(!federate.getFederationConnect());
  _federateList.push_back(federate);
  federate.setFederationConnect(this);
}

void
FederationConnect::erase(Federate& federate)
{
  if (!federate.getFederationConnect())
    return;
  OpenRTIAssert(this == federate.getFederationConnect());
  _federateList.unlink(federate);
  federate.setFederationConnect(0);
}

bool
FederationConnect::getIsTimeRegulating() const
{
  OpenRTIAssert(_timeRegulatingFederateList.empty() != Federation::TimeRegulatingFederationConnectList::link_type::is_linked());
  OpenRTIAssert(!Federation::TimeRegulatingFederationConnectList::link_type::is_linked() || _permitTimeRegulation);
  return Federation::TimeRegulatingFederationConnectList::link_type::is_linked();
}

void
FederationConnect::insertTimeRegulating(Federate& federate)
{
  _timeRegulatingFederateList.push_back(federate);
}

void
FederationConnect::eraseTimeRegulating(Federate& federate)
{
  _timeRegulatingFederateList.unlink(federate);
}

void
FederationConnect::send(const SharedPtr<const AbstractMessage>& message)
{
  if (!_active)
    return;
  _nodeConnect.send(message);
}

void
FederationConnect::insert(ObjectInstanceConnect& objectInstanceConnect)
{
  _objectInstanceConnectList.push_back(objectInstanceConnect);
}

} // namespace ServerModel
} // namespace OpenRTI
