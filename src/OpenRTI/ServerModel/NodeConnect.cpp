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

#include "NodeConnect.h"

#include "Federation.h"
#include "FederationConnect.h"
#include "Node.h"

namespace OpenRTI {
namespace ServerModel {

NodeConnect::NodeConnect(Node& serverNode, ConnectHandle const& connectHandle) :
  _serverNode(serverNode),
  _connectHandle(connectHandle),
  _isParentConnect(false)
{
  _serverNode._insertConnectHandleNodeConnectMap(*this);
}

NodeConnect::~NodeConnect()
{
  // We need to make sure that this list got processed before deletion.
  // FIXME, should be empty at this time
  _federationConnectList.clear();

  _serverNode._unlinkConnectHandleNodeConnectMap(*this);

  OpenRTIAssert(_federationConnectList.empty());
}

void
NodeConnect::setIsParentConnect(bool isParentConnect)
{
  _isParentConnect = isParentConnect;
}

void
NodeConnect::setName(std::string const& name)
{
  _name = name;
}

void
NodeConnect::setOptions(StringStringListMap const& options)
{
  _options = options;

  StringStringListMap::const_iterator i = options.find("serverName");
  if (i != options.end() && !i->second.empty())
    _name = i->second.front();
  else
    _name.clear();
}

const SharedPtr<AbstractMessageSender>&
NodeConnect::getMessageSender() const
{
  return _messageSender;
}

void
NodeConnect::setMessageSender(const SharedPtr<AbstractMessageSender>& messageSender)
{
  _messageSender = messageSender;
}

void
NodeConnect::send(const SharedPtr<const AbstractMessage>& message)
{
  if (!_messageSender.valid())
    return;
  _messageSender->send(message);
}

FederationConnect*
NodeConnect::createFederationConnect(Federation& federation)
{
  return new FederationConnect(federation, *this);
}

void
NodeConnect::_insertFederationConnectList(FederationConnect& federationConnect)
{
  _federationConnectList.push_back(federationConnect);
}

void
NodeConnect::_unlinkFederationConnectList(FederationConnect& federationConnect)
{
  _federationConnectList.unlink(federationConnect);
}

} // namespace ServerModel
} // namespace OpenRTI
