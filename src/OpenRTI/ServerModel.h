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

#ifndef OpenRTI_ServerModel_h
#define OpenRTI_ServerModel_h

#include <list>
#include <string>

#include "AbstractMessageSender.h"
#include "Exception.h"
#include "Handle.h"
#include "HandleAllocator.h"
#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"
#include "Referenced.h"
#include "RegionSet.h"
#include "ServerOptions.h"
#include "StringUtils.h"
#include "Types.h"
#include "LogStream.h"

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
  // Returns what to do with the other connects in the server node.
  // If .first is PropagateBroadcast, tell all but this one that sends this publication about the publication change.
  // If .first is PropagateSend, tell the connect at .second about the change.
  // If .first is PropagateNone, nothing has changed for the connects at this server node.
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

  // Returns what to do with the other connects in the server node.
  // If .first is PropagateBroadcast, tell all but this one that sends this publication about the publication change.
  // If .first is PropagateSend, tell the connect at .second about the change.
  // If .first is PropagateNone, nothing has changed for the connects at this server node.
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

  // Returns if any existing connect in this server node publishes this
  bool empty() const
  { return _connectHandleSet.empty(); }

  // Returns if the given connect handle publishes this
  bool contains(const ConnectHandle& connectHandle) const
  { return _connectHandleSet.find(connectHandle) != _connectHandleSet.end(); }
  // Returns the subscription type of all connects except the given one
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

  // Change publication state of the given connect.
  // Returns what to do with the other connects in the server node.
  // If .first is PropagateBroadcast, tell all but this one that sends this publication about the publication change.
  // If .first is PropagateSend, tell the connect at .second about the change.
  // If .first is PropagateNone, nothing has changed for the connects at this server node.
  PropagationTypeConnectHandlePair setPublicationType(const ConnectHandle& connectHandle, PublicationType publicationType)
  {
    if (publicationType == Published) {
      return _publishedConnects.insert(connectHandle);
    } else {
      return _publishedConnects.erase(connectHandle);
    }
  }
  // Returns if the given connect handle publishes this
  PublicationType getPublicationType(const ConnectHandle& connectHandle) const
  {
    if (_publishedConnects.contains(connectHandle))
      return Published;
    else
      return Unpublished;
  }
  // Returns if any existing connect in this server node publishes this
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

  // Change publication state of the given connect.
  // Returns what to do with the other connects in the server node.
  // If .first is PropagateBroadcast, tell all but this one that sends this publication about the publication change.
  // If .first is PropagateSend, tell the connect at .second about the change.
  // If .first is PropagateNone, nothing has changed for the connects at this server node.
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
  // Returns the subscription type of all connects except the given one
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

  // returns true if something changed
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
  // All the connects that publish this
  BroadcastConnectHandleSet _publishedConnects;

  // All the connects that are regionless subscribed at this
  // Since region subscriptions should not interfere with normal subscriptions, have these two and something for
  // regions set subscriptions without any active/passive flag
  BroadcastConnectHandleSet _subscribedConnects;
  BroadcastConnectHandleSet _activeSubscribedConnects;
};

///// FIXME above here should also move into a clean referencing scheme to the connects and what not.





















////////////////////////////////////////////////////////////

template<typename T, typename H>
class OPENRTI_LOCAL HandleEntity : public IntrusiveUnorderedMap<H, T>::Hook {
public:
  typedef IntrusiveUnorderedMap<H, T> HandleMap;

protected:
  HandleEntity()
  { }
  HandleEntity(const H& handle) :
    IntrusiveUnorderedMap<H, T>::Hook(handle)
  { }

  const H& _getHandle() const
  { return HandleMap::Hook::getKey(); }
  void _setHandle(const H& handle)
  { HandleMap::Hook::setKey(handle); }
};

////////////////////////////////////////////////////////////

template<typename T, typename H>
class OPENRTI_LOCAL HandleListEntity : public IntrusiveUnorderedMap<H, T>::Hook, public IntrusiveList<T, 0>::Hook {
public:
  typedef IntrusiveUnorderedMap<H, T> HandleMap;
  typedef IntrusiveList<T, 0> FirstList;

protected:
  HandleListEntity()
  { }
  HandleListEntity(const H& handle) :
    IntrusiveUnorderedMap<H, T>::Hook(handle)
  { }

  const H& _getHandle() const
  { return HandleMap::Hook::getKey(); }
  void _setHandle(const H& handle)
  { HandleMap::Hook::setKey(handle); }
};

////////////////////////////////////////////////////////////

