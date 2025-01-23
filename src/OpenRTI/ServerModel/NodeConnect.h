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

#ifndef OpenRTI_ServerModel_NodeConnect_h
#define OpenRTI_ServerModel_NodeConnect_h

#include "Intrusive.h"

#include "AbstractMessageSender.h"
#include "Handle.h"
#include "StringUtils.h"

namespace OpenRTI {
namespace ServerModel {

class FederationConnect;
class Node;

class OPENRTI_LOCAL NodeConnect :
    public Intrusive::UnorderedSetLink<NodeConnect, Intrusive::ParentTag<Node> >
{
public:
  NodeConnect(Node& serverNode, ConnectHandle const& connectHandle);
  ~NodeConnect();

  /// The parent ServerNode
  Node const& getServerNode() const
  { return _serverNode; }
  Node& getServerNode()
  { return _serverNode; }

  /// The connect handle to identify this connect
  ConnectHandle const& getConnectHandle() const
  { return _connectHandle; }

  /// True if this is the parent connect
  bool getIsParentConnect() const
  { return _isParentConnect; }
  void setIsParentConnect(bool isParentConnect);

  /// The connects name - for debugging
  std::string const& getName() const
  { return _name; }
  void setName(std::string const& name);

  /// The string map from the other side of the connect
  StringStringListMap const& getOptions() const
  { return _options; }
  void setOptions(StringStringListMap const& options);

  /// List of FederationConnect instances belonging to this NodeConnect
  typedef Intrusive::List<Intrusive::ListLink<FederationConnect, Intrusive::ParentTag<NodeConnect> > > FederationConnectList;
  /// Get the list of FederationConnect instances
  FederationConnectList const& getFederationConnectList() const
  { return _federationConnectList; }
  FederationConnectList& getFederationConnectList()
  { return _federationConnectList; }
  void insert(FederationConnect& federationConnect);

  /// The message send callback
  const SharedPtr<AbstractMessageSender>& getMessageSender() const;
  void setMessageSender(const SharedPtr<AbstractMessageSender>& messageSender);

  /// We can actually send messages through a connect
  void send(const SharedPtr<const AbstractMessage>& message);

  template<typename Link>
  struct IntrusiveKey;

private:
#if 201103L <= __cplusplus
  NodeConnect(NodeConnect const&) = delete;
  NodeConnect(NodeConnect&&) = delete;
  NodeConnect& operator=(NodeConnect const&) = delete;
  NodeConnect& operator=(NodeConnect&&) = delete;
#else
  NodeConnect(NodeConnect const&);
  NodeConnect& operator=(NodeConnect const&);
#if 200610L <= __cpp_rvalue_reference
  NodeConnect(NodeConnect&&);
  NodeConnect& operator=(NodeConnect&&);
#endif
#endif

  /// The parent ServerNode
  Node& _serverNode;

  /// The connect handle to identify this connect
  ConnectHandle const _connectHandle;

  /// True if this is the parent connect
  bool _isParentConnect;

  /// The connects name - for debugging
  std::string _name;

  /// The string map from the other side of the connect
  StringStringListMap _options;

  /// List of FederationConnect instances belonging to this NodeConnect
  FederationConnectList _federationConnectList;

  SharedPtr<AbstractMessageSender> _messageSender;
};

template<>
struct NodeConnect::IntrusiveKey<Intrusive::UnorderedSetLink<NodeConnect, Intrusive::ParentTag<Node> > > {
  static ConnectHandle const& get(NodeConnect const& nodeConnect)
  { return nodeConnect.getConnectHandle(); }
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_NodeConnect_h
