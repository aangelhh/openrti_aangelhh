/* -*-c++-*- OpenRTI - Copyright (C) 2009-2026 Mathias Froehlich
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

#ifndef OpenRTI_ServerModel_PublishSubscribe_h
#define OpenRTI_ServerModel_PublishSubscribe_h

#include "Handle.h"
#include "RegionSet.h"

namespace OpenRTI {
namespace ServerModel {

// Helper return type for some publish/subscribe stuff.
// Define outside the template class to avoid having different types of this per template
enum PropagationType {
  PropagateBroadcast,
  PropagateSend,
  PropagateNone
};
typedef std::pair<PropagationType, ConnectHandle> PropagationTypeConnectHandlePair;

class OPENRTI_LOCAL BroadcastConnectHandleSet {
 public:
  /// Returns what to do with the other connects in the server node.
  /// If .first is PropagateBroadcast, tell all but this one that sends this publication about the publication change.
  /// If .first is PropagateSend, tell the connect at .second about the change.
  /// If .first is PropagateNone, nothing has changed for the connects at this server node.
  PropagationTypeConnectHandlePair insert(const ConnectHandle& connectHandle)
  {
    // Mark this connect as publishing
    size_t initialConnectHandleSetSize = _connectHandleSet.size();
    ConnectHandle previousExclusiveConnectHandle;
    if (initialConnectHandleSetSize == 1)
      previousExclusiveConnectHandle = *_connectHandleSet.begin();

    // Did that change something? If not, propagate nothing
    if (!_connectHandleSet.insert(connectHandle).second)
      return PropagationTypeConnectHandlePair(PropagateNone, ConnectHandle());

    switch (initialConnectHandleSetSize) {
    case 0:
      // Nobody was publishing this so far. Tell all *others* about that
      return PropagationTypeConnectHandlePair(PropagateBroadcast, ConnectHandle());
    case 1:
      // One was publishing this so far. Due to not getting the loopback message in the 0 case just send that
      // remaining connect that does not yet know that this server node is already publishing this the publication
      // notification
      return PropagationTypeConnectHandlePair(PropagateSend, previousExclusiveConnectHandle);
    default:
      // Nothing changes, nothing todo
      return PropagationTypeConnectHandlePair(PropagateNone, ConnectHandle());
    }
  }

  /// Returns what to do with the other connects in the server node.
  /// If .first is PropagateBroadcast, tell all but this one that sends this publication about the publication change.
  /// If .first is PropagateSend, tell the connect at .second about the change.
  /// If .first is PropagateNone, nothing has changed for the connects at this server node.
  PropagationTypeConnectHandlePair erase(const ConnectHandle& connectHandle)
  {
    // Did that change something? If not, propagate nothing
    if (_connectHandleSet.erase(connectHandle) == 0)
      return PropagationTypeConnectHandlePair(PropagateNone, ConnectHandle());

    switch (_connectHandleSet.size()) {
    case 0:
      return PropagationTypeConnectHandlePair(PropagateBroadcast, ConnectHandle());
    case 1:
      return PropagationTypeConnectHandlePair(PropagateSend, *_connectHandleSet.begin());
    default:
      // Nothing changes, nothing todo
      return PropagationTypeConnectHandlePair(PropagateNone, ConnectHandle());
    }
  }

  /// Returns if any existing connect in this server node publishes this
  bool empty() const
  { return _connectHandleSet.empty(); }

  /// Returns if the given connect handle publishes this
  bool contains(const ConnectHandle& connectHandle) const
  { return _connectHandleSet.find(connectHandle) != _connectHandleSet.end(); }
  /// Returns the subscription type of all connects except the given one
  bool containsMoreThan(const ConnectHandle& connectHandle) const
  {
    size_t size = _connectHandleSet.size();
    if (1 < size)
      return true;
    if (size == 1 && *_connectHandleSet.begin() != connectHandle)
      return true;
    return false;
  }

  const ConnectHandleSet& getConnectHandleSet() const
  { return _connectHandleSet; }

 private:
  ConnectHandleSet _connectHandleSet;
};

class OPENRTI_LOCAL PublishSubscribe {
public:

  /// Change publication state of the given connect.
  /// Returns what to do with the other connects in the server node.
  /// If .first is PropagateBroadcast, tell all but this one that sends this publication about the publication change.
  /// If .first is PropagateSend, tell the connect at .second about the change.
  /// If .first is PropagateNone, nothing has changed for the connects at this server node.
  PropagationTypeConnectHandlePair setPublicationType(const ConnectHandle& connectHandle, PublicationType publicationType)
  {
    if (publicationType == Published) {
      return _publishedConnects.insert(connectHandle);
    } else {
      return _publishedConnects.erase(connectHandle);
    }
  }
  /// Returns if the given connect handle publishes this
  PublicationType getPublicationType(const ConnectHandle& connectHandle) const
  {
    if (_publishedConnects.contains(connectHandle))
      return Published;
    else
      return Unpublished;
  }
  /// Returns if any existing connect in this server node publishes this
  PublicationType getPublicationType() const
  {
    if (_publishedConnects.empty())
      return Unpublished;
    else
      return Published;
  }
  const ConnectHandleSet& getPublishingConnectHandleSet() const
  { return _publishedConnects.getConnectHandleSet(); }

  // FIXME currently only passive subscriptions are propagated

  /// Change publication state of the given connect.
  /// Returns what to do with the other connects in the server node.
  /// If .first is PropagateBroadcast, tell all but this one that sends this publication about the publication change.
  /// If .first is PropagateSend, tell the connect at .second about the change.
  /// If .first is PropagateNone, nothing has changed for the connects at this server node.
  PropagationTypeConnectHandlePair setSubscriptionType(const ConnectHandle& connectHandle, SubscriptionType subscriptionType)
  {
    // if (subscriptionType == Subscribed) {
    if (subscriptionType != Unsubscribed) {
      return _subscribedConnects.insert(connectHandle);
    } else {
      return _subscribedConnects.erase(connectHandle);
    }
  }

  SubscriptionType getSubscriptionType() const
  {
    if (!_activeSubscribedConnects.empty())
      return SubscribedActive;
    else if (!_subscribedConnects.empty())
      return SubscribedPassive;
    else
      return Unsubscribed;
  }

  SubscriptionType getSubscriptionType(const ConnectHandle& connectHandle) const
  {
    if (_activeSubscribedConnects.contains(connectHandle))
      return SubscribedActive;
    else if (_subscribedConnects.contains(connectHandle))
      return SubscribedPassive;
    else
      return Unsubscribed;
  }
  /// Returns the subscription type of all connects except the given one
  SubscriptionType getSubscriptionTypeToConnect(const ConnectHandle& connectHandle) const
  {
    if (_activeSubscribedConnects.containsMoreThan(connectHandle))
      return SubscribedActive;
    if (_subscribedConnects.containsMoreThan(connectHandle))
      return SubscribedPassive;
    return Unsubscribed;
  }

  const ConnectHandleSet& getSubscribedConnectHandleSet() const
  { return _subscribedConnects.getConnectHandleSet(); }
  void getSubscribedAndIntersectingConnectHandleSet(ConnectHandleSet& connectHandleSet, const RegionSet& regionSet) const
  { connectHandleSet = _subscribedConnects.getConnectHandleSet(); }

  void removeConnect(const ConnectHandle& connectHandle)
  {
    // FIXME, should not use this
    setSubscriptionType(connectHandle, Unsubscribed);

    // The the unpublish must have happened before, so just make sure that it is empty
    OpenRTIAssert(!_publishedConnects.contains(connectHandle));
    // The the unsubscribe must have happened before, so just make sure that it is empty
    OpenRTIAssert(!_activeSubscribedConnects.contains(connectHandle));
    OpenRTIAssert(!_subscribedConnects.contains(connectHandle));
  }

  /// returns true if something changed
  bool updateCumulativeSubscribedConnectHandleSet(const ConnectHandle& connectHandle, bool subscribe)
  {
    if (subscribe) {
      return _cumulativeSubscribedConnectHandleSet.insert(connectHandle).second;
    } else {
      return 0 != _cumulativeSubscribedConnectHandleSet.erase(connectHandle);
    }
  }

  /// The cumulative set of connects that subscribe to this.
  /// Cumulative in the sense that also subscriptions to base classes are included here.
  ConnectHandleSet _cumulativeSubscribedConnectHandleSet;

 private:
  /// All the connects that publish this
  BroadcastConnectHandleSet _publishedConnects;

  /// All the connects that are regionless subscribed at this
  /// Since region subscriptions should not interfere with normal subscriptions, have these two and something for
  /// regions set subscriptions without any active/passive flag
  BroadcastConnectHandleSet _subscribedConnects;
  BroadcastConnectHandleSet _activeSubscribedConnects;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_PublishSubscribe_h