template<typename T, typename H, typename S = std::string>
class OPENRTI_LOCAL HandleStringEntity : public IntrusiveUnorderedMap<H, T>::Hook, public IntrusiveUnorderedMap<S, T>::Hook {
public:
  typedef IntrusiveUnorderedMap<H, T> HandleMap;
  typedef IntrusiveUnorderedMap<S, T> StringMap;

protected:
  HandleStringEntity()
  { }
  HandleStringEntity(const H& handle, const S& name) :
    IntrusiveUnorderedMap<H, T>::Hook(handle),
    IntrusiveUnorderedMap<S, T>::Hook(name)
  { }

  const H& _getHandle() const
  { return HandleMap::Hook::getKey(); }
  void _setHandle(const H& handle)
  { HandleMap::Hook::setKey(handle); }

  const S& _getString() const
  { return StringMap::Hook::getKey(); }
  void _setString(const S& string)
  { StringMap::Hook::setKey(string); }
};

////////////////////////////////////////////////////////////

template<typename T>
class OPENRTI_LOCAL ListPair : public IntrusiveList<T, 0>::Hook, public IntrusiveList<T, 1>::Hook {
public:
  typedef IntrusiveList<T, 0> FirstList;
  typedef IntrusiveList<T, 1> SecondList;
};

////////////////////////////////////////////////////////////

// FIXME: make the Region some fixed definition but the RegionSubscription, RegionAssociation
// a tree element that is fast in its lookup!!

// IDEA, put often used query combinations in a hash and try to cache the results - at least for interaction delivery.

// class OPENRTI_LOCAL RegionSubscription {
// };

class OPENRTI_LOCAL Region : public HandleEntity<Region, LocalRegionHandle> {
public:
  typedef HandleEntity<Region, LocalRegionHandle>::HandleMap HandleMap;

  Region();
  ~Region();

  const LocalRegionHandle& getRegionHandle() const
  { return HandleEntity<Region, LocalRegionHandle>::_getHandle(); }
  void setRegionHandle(const LocalRegionHandle& regionHandle);

  RegionValue _regionValue;
  DimensionHandleSet _dimensionHandleSet;

private:
  Region(const Region&);
  Region& operator=(const Region&);
};

////////////////////////////////////////////////////////////

class ClassAttribute;
class ObjectInstance;

class OPENRTI_LOCAL InstanceAttribute : public HandleEntity<InstanceAttribute, AttributeHandle> {
public:
  typedef HandleEntity<InstanceAttribute, AttributeHandle>::HandleMap HandleMap;

  InstanceAttribute(ObjectInstance& objectInstance, ClassAttribute& classAttribute);
  ~InstanceAttribute();

  const AttributeHandle& getAttributeHandle() const
  { return HandleEntity<InstanceAttribute, AttributeHandle>::_getHandle(); }
  void setAttributeHandle(const AttributeHandle& attributeHandle);

  const ObjectInstance& getObjectInstance() const
  { return _objectInstance; }
  ObjectInstance& getObjectInstance()
  { return _objectInstance; }

  const ClassAttribute& getClassAttribute() const
  { return _classAttribute; }
  ClassAttribute& getClassAttribute()
  { return _classAttribute; }

  /// Get the ConnectHandle this attribute is owned
  const ConnectHandle& getOwnerConnectHandle() const
  { return _ownerConnectHandle; }
  void setOwnerConnectHandle(const ConnectHandle& connectHandle)
  {
    _receivingConnects.erase(connectHandle);
    _ownerConnectHandle = connectHandle;
  }

  void removeConnect(const ConnectHandle& connectHandle)
  {
    _receivingConnects.erase(connectHandle);
    if (_ownerConnectHandle == connectHandle)
      _ownerConnectHandle = ConnectHandle();
  }

  // Because of attribute ownership, it is clear for an object attribute where the update
  // stems from, so just have a set of connect handles that want to receive the updates
  ConnectHandleSet _receivingConnects;

  /// The connect this attribute is owned by
  ConnectHandle _ownerConnectHandle;

private:
  InstanceAttribute(const InstanceAttribute&);
  InstanceAttribute& operator=(const InstanceAttribute&);

  ObjectInstance& _objectInstance;
  ClassAttribute& _classAttribute;
};

////////////////////////////////////////////////////////////

class FederationConnect;
class ObjectInstance;

class OPENRTI_LOCAL ObjectInstanceConnect : public HandleListEntity<ObjectInstanceConnect, ConnectHandle> {
public:
  typedef HandleListEntity<ObjectInstanceConnect, ConnectHandle>::HandleMap HandleMap;
  typedef HandleListEntity<ObjectInstanceConnect, ConnectHandle>::FirstList FirstList;

