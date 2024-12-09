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

#include "Node.h"

#include "LogStream.h"

namespace OpenRTI {
namespace ServerModel {

Node::Node() :
  _serverOptions(new ServerOptions)
{
}

Node::~Node()
{
  _federationHandleFederationMap.clear();
  _connectHandleNodeConnectMap.clear();

  OpenRTIAssert(_federationNameFederationMap.empty());
  OpenRTIAssert(_federationHandleFederationMap.empty());
  OpenRTIAssert(_connectHandleNodeConnectMap.empty());
}

bool
Node::isRootServer() const
{
  return !_parentConnectHandle.valid();
}

bool
Node::hasChildConnects() const
{
  for (ConnectHandleNodeConnectMap::const_reverse_iterator i = _connectHandleNodeConnectMap.rbegin();
       i != _connectHandleNodeConnectMap.rend(); ++i) {
    if (i->getIsParentConnect())
      continue;
    return true;
  }
  return false;
}

NodeConnect const*
Node::getNodeConnect(ConnectHandle const& connectHandle) const
{
  ConnectHandleNodeConnectMap::const_iterator i;
  i = _connectHandleNodeConnectMap.find(connectHandle);
  if (i == _connectHandleNodeConnectMap.end())
    return 0;
  return i.get();
}

NodeConnect*
Node::getNodeConnect(ConnectHandle const& connectHandle)
{
  ConnectHandleNodeConnectMap::iterator i;
  i = _connectHandleNodeConnectMap.find(connectHandle);
  if (i == _connectHandleNodeConnectMap.end())
    return 0;
  return i.get();
}

NodeConnect*
Node::insertNodeConnect(const SharedPtr<AbstractMessageSender>& messageSender, StringStringListMap const& options)
{
  NodeConnect* nodeConnect = new NodeConnect(*this, _connectHandleAllocator.get());
  insert(*nodeConnect);
  nodeConnect->setMessageSender(messageSender);
  nodeConnect->setOptions(options);
  return nodeConnect;
}

NodeConnect*
Node::insertParentNodeConnect(const SharedPtr<AbstractMessageSender>& messageSender, StringStringListMap const& options)
{
  OpenRTIAssert(!_parentConnectHandle.valid());
  NodeConnect* nodeConnect = insertNodeConnect(messageSender, options);
  nodeConnect->setIsParentConnect(true);
  _parentConnectHandle = nodeConnect->getConnectHandle();
  return nodeConnect;
}

void
Node::insert(NodeConnect& nodeConnect)
{
  _connectHandleNodeConnectMap.insert(nodeConnect);
}

void
Node::erase(ConnectHandle const& connectHandle)
{
  ConnectHandleNodeConnectMap::iterator i = _connectHandleNodeConnectMap.find(connectHandle);
  OpenRTIAssert(i != _connectHandleNodeConnectMap.end());
  erase(*i);
}

void
Node::erase(NodeConnect& nodeConnect)
{
  if (_parentConnectHandle == nodeConnect.getConnectHandle())
    _parentConnectHandle = ConnectHandle();
  _connectHandleAllocator.put(nodeConnect.getConnectHandle());
  ConnectHandleNodeConnectMap::erase(nodeConnect);
}

Federation const*
Node::getFederation(FederationHandle const& federationHandle) const
{
  FederationHandleFederationMap::const_iterator i;
  i = _federationHandleFederationMap.find(federationHandle);
  if (i == _federationHandleFederationMap.end())
    return 0;
  return i.get();
}

Federation*
Node::getFederation(FederationHandle const& federationHandle)
{
  FederationHandleFederationMap::iterator i;
  i = _federationHandleFederationMap.find(federationHandle);
  if (i == _federationHandleFederationMap.end())
    return 0;
  return i.get();
}

void
Node::insert(Federation& federation)
{
  _federationNameFederationMap.insert(federation);
  _federationHandleFederationMap.insert(federation);
}

void
Node::erase(Federation& federation)
{
  OpenRTIAssert(!federation.hasJoinedChildren());
  Log(ServerFederation, Info) << getServerPath() << ": Released FederationHandle in child server for \""
                              << federation.getName() << "\"!" << std::endl;
  _federationHandleAllocator.put(federation.getFederationHandle());
  FederationHandleFederationMap::erase(federation);
}

Federation const*
Node::getFederation(std::string const& name) const
{
  FederationNameFederationMap::const_iterator i;
  i = _federationNameFederationMap.find(name);
  if (i == _federationNameFederationMap.end())
    return 0;
  return i.get();
}

Federation*
Node::getFederation(std::string const& name)
{
  FederationNameFederationMap::iterator i;
  i = _federationNameFederationMap.find(name);
  if (i == _federationNameFederationMap.end())
    return 0;
  return i.get();
}

bool
Node::getFederationExecutionAlreadyExists(std::string const& name) const
{
  return _federationNameFederationMap.find(name) != _federationNameFederationMap.end();
}

void
Node::insertName(Federation& federation)
{
  _federationNameFederationMap.insert(federation);
}

void
Node::eraseName(Federation& federation)
{
  OpenRTIAssert(!federation.hasJoinedChildren());
  Log(ServerFederation, Info) << getServerPath() << ": Destroyed federation execution in child server for \""
                              << federation.getName() << "\"!" << std::endl;
  _federationNameFederationMap.unlink(federation);
}

void
Node::send(ConnectHandle const& connectHandle, const SharedPtr<const AbstractMessage>& message)
{
  ConnectHandleNodeConnectMap::iterator i = _connectHandleNodeConnectMap.find(connectHandle);
  if (i == _connectHandleNodeConnectMap.end())
    return;
  i->send(message);
}

void
Node::sendToParent(const SharedPtr<const AbstractMessage>& message)
{
  send(_parentConnectHandle, message);
}

void
Node::broadcast(const SharedPtr<const AbstractMessage>& message)
{
  for (ConnectHandleNodeConnectMap::iterator i = _connectHandleNodeConnectMap.begin();
       i != _connectHandleNodeConnectMap.end(); ++i) {
    i->send(message);
  }
}

void
Node::broadcast(ConnectHandle const& connectHandle, const SharedPtr<const AbstractMessage>& message)
{
  for (ConnectHandleNodeConnectMap::iterator i = _connectHandleNodeConnectMap.begin();
       i != _connectHandleNodeConnectMap.end(); ++i) {
    if (i->getConnectHandle() == connectHandle)
      continue;
    i->send(message);
  }
}

void
Node::broadcastToChildren(const SharedPtr<const AbstractMessage>& message)
{
  broadcast(_parentConnectHandle, message);
}

} // namespace ServerModel
} // namespace OpenRTI
