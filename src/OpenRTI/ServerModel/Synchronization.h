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

#ifndef OpenRTI_ServerModel_Synchronization_h
#define OpenRTI_ServerModel_Synchronization_h

#include "OpenRTIConfig.h"

#include "Intrusive.h"

#include "Handle.h"
#include "VariableLengthData.h"

namespace OpenRTI {
namespace ServerModel {

class Federate;
class Federation;
class SynchronizationFederate;

class OPENRTI_LOCAL Synchronization :
    public Intrusive::UnorderedSetLink<Synchronization, Intrusive::ParentTag<Federation> >
{
public:
  Synchronization(Federation& federation, std::string const& label);
  ~Synchronization();

  Federation const& getFederation() const
  { return _federation; }
  Federation& getFederation()
  { return _federation; }

  std::string const& getLabel() const
  { return _label; }

  VariableLengthData const& getTag() const
  { return _tag; }
  void setTag(VariableLengthData const& tag);

  bool getAddJoiningFederates() const
  { return _addJoiningFederates; }
  void setAddJoiningFederates(bool addJoiningFederates);

  bool getIsWaitingFor(FederateHandle const& federateHandle);

  void addFederate(Federate& federate);
  void achieved(FederateHandle const& federateHandle, bool successful);

  /// The FederateHandle to SynchronizationFederate map of federates
  /// UnorderedSet of SynchronizationFederate instances indexed by federateHandle
  /// waiting for this synchronization point

  /// UnorderedSet of SynchronizationFederate instances indexed by federateHandle
  typedef Intrusive::UnorderedSet<FederateHandle, Intrusive::UnorderedSetLink<SynchronizationFederate, Intrusive::ParentTag<Synchronization> > > WaitingFederateSynchronizationMap;
  /// Get the set of SynchronizationFederate instances
  WaitingFederateSynchronizationMap const& getWaitingFederateSynchronizationMap() const
  { return _waitingFederateSynchronizationMap; }
  WaitingFederateSynchronizationMap& getWaitingFederateSynchronizationMap()
  { return _waitingFederateSynchronizationMap; }

  /// The FederateHandle to SynchronizationFederate map of federates
  /// that have achieved the synchronization point

  /// UnorderedSet of SynchronizationFederate instances indexed by federateHandle
  typedef Intrusive::UnorderedSet<FederateHandle, Intrusive::UnorderedSetLink<SynchronizationFederate, Intrusive::ParentTag<Synchronization> > > AchievedFederateSynchronizationMap;
  /// Get the set of SynchronizationFederate instances
  AchievedFederateSynchronizationMap const& getAchievedFederateSynchronizationMap() const
  { return _achievedFederateSynchronizationMap; }
  AchievedFederateSynchronizationMap& getAchievedFederateSynchronizationMap()
  { return _achievedFederateSynchronizationMap; }

  template<typename Link>
  struct IntrusiveKey;

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

  Federation& _federation;

  std::string const _label;

  VariableLengthData _tag;

  bool _addJoiningFederates;

  /// UnorderedSet of SynchronizationFederate instances indexed by federateHandle
  friend class SynchronizationFederate;
  /// Insert synchronizationFederate into waitingFederateSynchronizationMap
  void _insertWaitingFederateSynchronizationMap(SynchronizationFederate& synchronizationFederate);
  /// Unlink synchronizationFederate from waitingFederateSynchronizationMap
  void _unlinkWaitingFederateSynchronizationMap(SynchronizationFederate& synchronizationFederate);
  WaitingFederateSynchronizationMap _waitingFederateSynchronizationMap;

  /// UnorderedSet of SynchronizationFederate instances indexed by federateHandle
  /// Insert synchronizationFederate into achievedFederateSynchronizationMap
  void _insertAchievedFederateSynchronizationMap(SynchronizationFederate& synchronizationFederate);
  /// Unlink synchronizationFederate from achievedFederateSynchronizationMap
  void _unlinkAchievedFederateSynchronizationMap(SynchronizationFederate& synchronizationFederate);
  AchievedFederateSynchronizationMap _achievedFederateSynchronizationMap;
};

template<>
struct Synchronization::IntrusiveKey<Intrusive::UnorderedSetLink<Synchronization, Intrusive::ParentTag<Federation> > > {
  static std::string const& get(Synchronization const& synchronization)
  { return synchronization.getLabel(); }
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_Synchronization_h