  ObjectInstanceConnect(ObjectInstance& objectInstance, FederationConnect& federationConnect);
  ~ObjectInstanceConnect();

  /// The connect handle to identify this connect
  const ConnectHandle& getConnectHandle() const
  { return HandleMap::Hook::getKey(); }
  void setConnectHandle(const ConnectHandle& connectHandle);

  const ObjectInstance& getObjectInstance() const
  { return _objectInstance; }
  ObjectInstance& getObjectInstance()
  { return _objectInstance; }

  const FederationConnect& getFederationConnect() const
  { return _federationConnect; }
  FederationConnect& getFederationConnect()
  { return _federationConnect; }

private:
  ObjectInstanceConnect(const ObjectInstanceConnect&);
  ObjectInstanceConnect& operator=(const ObjectInstanceConnect&);

  ObjectInstance& _objectInstance;
  FederationConnect& _federationConnect;
};

////////////////////////////////////////////////////////////

class Federation;
class ObjectClass;

class OPENRTI_LOCAL ObjectInstance : public HandleStringEntity<ObjectInstance, ObjectInstanceHandle>, public IntrusiveList<ObjectInstance, 0>::Hook {
public:
  typedef HandleStringEntity<ObjectInstance, ObjectInstanceHandle>::HandleMap HandleMap;
  typedef HandleStringEntity<ObjectInstance, ObjectInstanceHandle>::StringMap NameMap;
  typedef IntrusiveList<ObjectInstance, 0> FirstList;

  ObjectInstance(Federation& federation);
  ~ObjectInstance();

  const ObjectInstanceHandle& getObjectInstanceHandle() const
  { return HandleStringEntity<ObjectInstance, ObjectInstanceHandle>::_getHandle(); }
  void setObjectInstanceHandle(const ObjectInstanceHandle& objectInstanceHandle);

  const std::string& getName() const
  { return HandleStringEntity<ObjectInstance, ObjectInstanceHandle>::_getString(); }
  void setName(const std::string& name);

  const Federation& getFederation() const
  { return _federation; }
  Federation& getFederation()
  { return _federation; }

  void insert(InstanceAttribute& instanceAttribute);
  InstanceAttribute* getInstanceAttribute(const AttributeHandle& attributeHandle);
  InstanceAttribute* getPrivilegeToDeleteInstanceAttribute();
  InstanceAttribute::HandleMap& getAttributeHandleInstanceAttributeMap()
  { return _attributeHandleInstanceAttributeMap; }

  void insert(ObjectInstanceConnect& objectInstanceConnect)
  { _connectHandleObjectInstanceConnectMap.insert(objectInstanceConnect); }
  ObjectInstanceConnect::HandleMap& getConnectHandleObjectInstanceConnectMap()
  { return _connectHandleObjectInstanceConnectMap; }
  /// Mark the name handle pair also represented with this as used in the federationConnect
  void reference(FederationConnect& federationConnect);
  /// Releases the ObjectInstanceConnect entry belonging to the connectHandle
  bool unreference(const ConnectHandle& connectHandle);

  void removeConnect(const ConnectHandle& connectHandle);

  ObjectClass* getObjectClass()
  { return _objectClass; }
  void setObjectClass(ObjectClass* objectClass);

  /// Return the connect that owns this object
  ConnectHandle getOwnerConnectHandle()
  {
    InstanceAttribute* instanceAttribute = getInstanceAttribute(AttributeHandle(0));
    if (!instanceAttribute)
      return ConnectHandle();
    return instanceAttribute->getOwnerConnectHandle();
  }
  void setOwnerConnectHandle(const ConnectHandle& connectHandle)
  {
    InstanceAttribute* instanceAttribute = getInstanceAttribute(AttributeHandle(0));
    if (!instanceAttribute)
      return;
    instanceAttribute->setOwnerConnectHandle(connectHandle);
  }

private:
  ObjectInstance(const ObjectInstance&);
  ObjectInstance& operator=(const ObjectInstance&);

  Federation& _federation;

  /// The pointer to the object class this object is an instance of, can be zero
  ObjectClass* _objectClass;

  /// All the instance attributes here
  InstanceAttribute::HandleMap _attributeHandleInstanceAttributeMap;

  // List of object instance handle/name references at this connect.
  ObjectInstanceConnect::HandleMap _connectHandleObjectInstanceConnectMap;
};

////////////////////////////////////////////////////////////

class Federate;
class Synchronization;

