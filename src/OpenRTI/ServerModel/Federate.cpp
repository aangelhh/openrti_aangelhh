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

#include "Federate.h"

#include "FederationConnect.h"

namespace OpenRTI {
namespace ServerModel {

Federate::Federate(Federation& federation) :
  _federation(federation),
  _resignAction(CANCEL_THEN_DELETE_THEN_DIVEST),
  _resignPending(false),
  _federationConnect(0),
  _commitId(0)
{
}

Federate::~Federate()
{
  _regionHandleRegionMap.clear();
  _synchronizationFederateList.clear();
  _federationConnect = 0;

  OpenRTIAssert(_regionHandleRegionMap.empty());
  OpenRTIAssert(_synchronizationFederateList.empty());
}

void
Federate::setFederateHandle(FederateHandle const& federateHandle)
{
  IntrusiveUnorderedMap<FederateHandle, Federate>::Hook::setKey(federateHandle);
}

void
Federate::setName(std::string const& name)
{
  IntrusiveUnorderedMap<std::string, Federate>::Hook::setKey(name);
}

void
Federate::setFederateType(std::string const& federateType)
{
  _federateType = federateType;
}

void
Federate::setResignAction(ResignAction resignAction)
{
  _resignAction = resignAction;
}

void
Federate::setResignPending(bool resignPending)
{
  _resignPending = resignPending;
}

void
Federate::setFederationConnect(FederationConnect* federationConnect)
{
  OpenRTIAssert(federationConnect || !getIsTimeRegulating());
  _federationConnect = federationConnect;
}

ConnectHandle
Federate::getConnectHandle() const
{
  if (!_federationConnect)
    return ConnectHandle();
  return _federationConnect->getConnectHandle();
}

void
Federate::send(const SharedPtr<const AbstractMessage>& message)
{
  if (!_federationConnect)
    return;
  _federationConnect->send(message);
}

bool
Federate::getIsTimeRegulating() const
{
  // OpenRTIAssert(!FederationConnect::TimeRegulatingFederateList::Hook::is_linked() || _federationConnect->_permitTimeRegulation);
  return FederationConnect::TimeRegulatingFederateList::Hook::is_linked();
}

void
Federate::setTimeAdvanceTimeStamp(VariableLengthData const& timeAdvanceTimeStamp)
{
  _timeAdvanceTimeStamp = timeAdvanceTimeStamp;
}

void
Federate::setNextMessageTimeStamp(VariableLengthData const& nextMessageTimeStamp)
{
  _nextMessageTimeStamp = nextMessageTimeStamp;
}

void
Federate::setCommitId(Unsigned commitId)
{
  _commitId = commitId;
}

Region const*
Federate::getRegion(LocalRegionHandle const& regionHandle) const
{
  RegionHandleRegionMap::const_iterator i;
  i = _regionHandleRegionMap.find(regionHandle);
  if (i == _regionHandleRegionMap.end())
    return 0;
  return i.get();
}

Region*
Federate::getRegion(LocalRegionHandle const& regionHandle)
{
  RegionHandleRegionMap::iterator i;
  i = _regionHandleRegionMap.find(regionHandle);
  if (i == _regionHandleRegionMap.end())
    return 0;
  return i.get();
}

} // namespace ServerModel
} // namespace OpenRTI
