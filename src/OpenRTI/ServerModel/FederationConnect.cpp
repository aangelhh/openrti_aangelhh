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
  _nodeConnect._insertFederationConnectList(*this);
  _federation._insertConnectHandleFederationConnectMap(*this);
}

FederationConnect::~FederationConnect()
{
  /// FIXME
  _objectInstanceConnectList.clear();
  for (TimeRegulatingFederateList::iterator i = _timeRegulatingFederateList.begin(), e = _timeRegulatingFederateList.end(); i != e;)
    (i++)->setIsTimeRegulating(false);
  OpenRTIAssert(_timeRegulatingFederateList.empty());
  for (FederateList::iterator i = _federateList.begin(), e = _federateList.end(); i != e;)
    (i++)->setFederationConnect(0);
  OpenRTIAssert(_federateList.empty());

  if (getIsTimeRegulating())
    _federation._unlinkTimeRegulatingFederationConnectList(*this);
  _federation._unlinkConnectHandleFederationConnectMap(*this);
  _nodeConnect._unlinkFederationConnectList(*this);

  OpenRTIAssert(_objectInstanceConnectList.empty());
}

bool
FederationConnect::getIsTimeRegulating() const
{
  return Intrusive::ListLink<FederationConnect, Intrusive::ParentTag<Federation> >::is_linked();
}

void
FederationConnect::setIsTimeRegulating(bool isTimeRegulating)
{
  if (isTimeRegulating == getIsTimeRegulating())
    return;
  if (isTimeRegulating) {
    _federation._insertTimeRegulatingFederationConnectList(*this);
  } else {
    _federation._unlinkTimeRegulatingFederationConnectList(*this);
  }
}

bool
FederationConnect::getIsParentConnect() const
{
  return getNodeConnect().getIsParentConnect();
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
FederationConnect::send(const SharedPtr<const AbstractMessage>& message)
{
  if (!_active)
    return;
  _nodeConnect.send(message);
}

void
FederationConnect::_insertFederateList(Federate& federate)
{
  _federateList.push_back(federate);
}

void
FederationConnect::_unlinkFederateList(Federate& federate)
{
  _federateList.unlink(federate);
}

void
FederationConnect::_insertTimeRegulatingFederateList(Federate& federate)
{
  setIsTimeRegulating(true);
  _timeRegulatingFederateList.push_back(federate);
}

void
FederationConnect::_unlinkTimeRegulatingFederateList(Federate& federate)
{
  _timeRegulatingFederateList.unlink(federate);
  setIsTimeRegulating(!_timeRegulatingFederateList.empty());
}

void
FederationConnect::_insertObjectInstanceConnectList(ObjectInstanceConnect& objectInstanceConnect)
{
  _objectInstanceConnectList.push_back(objectInstanceConnect);
}

void
FederationConnect::_unlinkObjectInstanceConnectList(ObjectInstanceConnect& objectInstanceConnect)
{
  _objectInstanceConnectList.unlink(objectInstanceConnect);
}

} // namespace ServerModel
} // namespace OpenRTI