class OPENRTI_LOCAL SynchronizationFederate : public HandleListEntity<SynchronizationFederate, FederateHandle>  {
public:
  typedef HandleListEntity<SynchronizationFederate, FederateHandle>::HandleMap HandleMap;
  typedef HandleListEntity<SynchronizationFederate, FederateHandle>::FirstList FirstList;

  SynchronizationFederate(Synchronization& synchronization, Federate& federate);
  ~SynchronizationFederate();

  const FederateHandle& getFederateHandle() const
  { return HandleListEntity<SynchronizationFederate, FederateHandle>::_getHandle(); }
  void setFederateHandle(const FederateHandle& federateHandle);

  const Synchronization& getSynchronization() const
  { return _synchronization; }
  Synchronization& getSynchronization()
  { return _synchronization; }
  const Federate& getFederate() const
  { return _federate; }
  Federate& getFederate()
  { return _federate; }

  bool getSuccessful() const
  { return _successful; }
  void setSuccessful(bool successful);

private:
  SynchronizationFederate(const SynchronizationFederate&);
  SynchronizationFederate& operator=(const SynchronizationFederate&);

  Synchronization& _synchronization;
  Federate& _federate;

  bool _successful;
};

////////////////////////////////////////////////////////////

class OPENRTI_LOCAL Synchronization : public IntrusiveUnorderedMap<std::string, Synchronization>::Hook {
public:
  typedef IntrusiveUnorderedMap<std::string, Synchronization> NameMap;

  Synchronization();
  ~Synchronization();

  const std::string& getLabel() const
  { return NameMap::Hook::getKey(); }
  void setLabel(const std::string& label);

  const VariableLengthData& getTag() const
  { return _tag; }
  void setTag(const VariableLengthData& tag);

  bool getAddJoiningFederates() const
  { return _addJoiningFederates; }
  void setAddJoiningFederates(bool addJoiningFederates);

  bool getIsWaitingFor(const FederateHandle& federateHandle);

  void insert(Federate& federate);
  void achieved(const FederateHandle& federateHandle, bool successful);

  // private:

  // The FederateHandle to SynchronizationFederate map of federates
  // waiting for this synchronization point
  SynchronizationFederate::HandleMap _waitingFederateSynchronizationMap;
  // The FederateHandle to SynchronizationFederate map of federates
  // that have achieved the synchronization point
  SynchronizationFederate::HandleMap _achievedFederateSynchronizationMap;

private:
  Synchronization(const Synchronization&);
  Synchronization& operator=(const Synchronization&);

  VariableLengthData _tag;

  bool _addJoiningFederates;
};

////////////////////////////////////////////////////////////

class NodeConnect;
class Federation;
class FederationConnect;

class OPENRTI_LOCAL Federate : public HandleStringEntity<Federate, FederateHandle>, public IntrusiveList<Federate, 0>::Hook, public IntrusiveList<Federate, 1>::Hook {
public:
  typedef HandleStringEntity<Federate, FederateHandle>::HandleMap HandleMap;
  typedef HandleStringEntity<Federate, FederateHandle>::StringMap NameMap;
  typedef IntrusiveList<Federate, 0> FirstList; // Used to access federates from the FederationConnect
  typedef IntrusiveList<Federate, 1> SecondList; // Used to access time regulating federates from the FederationConnect

  Federate(Federation& federation);
  ~Federate();

  const Federation& getFederation() const
  { return _federation; }
  Federation& getFederation()
  { return _federation; }

  const std::string& getName() const
  { return HandleStringEntity<Federate, FederateHandle>::_getString(); }
  void setName(const std::string& name);

  const FederateHandle& getFederateHandle() const
  { return HandleStringEntity<Federate, FederateHandle>::_getHandle(); }
  void setFederateHandle(const FederateHandle& federateHandle);

  const std::string& getFederateType() const
  { return _federateType; }
  void setFederateType(const std::string& federateType);

  ResignAction getResignAction() const
  { return _resignAction; }
  void setResignAction(ResignAction resignAction);

  bool getResignPending() const
  { return _resignPending; }
  void setResignPending(bool resignPending);

  void insert(SynchronizationFederate& synchronizationFederate);
  SynchronizationFederate::FirstList& getSynchronizationFederateList()
  { return _synchronizationFederateList; }

  Region* getRegion(const LocalRegionHandle& regionHandle);
  void insert(Region& region)
  { _regionHandleRegionMap.insert(region); }
  // void erase(Region& region)
  // { Region::HandleMap::dispose(region); }
  Region::HandleMap& getRegionHandleRegionMap()
  { return _regionHandleRegionMap; }

