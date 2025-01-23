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

#ifndef OpenRTI_ServerModel_Federate_h
#define OpenRTI_ServerModel_Federate_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

#include "Handle.h"
#include "Message.h"
#include "VariableLengthData.h"

namespace OpenRTI {
namespace ServerModel {

class Federation;
class FederationConnect;
class Region;
class SynchronizationFederate;

class OPENRTI_LOCAL Federate :
    public Intrusive::UnorderedSetLink<Federate, Intrusive::ParentTag<Federation> >,
    public Intrusive::UnorderedSetLink<Federate, Intrusive::ParentTag<Federation, 1> >,
    public Intrusive::ListLink<Federate, Intrusive::ParentTag<FederationConnect> >,
    public Intrusive::ListLink<Federate, Intrusive::ParentTag<FederationConnect, 1> >
{
public:
  Federate(Federation& federation, FederateHandle const& federateHandle, std::string const& name);
  ~Federate();

  Federation const& getFederation() const
  { return _federation; }
  Federation& getFederation()
  { return _federation; }

  FederateHandle const& getFederateHandle() const
  { return _federateHandle; }

  std::string const& getName() const
  { return _name; }

  std::string const& getFederateType() const
  { return _federateType; }
  void setFederateType(std::string const& federateType);

  ResignAction getResignAction() const
  { return _resignAction; }
  void setResignAction(ResignAction resignAction);

  bool getResignPending() const
  { return _resignPending; }
  void setResignPending(bool resignPending);

  /// The connect where this Federate is sitting behind
  FederationConnect const* getFederationConnect() const
  { return _federationConnect; }
  FederationConnect* getFederationConnect()
  { return _federationConnect; }
  void setFederationConnect(FederationConnect* federationConnect);
  ConnectHandle getConnectHandle() const;

  void send(const SharedPtr<const AbstractMessage>& message);

  /// List of SynchronizationFederate instances belonging to this Federate
  typedef IntrusiveList<SynchronizationFederate, 0> SynchronizationFederateList;
  /// Get the list of SynchronizationFederate instances
  SynchronizationFederateList const& getSynchronizationFederateList() const
  { return _synchronizationFederateList; }
  SynchronizationFederateList& getSynchronizationFederateList()
  { return _synchronizationFederateList; }
  void insert(SynchronizationFederate& synchronizationFederate);

  bool getIsTimeRegulating() const;

  /// Time constrained federates current state
  VariableLengthData const& getTimeAdvanceTimeStamp() const
  { return _timeAdvanceTimeStamp; }
  void setTimeAdvanceTimeStamp(VariableLengthData const& timeAdvanceTimeStamp);

  VariableLengthData const& getNextMessageTimeStamp() const
  { return _nextMessageTimeStamp; }
  void setNextMessageTimeStamp(VariableLengthData const& nextMessageTimeStamp);

  Unsigned getCommitId() const
  { return _commitId; }
  void setCommitId(Unsigned commitId);

  /// UnorderedSet of Region instances indexed by regionHandle
  typedef IntrusiveUnorderedMap<LocalRegionHandle const, Region> RegionHandleRegionMap;
  /// Get the set of Region instances
  RegionHandleRegionMap const& getRegionHandleRegionMap() const
  { return _regionHandleRegionMap; }
  RegionHandleRegionMap& getRegionHandleRegionMap()
  { return _regionHandleRegionMap; }
  /// Get one Region instance matching regionHandle
  Region const* getRegion(LocalRegionHandle const& regionHandle) const;
  Region* getRegion(LocalRegionHandle const& regionHandle);
  void insert(Region& region);

  template<typename Link>
  struct IntrusiveKey;

private:
#if 201103L <= __cplusplus
  Federate(Federate const&) = delete;
  Federate(Federate&&) = delete;
  Federate& operator=(Federate const&) = delete;
  Federate& operator=(Federate&&) = delete;
#else
  Federate(Federate const&);
  Federate& operator=(Federate const&);
#if 200610L <= __cpp_rvalue_reference
  Federate(Federate&&);
  Federate& operator=(Federate&&);
#endif
#endif

  Federation& _federation;

  FederateHandle const _federateHandle;

  std::string const _name;

  std::string _federateType;

  ResignAction _resignAction;

  bool _resignPending;

  /// The connect where this Federate is sitting behind
  FederationConnect* _federationConnect;

  /// List of SynchronizationFederate instances belonging to this Federate
  SynchronizationFederateList _synchronizationFederateList;

  /// Time constrained federates current state
  VariableLengthData _timeAdvanceTimeStamp;

  VariableLengthData _nextMessageTimeStamp;

  Unsigned _commitId;

  /// UnorderedSet of Region instances indexed by regionHandle
  RegionHandleRegionMap _regionHandleRegionMap;
};

template<>
struct Federate::IntrusiveKey<Intrusive::UnorderedSetLink<Federate, Intrusive::ParentTag<Federation> > > {
  static FederateHandle const& get(Federate const& federate)
  { return federate.getFederateHandle(); }
};

template<>
struct Federate::IntrusiveKey<Intrusive::UnorderedSetLink<Federate, Intrusive::ParentTag<Federation, 1> > > {
  static std::string const& get(Federate const& federate)
  { return federate.getName(); }
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_Federate_h
