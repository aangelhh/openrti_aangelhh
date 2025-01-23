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

#include "Synchronization.h"

#include "Federate.h"
#include "Federation.h"
#include "SynchronizationFederate.h"

namespace OpenRTI {
namespace ServerModel {

Synchronization::Synchronization(Federation& federation, std::string const& label) :
  _federation(federation),
  _label(label),
  _addJoiningFederates(true)
{
  _federation._insertSynchronizationNameSynchronizationMap(*this);
}

Synchronization::~Synchronization()
{
  _achievedFederateSynchronizationMap.clear();
  _waitingFederateSynchronizationMap.clear();

  _federation._unlinkSynchronizationNameSynchronizationMap(*this);

  OpenRTIAssert(_achievedFederateSynchronizationMap.empty());
  OpenRTIAssert(_waitingFederateSynchronizationMap.empty());
}

void
Synchronization::setTag(VariableLengthData const& tag)
{
  _tag = tag;
}

void
Synchronization::setAddJoiningFederates(bool addJoiningFederates)
{
  _addJoiningFederates = addJoiningFederates;
}

bool
Synchronization::getIsWaitingFor(FederateHandle const& federateHandle)
{
  return _waitingFederateSynchronizationMap.find(federateHandle) != _waitingFederateSynchronizationMap.end();
}

void
Synchronization::addFederate(Federate& federate)
{
  OpenRTIAssert(_waitingFederateSynchronizationMap.find(federate.getFederateHandle()) == _waitingFederateSynchronizationMap.end());
  OpenRTIAssert(_achievedFederateSynchronizationMap.find(federate.getFederateHandle()) == _achievedFederateSynchronizationMap.end());
  if (federate.getResignPending())
    return;
  SynchronizationFederate* synchronizationFederate = new SynchronizationFederate(*this, federate);
}

void
Synchronization::achieved(FederateHandle const& federateHandle, bool successful)
{
  WaitingFederateSynchronizationMap::iterator i;
  i = _waitingFederateSynchronizationMap.find(federateHandle);
  if (i == _waitingFederateSynchronizationMap.end())
    return;
  i->setSuccessful(successful);
  // OpenRTIAssert(_achievedFederateSynchronizationMap.find(i->getFederateHandle()) == _achievedFederateSynchronizationMap.end());
  // Note that no matter where we are currently linked,
  // this removes the entry from one of the maps
  _unlinkWaitingFederateSynchronizationMap(*i);
  _insertAchievedFederateSynchronizationMap(*i);
}

void
Synchronization::_insertWaitingFederateSynchronizationMap(SynchronizationFederate& synchronizationFederate)
{
  _waitingFederateSynchronizationMap.insert(synchronizationFederate);
}

void
Synchronization::_unlinkWaitingFederateSynchronizationMap(SynchronizationFederate& synchronizationFederate)
{
  _waitingFederateSynchronizationMap.unlink(synchronizationFederate);
}

void
Synchronization::_insertAchievedFederateSynchronizationMap(SynchronizationFederate& synchronizationFederate)
{
  _achievedFederateSynchronizationMap.insert(synchronizationFederate);
}

void
Synchronization::_unlinkAchievedFederateSynchronizationMap(SynchronizationFederate& synchronizationFederate)
{
  _achievedFederateSynchronizationMap.unlink(synchronizationFederate);
}

} // namespace ServerModel
} // namespace OpenRTI