  bool getIsTimeRegulating() const;

  const VariableLengthData& getTimeAdvanceTimeStamp() const
  { return _timeAdvanceTimeStamp; }
  void setTimeAdvanceTimeStamp(const VariableLengthData& timeAdvanceTimeStamp);

  const VariableLengthData& getNextMessageTimeStamp() const
  { return _nextMessageTimeStamp; }
  void setNextMessageTimeStamp(const VariableLengthData& nextMessageTimeStamp);

  Unsigned getCommitId() const
  { return _commitId; }
  void setCommitId(Unsigned commitId);

  void setFederationConnect(FederationConnect* federationConnect)
  {
    OpenRTIAssert(federationConnect || !getIsTimeRegulating());
    _federationConnect = federationConnect;
  }
  FederationConnect* getFederationConnect() const
  { return _federationConnect; }
  ConnectHandle getConnectHandle() const;
  void send(const SharedPtr<const AbstractMessage>& message);

private:
  Federate(const Federate&);
  Federate& operator=(const Federate&);

  Federation& _federation;

  std::string _federateType;

  ResignAction _resignAction;

  bool _resignPending;

  FederationConnect* _federationConnect;

  SynchronizationFederate::FirstList _synchronizationFederateList;

  Region::HandleMap _regionHandleRegionMap;

  // Time constrained federates current state
  VariableLengthData _timeAdvanceTimeStamp;
  VariableLengthData _nextMessageTimeStamp;
  Unsigned _commitId;
};

////////////////////////////////////////////////////////////

class Dimension;
class Module;

class OPENRTI_LOCAL DimensionModule : public ListPair<DimensionModule> {
public:
  DimensionModule(Dimension& dimension, Module& module);
  ~DimensionModule();

  const Dimension& getDimension() const
  { return _dimension; }
  Dimension& getDimension()
  { return _dimension; }
  const Module& getModule() const
  { return _module; }
  Module& getModule()
  { return _module; }

private:
  DimensionModule(const DimensionModule&);
  DimensionModule& operator=(const DimensionModule&);

  Dimension& _dimension;
  Module& _module;
};

////////////////////////////////////////////////////////////

template<typename T, typename H, typename S = std::string>
class OPENRTI_LOCAL ModuleEntity : public HandleStringEntity<T, H, S> {
public:

protected:
  ModuleEntity()
  { }
  ModuleEntity(const H& handle, const S& name) :
    HandleStringEntity<T, H, S>(handle, name)
  { }
};

////////////////////////////////////////////////////////////

template<typename T, typename H, typename S = StringVector>
class ModuleClassEntity : public ModuleEntity<T, H, S>, public IntrusiveList<T, 0>::Hook {
public:
  typedef IntrusiveList<T, 0> ChildList;

protected:
  ModuleClassEntity()
  { }
  ModuleClassEntity(const H& handle, const S& name) :
    ModuleEntity<T, H, S>(handle, name)
  { }
};

////////////////////////////////////////////////////////////

class Federation;

class OPENRTI_LOCAL Dimension : public ModuleEntity<Dimension, DimensionHandle> {
public:
  typedef ModuleEntity<Dimension, DimensionHandle>::HandleMap HandleMap;
  typedef ModuleEntity<Dimension, DimensionHandle>::StringMap NameMap;

  Dimension(Federation& federation);
  ~Dimension();

  const Federation& getFederation() const
  { return _federation; }
  Federation& getFederation()
  { return _federation; }

  const std::string& getName() const
  { return ModuleEntity<Dimension, DimensionHandle>::_getString(); }
  void setName(const std::string& name);

  const DimensionHandle& getDimensionHandle() const
  { return ModuleEntity<Dimension, DimensionHandle>::_getHandle(); }
  void setDimensionHandle(const DimensionHandle& dimensionHandle);

  const Unsigned& getUpperBound() const
  { return _upperBound; }
  void setUpperBound(const Unsigned& upperBound);

  bool getIsReferencedByAnyModule() const;

  void insert(DimensionModule& dimensionModule)
  { _dimensionModuleList.push_back(dimensionModule); }

private:
  Dimension(const Dimension&);
  Dimension& operator=(const Dimension&);

  Federation& _federation;

  // The upper bound
  Unsigned _upperBound;

  // The list of modules referencing this dimension.
  DimensionModule::SecondList _dimensionModuleList;
};

////////////////////////////////////////////////////////////

class Module;
class UpdateRate;

class OPENRTI_LOCAL UpdateRateModule : public ListPair<UpdateRateModule> {
public:
  UpdateRateModule(UpdateRate& updateRate, Module& module);
  ~UpdateRateModule();

