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

#include "SynchronizationFederate.h"

#include "Federate.h"
#include "Synchronization.h"

namespace OpenRTI {
namespace ServerModel {

SynchronizationFederate::SynchronizationFederate(Synchronization& synchronization, Federate& federate) :
  _synchronization(synchronization),
  _federate(federate),
  _federateHandle(federate.getFederateHandle()),
  _successful(false)
{
  _synchronization._insertWaitingFederateSynchronizationMap(*this);
  _federate._insertSynchronizationFederateList(*this);
}

SynchronizationFederate::~SynchronizationFederate()
{
  _federate._unlinkSynchronizationFederateList(*this);
  Intrusive::UnorderedSetLink<SynchronizationFederate, Intrusive::ParentTag<Synchronization> >::unlink();
}

void
SynchronizationFederate::setSuccessful(bool successful)
{
  _successful = successful;
}

} // namespace ServerModel
} // namespace OpenRTI
