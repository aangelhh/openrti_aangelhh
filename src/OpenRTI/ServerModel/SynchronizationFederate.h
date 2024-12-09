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

#ifndef OpenRTI_ServerModel_SynchronizationFederate_h
#define OpenRTI_ServerModel_SynchronizationFederate_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

#include "Handle.h"

namespace OpenRTI {
namespace ServerModel {

class Federate;
class Synchronization;

class OPENRTI_LOCAL SynchronizationFederate :
    public IntrusiveUnorderedMap<FederateHandle, SynchronizationFederate>::Hook,
    public IntrusiveList<SynchronizationFederate, 0>::Hook
{
public:
  SynchronizationFederate(Synchronization& synchronization, Federate& federate);
  ~SynchronizationFederate();

  Synchronization const& getSynchronization() const
  { return _synchronization; }
  Synchronization& getSynchronization()
  { return _synchronization; }

  Federate const& getFederate() const
  { return _federate; }
  Federate& getFederate()
  { return _federate; }

  FederateHandle const& getFederateHandle() const
  { return IntrusiveUnorderedMap<FederateHandle, SynchronizationFederate>::Hook::getKey(); }
  void setFederateHandle(FederateHandle const& federateHandle);

  bool getSuccessful() const
  { return _successful; }
  void setSuccessful(bool successful);

private:
#if 201103L <= __cplusplus
  SynchronizationFederate(SynchronizationFederate const&) = delete;
  SynchronizationFederate(SynchronizationFederate&&) = delete;
  SynchronizationFederate& operator=(SynchronizationFederate const&) = delete;
  SynchronizationFederate& operator=(SynchronizationFederate&&) = delete;
#else
  SynchronizationFederate(SynchronizationFederate const&);
  SynchronizationFederate& operator=(SynchronizationFederate const&);
#if 200610L <= __cpp_rvalue_reference
  SynchronizationFederate(SynchronizationFederate&&);
  SynchronizationFederate& operator=(SynchronizationFederate&&);
#endif
#endif

  Synchronization& _synchronization;

  Federate& _federate;

  bool _successful;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_SynchronizationFederate_h