  const UpdateRate& getUpdateRate() const
  { return _updateRate; }
  UpdateRate& getUpdateRate()
  { return _updateRate; }
  const Module& getModule() const
  { return _module; }
  Module& getModule()
  { return _module; }

private:
  UpdateRateModule(const UpdateRateModule&);
  UpdateRateModule& operator=(const UpdateRateModule&);

  UpdateRate& _updateRate;
  Module& _module;
};

////////////////////////////////////////////////////////////

class Federation;

class OPENRTI_LOCAL UpdateRate : public ModuleEntity<UpdateRate, UpdateRateHandle> {
public:
  typedef ModuleEntity<UpdateRate, UpdateRateHandle>::HandleMap HandleMap;
  typedef ModuleEntity<UpdateRate, UpdateRateHandle>::StringMap NameMap;

  UpdateRate(Federation& federation);
  ~UpdateRate();

  const Federation& getFederation() const
  { return _federation; }
  Federation& getFederation()
  { return _federation; }

  const std::string& getName() const
  { return ModuleEntity<UpdateRate, UpdateRateHandle>::_getString(); }
  void setName(const std::string& name);

  const UpdateRateHandle& getUpdateRateHandle() const
  { return ModuleEntity<UpdateRate, UpdateRateHandle>::_getHandle(); }
  void setUpdateRateHandle(const UpdateRateHandle& updateRateHandle);

  const double& getRate() const
  { return _rate; }
  void setRate(const double& rate);

  bool getIsReferencedByAnyModule() const;

  void insert(UpdateRateModule& updateRateModule)
  { _updateRateModuleList.push_back(updateRateModule); }

private:
  UpdateRate(const UpdateRate&);
  UpdateRate& operator=(const UpdateRate&);

  Federation& _federation;

  double _rate;

  UpdateRateModule::SecondList _updateRateModuleList;
};

////////////////////////////////////////////////////////////

class InteractionClass;
class Module;

class OPENRTI_LOCAL InteractionClassModule : public ListPair<InteractionClassModule> {
public:
  InteractionClassModule(InteractionClass& interactionClass, Module& module);
  ~InteractionClassModule();

  const InteractionClass& getInteractionClass() const
  { return _interactionClass; }
  InteractionClass& getInteractionClass()
  { return _interactionClass; }
  const Module& getModule() const
  { return _module; }
  Module& getModule()
  { return _module; }

private:
  InteractionClassModule(const InteractionClassModule&);
  InteractionClassModule& operator=(const InteractionClassModule&);

  InteractionClass& _interactionClass;
  Module& _module;
};

////////////////////////////////////////////////////////////

class InteractionClass;
class Module;

class OPENRTI_LOCAL ParameterDefinitionModule : public ListPair<ParameterDefinitionModule> {
public:
  ParameterDefinitionModule(InteractionClass& interactionClass, Module& module);
  ~ParameterDefinitionModule();

  const InteractionClass& getInteractionClass() const
  { return _interactionClass; }
  InteractionClass& getInteractionClass()
  { return _interactionClass; }
  const Module& getModule() const
  { return _module; }
  Module& getModule()
  { return _module; }

private:
  ParameterDefinitionModule(const ParameterDefinitionModule&);
  ParameterDefinitionModule& operator=(const ParameterDefinitionModule&);

  InteractionClass& _interactionClass;
  Module& _module;
};

////////////////////////////////////////////////////////////

class InteractionClass;
class ParameterDefinition;

class OPENRTI_LOCAL ClassParameter : public HandleListEntity<ClassParameter, ParameterHandle> {
public:
  typedef HandleListEntity<ClassParameter, ParameterHandle>::HandleMap HandleMap;
  typedef HandleListEntity<ClassParameter, ParameterHandle>::FirstList FirstList;

  ClassParameter(InteractionClass& interactionClass, ParameterDefinition& parameterDefinition);
  ~ClassParameter();

  const ParameterHandle& getParameterHandle() const
  { return HandleListEntity<ClassParameter, ParameterHandle>::_getHandle(); }
  void setParameterHandle(const ParameterHandle& parameterHandle);

  const InteractionClass& getInteractionClass() const
  { return _interactionClass; }
  InteractionClass& getInteractionClass()
  { return _interactionClass; }

  const ParameterDefinition& getParameterDefinition() const
  { return _parameterDefinition; }
  ParameterDefinition& getParameterDefinition()
  { return _parameterDefinition; }

private:
  ClassParameter(const ClassParameter&);
  ClassParameter& operator=(const ClassParameter&);

