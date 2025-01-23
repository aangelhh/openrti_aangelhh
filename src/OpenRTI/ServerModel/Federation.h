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

#ifndef OpenRTI_ServerModel_Federation_h
#define OpenRTI_ServerModel_Federation_h

#include "OpenRTIConfig.h"

#include "Intrusive.h"

#include "Handle.h"
#include "HandleAllocator.h"
#include "Message.h"
#include "Region.h"
#include "StringUtils.h"

namespace OpenRTI {
namespace ServerModel {

class Dimension;
class Federate;
class FederationConnect;
class InteractionClass;
class Module;
class Node;
class ObjectClass;
class ObjectInstance;
class Synchronization;
class UpdateRate;

class OPENRTI_LOCAL Federation :
    public Intrusive::UnorderedSetLink<Federation, Intrusive::ParentTag<Node> >,
    public Intrusive::UnorderedSetLink<Federation, Intrusive::ParentTag<Node, 1> >
{
public:
  Federation(Node& serverNode, FederationHandle const& federationHandle, std::string const& name);
  virtual ~Federation();

  /// The parent server node this belongs to
  Node const& getServerNode() const
  { return _serverNode; }
  Node& getServerNode()
  { return _serverNode; }

  /// The name of the Server as provided in the ServerOptions
  std::string const& getServerName() const;

  /// The path of the Server as provided in the ServerOptions
  std::string const& getServerPath() const;

  /// Returns true if the server node is a root server
  bool isRootServer() const;
  /// Returns true if this is the parents server connect
  bool isParentConnect(ConnectHandle const& connectHandle) const;
  bool hasChildConnects() const;
  bool hasChildConnect(ConnectHandle const& connectHandle);
  bool hasJoinedFederates() const;
  /// Returns true if there is any federate joined that belongs to a child connect
  bool hasJoinedChildren() /*const*/;

  /// The federation handle
  FederationHandle const& getFederationHandle() const
  { return _federationHandle; }

  /// The federation name
  std::string const& getName() const
  { return _name; }

  /// True if the federation name is registered in the Node
  bool getNameIsLinked() const
  { return Intrusive::UnorderedSetLink<Federation, Intrusive::ParentTag<Node, 1> >::is_linked(); }
  void setNameIsLinked(bool nameIsLinked);

  /// The name of the logical time factory
  std::string const& getLogicalTimeFactoryName() const
  { return _logicalTimeFactoryName; }
  void setLogicalTimeFactoryName(std::string const& logicalTimeFactoryName);

  /// UnorderedSet of FederationConnect instances indexed by connectHandle
  typedef Intrusive::UnorderedSet<ConnectHandle, Intrusive::UnorderedSetLink<FederationConnect, Intrusive::ParentTag<Federation> > > ConnectHandleFederationConnectMap;
  /// Get the set of FederationConnect instances
  ConnectHandleFederationConnectMap const& getConnectHandleFederationConnectMap() const
  { return _connectHandleFederationConnectMap; }
  ConnectHandleFederationConnectMap& getConnectHandleFederationConnectMap()
  { return _connectHandleFederationConnectMap; }
  /// Get one FederationConnect instance matching connectHandle
  FederationConnect const* getFederationConnect(ConnectHandle const& connectHandle) const;
  FederationConnect* getFederationConnect(ConnectHandle const& connectHandle);
  void removeConnect(ConnectHandle const& connectHandle);

  void send(ConnectHandle const& connectHandle, const SharedPtr<const AbstractMessage>& message);
  void send(FederateHandle const& federateHandle, const SharedPtr<const AbstractMessage>& message);
  void sendToParent(const SharedPtr<const AbstractMessage>& message);
  void broadcast(const SharedPtr<const AbstractMessage>& message);
  void broadcast(ConnectHandle const& connectHandle, const SharedPtr<const AbstractMessage>& message);
  void broadcastToChildren(const SharedPtr<const AbstractMessage>& message);
  void broadcastToChildren(ConnectHandle const& connectHandle, const SharedPtr<const AbstractMessage>& message);

  /// Links those FederationConnects that contain time regulating federates
  typedef Intrusive::List<Intrusive::ListLink<FederationConnect, Intrusive::ParentTag<Federation> > > TimeRegulatingFederationConnectList;
  /// Get the list of FederationConnect instances
  TimeRegulatingFederationConnectList const& getTimeRegulatingFederationConnectList() const
  { return _timeRegulatingFederationConnectList; }
  TimeRegulatingFederationConnectList& getTimeRegulatingFederationConnectList()
  { return _timeRegulatingFederationConnectList; }

  /// UnorderedSet of Module instances indexed by moduleHandle
  typedef Intrusive::UnorderedSet<ModuleHandle, Intrusive::UnorderedSetLink<Module, Intrusive::ParentTag<Federation> > > ModuleHandleModuleMap;
  /// Get the set of Module instances
  ModuleHandleModuleMap const& getModuleHandleModuleMap() const
  { return _moduleHandleModuleMap; }
  ModuleHandleModuleMap& getModuleHandleModuleMap()
  { return _moduleHandleModuleMap; }
  /// Get one Module instance matching moduleHandle
  Module const* getModule(ModuleHandle const& moduleHandle) const;
  Module* getModule(ModuleHandle const& moduleHandle);

  OrderType resolveOrderType(std::string const& orderType);
  TransportationType resolveTransportationType(std::string const& transportationType);

  /// UnorderedSet of Dimension instances indexed by dimensionHandle
  typedef Intrusive::UnorderedSet<DimensionHandle, Intrusive::UnorderedSetLink<Dimension, Intrusive::ParentTag<Federation> > > DimensionHandleDimensionMap;
  /// Get the set of Dimension instances
  DimensionHandleDimensionMap const& getDimensionHandleDimensionMap() const
  { return _dimensionHandleDimensionMap; }
  DimensionHandleDimensionMap& getDimensionHandleDimensionMap()
  { return _dimensionHandleDimensionMap; }
  /// Get one Dimension instance matching dimensionHandle
  Dimension const* getDimension(DimensionHandle const& dimensionHandle) const;
  Dimension* getDimension(DimensionHandle const& dimensionHandle);

  /// UnorderedSet of Dimension instances indexed by name
  typedef Intrusive::UnorderedSet<std::string, Intrusive::UnorderedSetLink<Dimension, Intrusive::ParentTag<Federation, 1> > > DimensionNameDimensionMap;
  /// Get one Dimension instance matching name
  Dimension const* getDimension(std::string const& name) const;
  Dimension* getDimension(std::string const& name);
  Dimension* resolveDimension(std::string const& dimensionName)
  { return getDimension(dimensionName); }

  /// UnorderedSet of UpdateRate instances indexed by updateRateHandle
  typedef Intrusive::UnorderedSet<UpdateRateHandle, Intrusive::UnorderedSetLink<UpdateRate, Intrusive::ParentTag<Federation> > > UpdateRateHandleUpdateRateMap;
  /// Get the set of UpdateRate instances
  UpdateRateHandleUpdateRateMap const& getUpdateRateHandleUpdateRateMap() const
  { return _updateRateHandleUpdateRateMap; }
  UpdateRateHandleUpdateRateMap& getUpdateRateHandleUpdateRateMap()
  { return _updateRateHandleUpdateRateMap; }
  /// Get one UpdateRate instance matching updateRateHandle
  UpdateRate const* getUpdateRate(UpdateRateHandle const& updateRateHandle) const;
  UpdateRate* getUpdateRate(UpdateRateHandle const& updateRateHandle);

  /// UnorderedSet of UpdateRate instances indexed by name
  typedef Intrusive::UnorderedSet<std::string, Intrusive::UnorderedSetLink<UpdateRate, Intrusive::ParentTag<Federation, 1> > > UpdateRateNameUpdateRateMap;
  /// Get one UpdateRate instance matching name
  UpdateRate const* getUpdateRate(std::string const& name) const;
  UpdateRate* getUpdateRate(std::string const& name);
  UpdateRate* resolveUpdateRate(std::string const& updateRateName)
  { return getUpdateRate(updateRateName); }

  /// UnorderedSet of InteractionClass instances indexed by interactionClassHandle
  typedef Intrusive::UnorderedSet<InteractionClassHandle, Intrusive::UnorderedSetLink<InteractionClass, Intrusive::ParentTag<Federation> > > InteractionClassHandleInteractionClassMap;
  /// Get the set of InteractionClass instances
  InteractionClassHandleInteractionClassMap const& getInteractionClassHandleInteractionClassMap() const
  { return _interactionClassHandleInteractionClassMap; }
  InteractionClassHandleInteractionClassMap& getInteractionClassHandleInteractionClassMap()
  { return _interactionClassHandleInteractionClassMap; }
  /// Get one InteractionClass instance matching interactionClassHandle
  InteractionClass const* getInteractionClass(InteractionClassHandle const& interactionClassHandle) const;
  InteractionClass* getInteractionClass(InteractionClassHandle const& interactionClassHandle);

  /// UnorderedSet of InteractionClass instances indexed by name
  typedef Intrusive::UnorderedSet<StringVector, Intrusive::UnorderedSetLink<InteractionClass, Intrusive::ParentTag<Federation, 1> > > InteractionClassNameInteractionClassMap;
  /// Get one InteractionClass instance matching name
  InteractionClass const* getInteractionClass(StringVector const& name) const;
  InteractionClass* getInteractionClass(StringVector const& name);
  InteractionClass* resolveParentInteractionClass(StringVector const& interactionClassName);

  /// UnorderedSet of ObjectClass instances indexed by objectClassHandle
  typedef Intrusive::UnorderedSet<ObjectClassHandle, Intrusive::UnorderedSetLink<ObjectClass, Intrusive::ParentTag<Federation> > > ObjectClassHandleObjectClassMap;
  /// Get the set of ObjectClass instances
  ObjectClassHandleObjectClassMap const& getObjectClassHandleObjectClassMap() const
  { return _objectClassHandleObjectClassMap; }
  ObjectClassHandleObjectClassMap& getObjectClassHandleObjectClassMap()
  { return _objectClassHandleObjectClassMap; }
  /// Get one ObjectClass instance matching objectClassHandle
  ObjectClass const* getObjectClass(ObjectClassHandle const& objectClassHandle) const;
  ObjectClass* getObjectClass(ObjectClassHandle const& objectClassHandle);

  /// UnorderedSet of ObjectClass instances indexed by name
  typedef Intrusive::UnorderedSet<StringVector, Intrusive::UnorderedSetLink<ObjectClass, Intrusive::ParentTag<Federation, 1> > > ObjectClassNameObjectClassMap;
  /// Get one ObjectClass instance matching name
  ObjectClass const* getObjectClass(StringVector const& name) const;
  ObjectClass* getObjectClass(StringVector const& name);
  ObjectClass* resolveParentObjectClass(StringVector const& objectClassName);

  /// Either insert a new entity or checks if the provided entity is
  /// compatible with the existing one. Returns true if the entity was newly created.
  bool insertOrCheck(Module& module, FOMStringDimension const& stringDimension);
  bool insertOrCheck(Module& module, FOMStringUpdateRate const& stringUpdateRate);
  bool insertOrCheck(Module& module, FOMStringInteractionClass const& stringInteractionClass);
  bool insertOrCheck(Module& module, FOMStringObjectClass const& stringObjectClass);

  /// This is for inserting the initial object model
  ModuleHandle insert(FOMStringModule const& stringModule);
  void insert(FOMStringModuleList const& stringModuleList);
  void insert(ModuleHandleVector& moduleHandleVector, FOMStringModuleList const& stringModuleList);

  /// Either insert a new entity or creates a new one.
  /// Throws a message error if an existing one does not match the provided.
  void insert(Module& module, FOMDimension const& fomDimension);
  void insert(Module& module, FOMUpdateRate const& fomUpdateRate);
  void insert(Module& module, FOMInteractionClass const& fomInteractionClass);
  void insert(Module& module, FOMObjectClass const& fomObjectClass);

  void insert(FOMModule const& fomModule);
  void insert(FOMModuleList const& fomModuleList);
  void erase(ModuleHandle const& moduleHandle);
  void erase(Module& module);

  /// To push this to other server nodes, we need to collect the module data for the message.
  void getModuleList(FOMModuleList& moduleList) const;
  void getModuleList(FOMModuleList& moduleList, ModuleHandleVector const& moduleHandleVector) const;

  /// UnorderedSet of Synchronization instances indexed by label
  typedef Intrusive::UnorderedSet<std::string, Intrusive::UnorderedSetLink<Synchronization, Intrusive::ParentTag<Federation> > > SynchronizationNameSynchronizationMap;
  /// Get the set of Synchronization instances
  SynchronizationNameSynchronizationMap const& getSynchronizationNameSynchronizationMap() const
  { return _synchronizationNameSynchronizationMap; }
  SynchronizationNameSynchronizationMap& getSynchronizationNameSynchronizationMap()
  { return _synchronizationNameSynchronizationMap; }
  /// Get one Synchronization instance matching label
  Synchronization const* getSynchronization(std::string const& label) const;
  Synchronization* getSynchronization(std::string const& label);

  /// UnorderedSet of Federate instances indexed by federateHandle
  typedef Intrusive::UnorderedSet<FederateHandle, Intrusive::UnorderedSetLink<Federate, Intrusive::ParentTag<Federation> > > FederateHandleFederateMap;
  /// Get the set of Federate instances
  FederateHandleFederateMap const& getFederateHandleFederateMap() const
  { return _federateHandleFederateMap; }
  FederateHandleFederateMap& getFederateHandleFederateMap()
  { return _federateHandleFederateMap; }
  /// Get one Federate instance matching federateHandle
  Federate const* getFederate(FederateHandle const& federateHandle) const;
  Federate* getFederate(FederateHandle const& federateHandle);

  /// UnorderedSet of Federate instances indexed by name
  typedef Intrusive::UnorderedSet<std::string, Intrusive::UnorderedSetLink<Federate, Intrusive::ParentTag<Federation, 1> > > FederateNameFederateMap;
  /// Get one Federate instance matching name
  Federate const* getFederate(std::string const& name) const;
  Federate* getFederate(std::string const& name);

  bool isFederateNameInUse(std::string const& name) const;

  void erase(Federate& federate);

  /// FIXME simplify region communication, only commit and erase is needed.
  Region* getOrCreateRegion(RegionHandle const& regionHandle);
  Region* getRegion(RegionHandle const& regionHandle);

  /// UnorderedSet of ObjectInstance instances indexed by objectInstanceHandle
  typedef Intrusive::UnorderedSet<ObjectInstanceHandle, Intrusive::UnorderedSetLink<ObjectInstance, Intrusive::ParentTag<Federation> > > ObjectInstanceHandleObjectInstanceMap;
  /// Get the set of ObjectInstance instances
  ObjectInstanceHandleObjectInstanceMap const& getObjectInstanceHandleObjectInstanceMap() const
  { return _objectInstanceHandleObjectInstanceMap; }
  ObjectInstanceHandleObjectInstanceMap& getObjectInstanceHandleObjectInstanceMap()
  { return _objectInstanceHandleObjectInstanceMap; }
  /// Get one ObjectInstance instance matching objectInstanceHandle
  ObjectInstance const* getObjectInstance(ObjectInstanceHandle const& objectInstanceHandle) const;
  ObjectInstance* getObjectInstance(ObjectInstanceHandle const& objectInstanceHandle);

  /// UnorderedSet of ObjectInstance instances indexed by name
  typedef Intrusive::UnorderedSet<std::string, Intrusive::UnorderedSetLink<ObjectInstance, Intrusive::ParentTag<Federation, 1> > > ObjectInstanceNameObjectInstanceMap;
  /// Get one ObjectInstance instance matching name
  ObjectInstance const* getObjectInstance(std::string const& name) const;
  ObjectInstance* getObjectInstance(std::string const& name);
  void erase(ObjectInstance& objectInstance);
  bool isObjectInstanceNameInUse(std::string const& name) const;

  ///
  /// Create a new Module instance
  Module* createModule(ModuleHandle const& moduleHandle);

  ///
  /// Create a new Dimension instance
  Dimension* createDimension(DimensionHandle const& dimensionHandle, std::string const& name);

  ///
  /// Create a new UpdateRate instance
  UpdateRate* createUpdateRate(UpdateRateHandle const& updateRateHandle, std::string const& name);

  ///
  /// Create a new Federate instance
  Federate* createFederate(FederateHandle const& federateHandle, std::string const& name);

  ///
  /// Create a new ObjectInstance instance
  ObjectInstance* createObjectInstance(ObjectInstanceHandle const& objectInstanceHandle, std::string const& name);

  template<typename Link>
  struct IntrusiveKey;

private:
#if 201103L <= __cplusplus
  Federation(Federation const&) = delete;
  Federation(Federation&&) = delete;
  Federation& operator=(Federation const&) = delete;
  Federation& operator=(Federation&&) = delete;
#else
  Federation(Federation const&);
  Federation& operator=(Federation const&);
#if 200610L <= __cpp_rvalue_reference
  Federation(Federation&&);
  Federation& operator=(Federation&&);
#endif
#endif

  /// The parent server node this belongs to
  Node& _serverNode;

  /// The federation handle
  FederationHandle const _federationHandle;

  /// The federation name
  std::string const _name;

  /// The name of the logical time factory
  std::string _logicalTimeFactoryName;

  /// UnorderedSet of FederationConnect instances indexed by connectHandle
  /// For almost everything a list would have been sufficient.
  /// But inserting a federate into a NodeConnect requires to know if the
  /// federation is already known at this connect and we do this by asking
  /// the federation for a valid connect for a given connect handle.
  /// Here a fast index keyed by connect handle helps a lot, so we have a map here.
  friend class FederationConnect;
  /// Insert federationConnect into connectHandleFederationConnectMap
  void _insertConnectHandleFederationConnectMap(FederationConnect& federationConnect);
  /// Unlink federationConnect from connectHandleFederationConnectMap
  void _unlinkConnectHandleFederationConnectMap(FederationConnect& federationConnect);
  ConnectHandleFederationConnectMap _connectHandleFederationConnectMap;

  /// Links those FederationConnects that contain time regulating federates
  /// Insert federationConnect into timeRegulatingFederationConnectList
  void _insertTimeRegulatingFederationConnectList(FederationConnect& federationConnect);
  /// Unlink federationConnect from timeRegulatingFederationConnectList
  void _unlinkTimeRegulatingFederationConnectList(FederationConnect& federationConnect);
  TimeRegulatingFederationConnectList _timeRegulatingFederationConnectList;

  /// UnorderedSet of Module instances indexed by moduleHandle
  friend class Module;
  /// Insert module into moduleHandleModuleMap
  void _insertModuleHandleModuleMap(Module& module);
  /// Unlink module from moduleHandleModuleMap
  void _unlinkModuleHandleModuleMap(Module& module);
  ModuleHandleModuleMap _moduleHandleModuleMap;

  /// The appropriate HandleAllocator
  HandleAllocator<ModuleHandle> _moduleHandleAllocator;

  /// FIXME, take these to map any name type string combination to one of the two fixed backend types
  /// OrderTypeHandleOrderTypeMap _orderTypeHandleOrderTypeMap;
  /// OrderTypeNameOrderTypeMap _orderTypeNameOrderTypeMap;
  /// HandleAllocator<OrderTypeHandle> _orderTypeHandleAllocator;

  /// FIXME, take these to map any name type string combination to one of the two fixed backend types
  /// TransportationTypeHandleTransportationTypeMap _transportationTypeHandleTransportationTypeMap;
  /// TransportationTypeNameTransportationTypeMap _transportationTypeNameTransportationTypeMap;
  /// HandleAllocator<TransportationTypeHandle> _transportationTypeHandleAllocator;

  /// UnorderedSet of Dimension instances indexed by dimensionHandle
  friend class Dimension;
  /// Insert dimension into dimensionHandleDimensionMap
  void _insertDimensionHandleDimensionMap(Dimension& dimension);
  /// Unlink dimension from dimensionHandleDimensionMap
  void _unlinkDimensionHandleDimensionMap(Dimension& dimension);
  DimensionHandleDimensionMap _dimensionHandleDimensionMap;

  /// The appropriate HandleAllocator
  HandleAllocator<DimensionHandle> _dimensionHandleAllocator;

  /// UnorderedSet of Dimension instances indexed by name
  /// Insert dimension into dimensionNameDimensionMap
  void _insertDimensionNameDimensionMap(Dimension& dimension);
  /// Unlink dimension from dimensionNameDimensionMap
  void _unlinkDimensionNameDimensionMap(Dimension& dimension);
  DimensionNameDimensionMap _dimensionNameDimensionMap;

  /// UnorderedSet of UpdateRate instances indexed by updateRateHandle
  friend class UpdateRate;
  /// Insert updateRate into updateRateHandleUpdateRateMap
  void _insertUpdateRateHandleUpdateRateMap(UpdateRate& updateRate);
  /// Unlink updateRate from updateRateHandleUpdateRateMap
  void _unlinkUpdateRateHandleUpdateRateMap(UpdateRate& updateRate);
  UpdateRateHandleUpdateRateMap _updateRateHandleUpdateRateMap;

  /// The appropriate HandleAllocator
  HandleAllocator<UpdateRateHandle> _updateRateHandleAllocator;

  /// UnorderedSet of UpdateRate instances indexed by name
  /// Insert updateRate into updateRateNameUpdateRateMap
  void _insertUpdateRateNameUpdateRateMap(UpdateRate& updateRate);
  /// Unlink updateRate from updateRateNameUpdateRateMap
  void _unlinkUpdateRateNameUpdateRateMap(UpdateRate& updateRate);
  UpdateRateNameUpdateRateMap _updateRateNameUpdateRateMap;

  /// UnorderedSet of InteractionClass instances indexed by interactionClassHandle
  friend class InteractionClass;
  /// Insert interactionClass into interactionClassHandleInteractionClassMap
  void _insertInteractionClassHandleInteractionClassMap(InteractionClass& interactionClass);
  /// Unlink interactionClass from interactionClassHandleInteractionClassMap
  void _unlinkInteractionClassHandleInteractionClassMap(InteractionClass& interactionClass);
  InteractionClassHandleInteractionClassMap _interactionClassHandleInteractionClassMap;

  /// The appropriate HandleAllocator
  HandleAllocator<InteractionClassHandle> _interactionClassHandleAllocator;

  /// UnorderedSet of InteractionClass instances indexed by name
  /// Insert interactionClass into interactionClassNameInteractionClassMap
  void _insertInteractionClassNameInteractionClassMap(InteractionClass& interactionClass);
  /// Unlink interactionClass from interactionClassNameInteractionClassMap
  void _unlinkInteractionClassNameInteractionClassMap(InteractionClass& interactionClass);
  InteractionClassNameInteractionClassMap _interactionClassNameInteractionClassMap;

  /// UnorderedSet of ObjectClass instances indexed by objectClassHandle
  friend class ObjectClass;
  /// Insert objectClass into objectClassHandleObjectClassMap
  void _insertObjectClassHandleObjectClassMap(ObjectClass& objectClass);
  /// Unlink objectClass from objectClassHandleObjectClassMap
  void _unlinkObjectClassHandleObjectClassMap(ObjectClass& objectClass);
  ObjectClassHandleObjectClassMap _objectClassHandleObjectClassMap;

  /// The appropriate HandleAllocator
  HandleAllocator<ObjectClassHandle> _objectClassHandleAllocator;

  /// UnorderedSet of ObjectClass instances indexed by name
  /// Insert objectClass into objectClassNameObjectClassMap
  void _insertObjectClassNameObjectClassMap(ObjectClass& objectClass);
  /// Unlink objectClass from objectClassNameObjectClassMap
  void _unlinkObjectClassNameObjectClassMap(ObjectClass& objectClass);
  ObjectClassNameObjectClassMap _objectClassNameObjectClassMap;

  /// UnorderedSet of Synchronization instances indexed by label
  friend class Synchronization;
  /// Insert synchronization into synchronizationNameSynchronizationMap
  void _insertSynchronizationNameSynchronizationMap(Synchronization& synchronization);
  /// Unlink synchronization from synchronizationNameSynchronizationMap
  void _unlinkSynchronizationNameSynchronizationMap(Synchronization& synchronization);
  SynchronizationNameSynchronizationMap _synchronizationNameSynchronizationMap;

  /// UnorderedSet of Federate instances indexed by federateHandle
  friend class Federate;
  /// Insert federate into federateHandleFederateMap
  void _insertFederateHandleFederateMap(Federate& federate);
  /// Unlink federate from federateHandleFederateMap
  void _unlinkFederateHandleFederateMap(Federate& federate);
  FederateHandleFederateMap _federateHandleFederateMap;

  /// The appropriate HandleAllocator
  HandleAllocator<FederateHandle> _federateHandleAllocator;

  /// UnorderedSet of Federate instances indexed by name
  /// Insert federate into federateNameFederateMap
  void _insertFederateNameFederateMap(Federate& federate);
  /// Unlink federate from federateNameFederateMap
  void _unlinkFederateNameFederateMap(Federate& federate);
  FederateNameFederateMap _federateNameFederateMap;

  /// UnorderedSet of ObjectInstance instances indexed by objectInstanceHandle
  friend class ObjectInstance;
  /// Insert objectInstance into objectInstanceHandleObjectInstanceMap
  void _insertObjectInstanceHandleObjectInstanceMap(ObjectInstance& objectInstance);
  /// Unlink objectInstance from objectInstanceHandleObjectInstanceMap
  void _unlinkObjectInstanceHandleObjectInstanceMap(ObjectInstance& objectInstance);
  ObjectInstanceHandleObjectInstanceMap _objectInstanceHandleObjectInstanceMap;

  /// The appropriate HandleAllocator
  HandleAllocator<ObjectInstanceHandle> _objectInstanceHandleAllocator;

  /// UnorderedSet of ObjectInstance instances indexed by name
  /// Insert objectInstance into objectInstanceNameObjectInstanceMap
  void _insertObjectInstanceNameObjectInstanceMap(ObjectInstance& objectInstance);
  /// Unlink objectInstance from objectInstanceNameObjectInstanceMap
  void _unlinkObjectInstanceNameObjectInstanceMap(ObjectInstance& objectInstance);
  ObjectInstanceNameObjectInstanceMap _objectInstanceNameObjectInstanceMap;
};

template<>
struct Federation::IntrusiveKey<Intrusive::UnorderedSetLink<Federation, Intrusive::ParentTag<Node> > > {
  static FederationHandle const& get(Federation const& federation)
  { return federation.getFederationHandle(); }
};

template<>
struct Federation::IntrusiveKey<Intrusive::UnorderedSetLink<Federation, Intrusive::ParentTag<Node, 1> > > {
  static std::string const& get(Federation const& federation)
  { return federation.getName(); }
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_Federation_h
