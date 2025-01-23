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

#ifndef OpenRTI_ServerModel_Node_h
#define OpenRTI_ServerModel_Node_h

#include "OpenRTIConfig.h"

#include "Intrusive.h"

#include "AbstractMessageSender.h"
#include "Handle.h"
#include "HandleAllocator.h"
#include "ServerOptions.h"

namespace OpenRTI {
namespace ServerModel {

class Federation;
class NodeConnect;

class OPENRTI_LOCAL Node
{
public:
  Node();
  virtual ~Node();

  /// The ConnectHandle of the parent Connect
  ConnectHandle const& getParentConnectHandle() const
  { return _parentConnectHandle; }

  /// Returns true if the provided connectHandle is the parent connect handle
  bool isParentConnect(ConnectHandle const& connectHandle) const
  { OpenRTIAssert(connectHandle.valid()); return _parentConnectHandle == connectHandle; }

  /// Returns true if the server node is a root server
  bool isRootServer() const;

  /// Returns true if the server node has child connects
  bool hasChildConnects() const;

  /// Returns true if the server node is running idle
  bool isIdle() const
  { return !isRootServer() && !hasChildConnects(); }

  /// The ServerOptions for this server component
  ServerOptions const& getServerOptions() const
  { return *_serverOptions; }
  ServerOptions& getServerOptions()
  { return *_serverOptions; }

  /// The name of the Server as provided in the ServerOptions
  std::string const& getServerName() const
  { return _serverOptions->getServerName(); }

  /// The path of the Server as provided in the ServerOptions
  std::string const& getServerPath() const
  { return _serverOptions->getServerPath(); }

  /// UnorderedSet of NodeConnect instances indexed by connectHandle
  typedef Intrusive::UnorderedSet<ConnectHandle, Intrusive::UnorderedSetLink<NodeConnect, Intrusive::ParentTag<Node> > > ConnectHandleNodeConnectMap;
  /// Get the set of NodeConnect instances
  ConnectHandleNodeConnectMap const& getConnectHandleNodeConnectMap() const
  { return _connectHandleNodeConnectMap; }
  ConnectHandleNodeConnectMap& getConnectHandleNodeConnectMap()
  { return _connectHandleNodeConnectMap; }
  /// Get one NodeConnect instance matching connectHandle
  NodeConnect const* getNodeConnect(ConnectHandle const& connectHandle) const;
  NodeConnect* getNodeConnect(ConnectHandle const& connectHandle);
  NodeConnect* insertNodeConnect(const SharedPtr<AbstractMessageSender>& messageSender, StringStringListMap const& options);
  NodeConnect* insertParentNodeConnect(const SharedPtr<AbstractMessageSender>& messageSender, StringStringListMap const& options);
  void erase(ConnectHandle const& connectHandle);
  void erase(NodeConnect& nodeConnect);

  /// UnorderedSet of Federation instances indexed by federationHandle
  typedef Intrusive::UnorderedSet<FederationHandle, Intrusive::UnorderedSetLink<Federation, Intrusive::ParentTag<Node> > > FederationHandleFederationMap;
  /// Get the set of Federation instances
  FederationHandleFederationMap const& getFederationHandleFederationMap() const
  { return _federationHandleFederationMap; }
  FederationHandleFederationMap& getFederationHandleFederationMap()
  { return _federationHandleFederationMap; }
  /// Get one Federation instance matching federationHandle
  Federation const* getFederation(FederationHandle const& federationHandle) const;
  Federation* getFederation(FederationHandle const& federationHandle);
  void erase(Federation& federation);

  /// UnorderedSet of Federation instances indexed by name
  typedef Intrusive::UnorderedSet<std::string, Intrusive::UnorderedSetLink<Federation, Intrusive::ParentTag<Node, 1> > > FederationNameFederationMap;
  /// Get one Federation instance matching name
  Federation const* getFederation(std::string const& name) const;
  Federation* getFederation(std::string const& name);
  bool getFederationExecutionAlreadyExists(std::string const& name) const;

  void send(ConnectHandle const& connectHandle, const SharedPtr<const AbstractMessage>& message);
  void sendToParent(const SharedPtr<const AbstractMessage>& message);
  void broadcast(const SharedPtr<const AbstractMessage>& message);
  void broadcast(ConnectHandle const& connectHandle, const SharedPtr<const AbstractMessage>& message);
  void broadcastToChildren(const SharedPtr<const AbstractMessage>& message);

protected:
#if 201103L <= __cplusplus
  Node(Node const&) = delete;
  Node(Node&&) = delete;
  Node& operator=(Node const&) = delete;
  Node& operator=(Node&&) = delete;
#else
  Node(Node const&);
  Node& operator=(Node const&);
#if 200610L <= __cpp_rvalue_reference
  Node(Node&&);
  Node& operator=(Node&&);
#endif
#endif

  /// The ConnectHandle of the parent Connect
  ConnectHandle _parentConnectHandle;

  /// The ServerOptions for this server component
  SharedPtr<ServerOptions> _serverOptions;

  /// UnorderedSet of NodeConnect instances indexed by connectHandle
  friend class NodeConnect;
  /// Insert nodeConnect into connectHandleNodeConnectMap
  void _insertConnectHandleNodeConnectMap(NodeConnect& nodeConnect);
  /// Unlink nodeConnect from connectHandleNodeConnectMap
  void _unlinkConnectHandleNodeConnectMap(NodeConnect& nodeConnect);
  ConnectHandleNodeConnectMap _connectHandleNodeConnectMap;

  /// The appropriate HandleAllocator
  HandleAllocator<ConnectHandle> _connectHandleAllocator;

  /// UnorderedSet of Federation instances indexed by federationHandle
  friend class Federation;
  /// Insert federation into federationHandleFederationMap
  void _insertFederationHandleFederationMap(Federation& federation);
  /// Unlink federation from federationHandleFederationMap
  void _unlinkFederationHandleFederationMap(Federation& federation);
  FederationHandleFederationMap _federationHandleFederationMap;

  /// The appropriate HandleAllocator
  FederationHandleAllocator _federationHandleAllocator;

  /// UnorderedSet of Federation instances indexed by name
  /// Insert federation into federationNameFederationMap
  void _insertFederationNameFederationMap(Federation& federation);
  /// Unlink federation from federationNameFederationMap
  void _unlinkFederationNameFederationMap(Federation& federation);
  FederationNameFederationMap _federationNameFederationMap;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_Node_h