  InteractionClass& _interactionClass;
  ParameterDefinition& _parameterDefinition;
};

////////////////////////////////////////////////////////////

class OPENRTI_LOCAL ParameterDefinition : public HandleStringEntity<ParameterDefinition, ParameterHandle> {
public:
  typedef HandleStringEntity<ParameterDefinition, ParameterHandle>::HandleMap HandleMap;
  typedef HandleStringEntity<ParameterDefinition, ParameterHandle>::StringMap NameMap;

  ParameterDefinition(InteractionClass& interactionClass);
  ~ParameterDefinition();

  const std::string& getName() const
  { return HandleStringEntity<ParameterDefinition, ParameterHandle>::_getString(); }
  void setName(const std::string& name);

  const ParameterHandle& getParameterHandle() const
  { return HandleStringEntity<ParameterDefinition, ParameterHandle>::_getHandle(); }
  void setParameterHandle(const ParameterHandle& parameterHandle);

  const InteractionClass& getInteractionClass() const
  { return _interactionClass; }
  InteractionClass& getInteractionClass()
  { return _interactionClass; }

  void insert(ClassParameter& classParameter)
  { _classParameterList.push_back(classParameter); }
  ClassParameter::FirstList& getClassParameterList()
  { return _classParameterList; }

private:
  ParameterDefinition(const ParameterDefinition&);
  ParameterDefinition& operator=(const ParameterDefinition&);

  InteractionClass& _interactionClass;

  ClassParameter::FirstList _classParameterList;
};

////////////////////////////////////////////////////////////

class OPENRTI_LOCAL InteractionClass : public ModuleClassEntity<InteractionClass, InteractionClassHandle>, public PublishSubscribe /*FIXME*/ {
public:
  typedef ModuleClassEntity<InteractionClass, InteractionClassHandle>::HandleMap HandleMap;
  typedef ModuleClassEntity<InteractionClass, InteractionClassHandle>::StringMap NameMap;

  InteractionClass(Federation& federation, InteractionClass* parentInteractionClass = 0);
  ~InteractionClass();

  const Federation& getFederation() const
  { return _federation; }
  Federation& getFederation()
  { return _federation; }

  const StringVector& getName() const
  { return ModuleClassEntity<InteractionClass, InteractionClassHandle>::_getString(); }
  void setName(const StringVector& name);

  const InteractionClassHandle& getInteractionClassHandle() const
  { return ModuleClassEntity<InteractionClass, InteractionClassHandle>::_getHandle(); }
  void setInteractionClassHandle(const InteractionClassHandle& interactionClassHandle);

  OrderType getOrderType() const
  { return _orderType; }
  void setOrderType(OrderType orderType);

  TransportationType getTransportationType() const
  { return _transportationType; }
  void setTransportationType(TransportationType transportationType);

  InteractionClass* getParentInteractionClass();
  InteractionClassHandle getParentInteractionClassHandle() const;

  ChildList& getChildInteractionClassList()
  { return _childInteractionClassList; }

  bool getIsReferencedByAnyModule() const;
  bool getAreParametersReferencedByAnyModule() const;

  void eraseParameterDefinitions();
  std::size_t getNumParameterDefinitions() const;
  ParameterHandle getFirstUnusedParameterHandle();

  void insert(ParameterDefinition& parameterDefinition);
  ParameterDefinition* getParameterDefinition(const std::string& name);
  ParameterDefinition* getParameterDefinition(const ParameterHandle& parameterHandle);
  const ParameterDefinition::HandleMap& getParameterHandleParameterDefinitionMap() const
  { return _parameterHandleParameterDefinitionMap; }
  ParameterDefinition::HandleMap& getParameterHandleParameterDefinitionMap()
  { return _parameterHandleParameterDefinitionMap; }

  void insertClassParameterFor(ParameterDefinition& parameterDefinition);
  ClassParameter* getClassParameter(const ParameterHandle& parameterHandle);
  ClassParameter::HandleMap& getParameterHandleClassParameterMap()
  { return _parameterHandleClassParameterMap; }

  void insert(InteractionClassModule& interactionClassModule)
  { _interactionClassModuleList.push_back(interactionClassModule); }
  InteractionClassModule::SecondList& getInteractionClassModuleList()
  { return _interactionClassModuleList; }

  void insert(ParameterDefinitionModule& parameterDefinitionModule)
  { _parameterDefinitionModuleList.push_back(parameterDefinitionModule); }
  ParameterDefinitionModule::SecondList& getParameterDefinitionModuleList()
  { return _parameterDefinitionModuleList; }

