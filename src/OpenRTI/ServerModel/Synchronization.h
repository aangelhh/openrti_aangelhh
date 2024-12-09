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

#ifndef OpenRTI_ServerModel_Synchronization_h
#define OpenRTI_ServerModel_Synchronization_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

#include "Handle.h"
#include "SynchronizationFederate.h"
#include "VariableLengthData.h"

namespace OpenRTI {
namespace ServerModel {

class Federate;

class OPENRTI_LOCAL Synchronization :
    public IntrusiveUnorderedMap<std::string, Synchronization>::Hook
{
public:
  Synchronization();
  ~Synchronization();

  std::string const& getLabel() const
  { return IntrusiveUnorderedMap<std::string, Synchronization>::Hook::getKey(); }
  void setLabel(std::string const& label);

  VariableLengthData const& getTag() const
  { return _tag; }
  void setTag(VariableLengthData const& tag);

  bool getAddJoiningFederates() const
  { return _addJoiningFederates; }
  void setAddJoiningFederates(bool addJoiningFederates);

  bool getIsWaitingFor(FederateHandle const& federateHandle);

  void insert(Federate& federate);
  void achieved(FederateHandle const& federateHandle, bool successful);

  // private:

  /// The FederateHandle to SynchronizationFederate map of federates
  /// waiting for this synchronization point
  typedef IntrusiveUnorderedMap<FederateHandle, SynchronizationFederate> WaitingFederateSynchronizationMap;
  WaitingFederateSynchronizationMap _waitingFederateSynchronizationMap;
  /// The FederateHandle to SynchronizationFederate map of federates
  /// that have achieved the synchronization point
  typedef IntrusiveUnorderedMap<FederateHandle, SynchronizationFederate> AchievedFederateSynchronizationMap;
  AchievedFederateSynchronizationMap _achievedFederateSynchronizationMap;

private:
#if 201103L <= __cplusplus
  Synchronization(Synchronization const&) = delete;
  Synchronization(Synchronization&&) = delete;
  Synchronization& operator=(Synchronization const&) = delete;
  Synchronization& operator=(Synchronization&&) = delete;
#else
  Synchronization(Synchronization const&);
  Synchronization& operator=(Synchronization const&);
#if 200610L <= __cpp_rvalue_reference
  Synchronization(Synchronization&&);
  Synchronization& operator=(Synchronization&&);
#endif
#endif

  VariableLengthData _tag;

  bool _addJoiningFederates;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_Synchronization_h
