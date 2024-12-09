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

#ifndef OpenRTI_ServerModel_FederationConnect_h
#define OpenRTI_ServerModel_FederationConnect_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

#include "Federate.h"
#include "Handle.h"
#include "ObjectInstanceConnect.h"

namespace OpenRTI {
namespace ServerModel {

class Federation;
class NodeConnect;

class OPENRTI_LOCAL FederationConnect :
    public IntrusiveList<FederationConnect, 0>::Hook,
    public IntrusiveUnorderedMap<ConnectHandle, FederationConnect>::Hook,
    public IntrusiveList<FederationConnect, 1>::Hook
{
public:
  FederationConnect(Federation& federation, NodeConnect& nodeConnect);
  ~FederationConnect();

  /// The Federation backward reference
  Federation const& getFederation() const
  { return _federation; }
  Federation& getFederation()
  { return _federation; }

  /// The NodeConnect backward reference
  NodeConnect const& getNodeConnect() const
  { return _nodeConnect; }
  NodeConnect& getNodeConnect()
  { return _nodeConnect; }

  /// Retrieve the handles to the referenced data structures
  FederationHandle const& getFederationHandle() const;

  /// The connect handle to identify this connect
  ConnectHandle const& getConnectHandle() const;

  /// True if this is the parent connect
  bool getIsParentConnect() const;

  bool getHasFederates() const;

  /// If we should really send messages there.
  /// Also an initial connect is not active to signal that we need to push the
  /// federations context into this connect. Once this is done, the connect is marked active.
  bool getActive() const
  { return _active; }
  void setActive(bool active);

  /// If the federates behind this connect are allowed to get time regulating
  bool getPermitTimeRegulation() const;
  void setPermitTimeRegulation(bool permitTimeRegulation);

  /// List of Federate instances belonging to this FederationConnect
  typedef IntrusiveList<Federate, 0> FederateList;
  /// Get the list of Federate instances
  FederateList const& getFederateList() const
  { return _federateList; }
  FederateList& getFederateList()
  { return _federateList; }
  void insert(Federate& federate)
  { OpenRTIAssert(!federate.getFederationConnect()); _federateList.push_back(federate); federate.setFederationConnect(this); }
  void erase(Federate& federate)
  {
    if (!federate.getFederationConnect())
      return;
    OpenRTIAssert(this == federate.getFederationConnect());
    _federateList.unlink(federate);
    federate.setFederationConnect(0);
  }

  /// List of Time Regulating Federate instances belonging to this FederationConnect
  typedef IntrusiveList<Federate, 1> TimeRegulatingFederateList;
  /// Get the list of Federate instances
  TimeRegulatingFederateList const& getTimeRegulatingFederateList() const
  { return _timeRegulatingFederateList; }
  TimeRegulatingFederateList& getTimeRegulatingFederateList()
  { return _timeRegulatingFederateList; }
  bool getIsTimeRegulating() const;
  void insertTimeRegulating(Federate& federate);
  void eraseTimeRegulating(Federate& federate);

  /// List of ObjectInstanceConnect instances belonging to this FederationConnect
  typedef IntrusiveList<ObjectInstanceConnect, 0> ObjectInstanceConnectList;
  /// Get the list of ObjectInstanceConnect instances
  ObjectInstanceConnectList const& getObjectInstanceConnectList() const
  { return _objectInstanceConnectList; }
  ObjectInstanceConnectList& getObjectInstanceConnectList()
  { return _objectInstanceConnectList; }
  void insert(ObjectInstanceConnect& objectInstanceConnect)
  { _objectInstanceConnectList.push_back(objectInstanceConnect); }

  /// We can actually send something there
  void send(const SharedPtr<const AbstractMessage>& message);

private:
#if 201103L <= __cplusplus
  FederationConnect(FederationConnect const&) = delete;
  FederationConnect(FederationConnect&&) = delete;
  FederationConnect& operator=(FederationConnect const&) = delete;
  FederationConnect& operator=(FederationConnect&&) = delete;
#else
  FederationConnect(FederationConnect const&);
  FederationConnect& operator=(FederationConnect const&);
#if 200610L <= __cpp_rvalue_reference
  FederationConnect(FederationConnect&&);
  FederationConnect& operator=(FederationConnect&&);
#endif
#endif

  /// The Federation backward reference
  Federation& _federation;

  /// The NodeConnect backward reference
  NodeConnect& _nodeConnect;

  bool _active;

  /// If the federates behind this connect are allowed to get time regulating
  bool _permitTimeRegulation;

  /// List of Federate instances belonging to this FederationConnect
  FederateList _federateList;

  /// List of Time Regulating Federate instances belonging to this FederationConnect
  TimeRegulatingFederateList _timeRegulatingFederateList;

  /// List of ObjectInstanceConnect instances belonging to this FederationConnect
  ObjectInstanceConnectList _objectInstanceConnectList;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_FederationConnect_h