  // FIXME temporarily in this way
  DimensionHandleSet _dimensionHandleSet;

  void updateCumulativeSubscription(const ConnectHandle& connectHandle)
  {
    bool parentSubscribed = false;
    if (_parentInteractionClass &&
        0 != _parentInteractionClass->_cumulativeSubscribedConnectHandleSet.count(connectHandle))
      parentSubscribed = true;

    _updateCumulativeSubscription(connectHandle, parentSubscribed);
  }
  void _updateCumulativeSubscription(const ConnectHandle& connectHandle, bool subscribe /*Replace with regionset or something*/)
  {
    subscribe |= (Unsubscribed != getSubscriptionType(connectHandle));
    if (!updateCumulativeSubscribedConnectHandleSet(connectHandle, subscribe))
      return;
    // Update the receiving connect handle set
    for (ChildList::iterator i = _childInteractionClassList.begin(); i != _childInteractionClassList.end(); ++i) {
      i->_updateCumulativeSubscription(connectHandle, subscribe);
    }
  }

private:
  InteractionClass(const InteractionClass&);
  InteractionClass& operator=(const InteractionClass&);

  Federation& _federation;

  InteractionClassModule::SecondList _interactionClassModuleList;
  ParameterDefinitionModule::SecondList _parameterDefinitionModuleList;

  InteractionClass* _parentInteractionClass;
  ChildList _childInteractionClassList;

  OrderType _orderType;
  TransportationType _transportationType;

  ParameterDefinition::HandleMap _parameterHandleParameterDefinitionMap;
  ParameterDefinition::NameMap _parameterNameParameterMap;

  ClassParameter::HandleMap _parameterHandleClassParameterMap;
};

////////////////////////////////////////////////////////////

class Module;
class ObjectClass;

class OPENRTI_LOCAL ObjectClassModule : public ListPair<ObjectClassModule> {
public:
  ObjectClassModule(ObjectClass& objectClass, Module& module);
  ~ObjectClassModule();

  const ObjectClass& getObjectClass() const
  { return _objectClass; }
  ObjectClass& getObjectClass()
  { return _objectClass; }
  const Module& getModule() const
  { return _module; }
  Module& getModule()
  { return _module; }

private:
  ObjectClassModule(const ObjectClassModule&);
  ObjectClassModule& operator=(const ObjectClassModule&);

  ObjectClass& _objectClass;
  Module& _module;
};

////////////////////////////////////////////////////////////

class Module;
class ObjectClass;

class OPENRTI_LOCAL AttributeDefinitionModule : public ListPair<AttributeDefinitionModule> {
public:
  AttributeDefinitionModule(ObjectClass& objectClass, Module& module);
  ~AttributeDefinitionModule();

  const ObjectClass& getObjectClass() const
  { return _objectClass; }
  ObjectClass& getObjectClass()
  { return _objectClass; }
  const Module& getModule() const
  { return _module; }
  Module& getModule()
  { return _module; }

private:
  AttributeDefinitionModule(const AttributeDefinitionModule&);
  AttributeDefinitionModule& operator=(const AttributeDefinitionModule&);

  ObjectClass& _objectClass;
  Module& _module;
};

////////////////////////////////////////////////////////////

class AttributeDefinition;

class OPENRTI_LOCAL ClassAttribute : public HandleListEntity<ClassAttribute, AttributeHandle>,
                                     public PublishSubscribe /*FIXME*/ {
public:
  typedef HandleListEntity<ClassAttribute, AttributeHandle>::HandleMap HandleMap;
  typedef HandleListEntity<ClassAttribute, AttributeHandle>::FirstList FirstList;

  ClassAttribute(ObjectClass& objectClass, AttributeDefinition& attributeDefinition);
  ~ClassAttribute();

  const AttributeHandle& getAttributeHandle() const
  { return HandleListEntity<ClassAttribute, AttributeHandle>::_getHandle(); }
  void setAttributeHandle(const AttributeHandle& attributeHandle);

  const ObjectClass& getObjectClass() const
  { return _objectClass; }
  ObjectClass& getObjectClass()
  { return _objectClass; }

  const AttributeDefinition& getAttributeDefinition() const
  { return _attributeDefinition; }
  AttributeDefinition& getAttributeDefinition()
  { return _attributeDefinition; }

private:
  ClassAttribute(const ClassAttribute&);
  ClassAttribute& operator=(const ClassAttribute&);

  ObjectClass& _objectClass;
  AttributeDefinition& _attributeDefinition;

  // InstanceAttribute::FirstList _instanceAttributeList;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif
