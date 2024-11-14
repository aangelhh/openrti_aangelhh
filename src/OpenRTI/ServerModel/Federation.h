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

#ifndef OpenRTI_ServerModel_Federation_h
#define OpenRTI_ServerModel_Federation_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

#include "FederationConnect.h"
#include "Handle.h"
#include "HandleAllocator.h"
#include "InteractionClass.h"
#include "Module.h"
#include "ObjectClass.h"
#include "ServerModel.h"
#include "StringUtils.h"
#include "UpdateRate.h"

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
class UpdateRate;

class OPENRTI_LOCAL Federation :
    public IntrusiveUnorderedMap<FederationHandle, Federation>::Hook,
    public IntrusiveUnorderedMap<std::string, Federation>::Hook
{
public:
  typedef IntrusiveUnorderedMap<FederationHandle, Federation> HandleMap;
  typedef IntrusiveUnorderedMap<std::string, Federation> NameMap;

  Federation(Node& serverNode);
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
  { return IntrusiveUnorderedMap<FederationHandle, Federation>::Hook::getKey(); }
  void setFederationHandle(FederationHandle const& federationHandle);

  /// The federation name
  std::string const& getName() const
  { return IntrusiveUnorderedMap<std::string, Federation>::Hook::getKey(); }
  void setName(std::string const& name);

  /// The name of the logical time factory
  std::string const& getLogicalTimeFactoryName() const
  { return _logicalTimeFactoryName; }
  void setLogicalTimeFactoryName(std::string const& logicalTimeFactoryName);

  /// UnorderedSet of FederationConnect instances indexed by connectHandle
  typedef IntrusiveUnorderedMap<ConnectHandle, FederationConnect> ConnectHandleFederationConnectMap;
  /// Get the set of FederationConnect instances
  ConnectHandleFederationConnectMap const& getConnectHandleFederationConnectMap() const
  { return _connectHandleFederationConnectMap; }
  ConnectHandleFederationConnectMap& getConnectHandleFederationConnectMap()
  { return _connectHandleFederationConnectMap; }
  /// Get one FederationConnect instance matching connectHandle
  FederationConnect const* getFederationConnect(ConnectHandle const& connectHandle) const;
  FederationConnect* getFederationConnect(ConnectHandle const& connectHandle);
  void insert(FederationConnect& federationConnect)
  { _connectHandleFederationConnectMap.insert(federationConnect); }
  void removeConnect(ConnectHandle const& connectHandle);

  void send(ConnectHandle const& connectHandle, const SharedPtr<const AbstractMessage>& message);
  void send(FederateHandle const& federateHandle, const SharedPtr<const AbstractMessage>& message);
  void sendToParent(const SharedPtr<const AbstractMessage>& message);
  void broadcast(const SharedPtr<const AbstractMessage>& message);
  void broadcast(ConnectHandle const& connectHandle, const SharedPtr<const AbstractMessage>& message);
  void broadcastToChildren(const SharedPtr<const AbstractMessage>& message);
  void broadcastToChildren(ConnectHandle const& connectHandle, const SharedPtr<const AbstractMessage>& message);

  /// Links those FederationConnects that contain time regulating federates
  typedef IntrusiveList<FederationConnect, 1> TimeRegulatingFederationConnectList;
  /// Get the list of FederationConnect instances
  TimeRegulatingFederationConnectList const& getTimeRegulatingFederationConnectList() const
  { return _timeRegulatingFederationConnectList; }
  TimeRegulatingFederationConnectList& getTimeRegulatingFederationConnectList()
  { return _timeRegulatingFederationConnectList; }
  void insertTimeRegulating(Federate& federate);
  void eraseTimeRegulating(Federate& federate);

  /// UnorderedSet of Module instances indexed by moduleHandle
  typedef IntrusiveUnorderedMap<ModuleHandle, Module> ModuleHandleModuleMap;
  /// Get the set of Module instances
  ModuleHandleModuleMap const& getModuleHandleModuleMap() const
  { return _moduleHandleModuleMap; }
  ModuleHandleModuleMap& getModuleHandleModuleMap()
  { return _moduleHandleModuleMap; }
  /// Get one Module instance matching moduleHandle
  Module const* getModule(ModuleHandle const& moduleHandle) const;
  Module* getModule(ModuleHandle const& moduleHandle);
  void insert(Module& module);

  OrderType resolveOrderType(std::string const& orderType);
  TransportationType resolveTransportationType(std::string const& transportationType);

  /// UnorderedSet of Dimension instances indexed by dimensionHandle
  typedef IntrusiveUnorderedMap<DimensionHandle, Dimension> DimensionHandleDimensionMap;
  /// Get the set of Dimension instances
  DimensionHandleDimensionMap const& getDimensionHandleDimensionMap() const
  { return _dimensionHandleDimensionMap; }
  DimensionHandleDimensionMap& getDimensionHandleDimensionMap()
  { return _dimensionHandleDimensionMap; }
  /// Get one Dimension instance matching dimensionHandle
  Dimension const* getDimension(DimensionHandle const& dimensionHandle) const;
  Dimension* getDimension(DimensionHandle const& dimensionHandle);

  /// UnorderedSet of Dimension instances indexed by name
  typedef IntrusiveUnorderedMap<std::string, Dimension> DimensionNameDimensionMap;
  /// Get one Dimension instance matching name
  Dimension const* getDimension(std::string const& name) const;
  Dimension* getDimension(std::string const& name);
  Dimension* resolveDimension(std::string const& dimensionName)
  { return getDimension(dimensionName); }
  void insert(Dimension& dimension);

  /// UnorderedSet of UpdateRate instances indexed by updateRateHandle
  typedef IntrusiveUnorderedMap<UpdateRateHandle, UpdateRate> UpdateRateHandleUpdateRateMap;
  /// Get the set of UpdateRate instances
  UpdateRateHandleUpdateRateMap const& getUpdateRateHandleUpdateRateMap() const
  { return _updateRateHandleUpdateRateMap; }
  UpdateRateHandleUpdateRateMap& getUpdateRateHandleUpdateRateMap()
  { return _updateRateHandleUpdateRateMap; }
  /// Get one UpdateRate instance matching updateRateHandle
  UpdateRate const* getUpdateRate(UpdateRateHandle const& updateRateHandle) const;
  UpdateRate* getUpdateRate(UpdateRateHandle const& updateRateHandle);

  /// UnorderedSet of UpdateRate instances indexed by name
  typedef IntrusiveUnorderedMap<std::string, UpdateRate> UpdateRateNameUpdateRateMap;
  /// Get one UpdateRate instance matching name
  UpdateRate const* getUpdateRate(std::string const& name) const;
  UpdateRate* getUpdateRate(std::string const& name);
  UpdateRate* resolveUpdateRate(std::string const& updateRateName)
  { return getUpdateRate(updateRateName); }
  void insert(UpdateRate& updateRate);

  /// UnorderedSet of InteractionClass instances indexed by interactionClassHandle
  typedef IntrusiveUnorderedMap<InteractionClassHandle, InteractionClass> InteractionClassHandleInteractionClassMap;
  /// Get the set of InteractionClass instances
  InteractionClassHandleInteractionClassMap const& getInteractionClassHandleInteractionClassMap() const
  { return _interactionClassHandleInteractionClassMap; }
  InteractionClassHandleInteractionClassMap& getInteractionClassHandleInteractionClassMap()
  { return _interactionClassHandleInteractionClassMap; }
  /// Get one InteractionClass instance matching interactionClassHandle
  InteractionClass const* getInteractionClass(InteractionClassHandle const& interactionClassHandle) const;
  InteractionClass* getInteractionClass(InteractionClassHandle const& interactionClassHandle);

  /// UnorderedSet of InteractionClass instances indexed by name
  typedef IntrusiveUnorderedMap<StringVector, InteractionClass> InteractionClassNameInteractionClassMap;
  /// Get one InteractionClass instance matching name
  InteractionClass const* getInteractionClass(StringVector const& name) const;
  InteractionClass* getInteractionClass(StringVector const& name);
  void insert(InteractionClass& interactionClass);
  InteractionClass* resolveParentInteractionClass(StringVector const& interactionClassName);

  /// UnorderedSet of ObjectClass instances indexed by objectClassHandle
  typedef IntrusiveUnorderedMap<ObjectClassHandle, ObjectClass> ObjectClassHandleObjectClassMap;
  /// Get the set of ObjectClass instances
  ObjectClassHandleObjectClassMap const& getObjectClassHandleObjectClassMap() const
  { return _objectClassHandleObjectClassMap; }
  ObjectClassHandleObjectClassMap& getObjectClassHandleObjectClassMap()
  { return _objectClassHandleObjectClassMap; }
  /// Get one ObjectClass instance matching objectClassHandle
  ObjectClass const* getObjectClass(ObjectClassHandle const& objectClassHandle) const;
  ObjectClass* getObjectClass(ObjectClassHandle const& objectClassHandle);

  /// UnorderedSet of ObjectClass instances indexed by name
  typedef IntrusiveUnorderedMap<StringVector, ObjectClass> ObjectClassNameObjectClassMap;
  /// Get one ObjectClass instance matching name
  ObjectClass const* getObjectClass(StringVector const& name) const;
  ObjectClass* getObjectClass(StringVector const& name);
  void insert(ObjectClass& objectClass);
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

  /// Synchronization state FIXME
  /// UnorderedSet of Synchronization instances indexed by label
  typedef IntrusiveUnorderedMap<std::string, Synchronization> SynchronizationNameSynchronizationMap;
  SynchronizationNameSynchronizationMap _synchronizationNameSynchronizationMap;

  /// UnorderedSet of Federate instances indexed by federateHandle
  typedef IntrusiveUnorderedMap<FederateHandle, Federate> FederateHandleFederateMap;
  /// Get the set of Federate instances
  FederateHandleFederateMap const& getFederateHandleFederateMap() const
  { return _federateHandleFederateMap; }
  FederateHandleFederateMap& getFederateHandleFederateMap()
  { return _federateHandleFederateMap; }
  /// Get one Federate instance matching federateHandle
  Federate const* getFederate(FederateHandle const& federateHandle) const;
  Federate* getFederate(FederateHandle const& federateHandle);

  /// UnorderedSet of Federate instances indexed by name
  typedef IntrusiveUnorderedMap<std::string, Federate> FederateNameFederateMap;
  /// Get one Federate instance matching name
  Federate const* getFederate(std::string const& name) const;
  Federate* getFederate(std::string const& name);

  bool isFederateNameInUse(std::string const& name) const;

  void insert(Federate& federate);
  void erase(FederateHandle const& federateHandle);
  void erase(Federate& federate);

  /// FIXME simplify region communication, only commit and erase is needed.
  Region* getOrCreateRegion(RegionHandle const& regionHandle);
  Region* getRegion(RegionHandle const& regionHandle);

  /// UnorderedSet of ObjectInstance instances indexed by objectInstanceHandle
  typedef IntrusiveUnorderedMap<ObjectInstanceHandle, ObjectInstance> ObjectInstanceHandleObjectInstanceMap;
  /// Get the set of ObjectInstance instances
  ObjectInstanceHandleObjectInstanceMap const& getObjectInstanceHandleObjectInstanceMap() const
  { return _objectInstanceHandleObjectInstanceMap; }
  ObjectInstanceHandleObjectInstanceMap& getObjectInstanceHandleObjectInstanceMap()
  { return _objectInstanceHandleObjectInstanceMap; }
  /// Get one ObjectInstance instance matching objectInstanceHandle
  ObjectInstance const* getObjectInstance(ObjectInstanceHandle const& objectInstanceHandle) const;
  ObjectInstance* getObjectInstance(ObjectInstanceHandle const& objectInstanceHandle);

  /// UnorderedSet of ObjectInstance instances indexed by name
  typedef IntrusiveUnorderedMap<std::string, ObjectInstance> ObjectInstanceNameObjectInstanceMap;
  /// Get one ObjectInstance instance matching name
  ObjectInstance const* getObjectInstance(std::string const& name) const;
  ObjectInstance* getObjectInstance(std::string const& name);
  void insert(ObjectInstance& objectInstance);
  void erase(ObjectInstance& objectInstance);
  bool isObjectInstanceNameInUse(std::string const& name) const;
  /// FIXME
  ObjectInstance* insertObjectInstance(ObjectInstanceHandle const& objectInstanceHandle, std::string const& objectInstanceName);

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

  /// The name of the logical time factory
  std::string _logicalTimeFactoryName;

  /// UnorderedSet of FederationConnect instances indexed by connectHandle
  /// For almost everything a list would have been sufficient.
  /// But inserting a federate into a NodeConnect requires to know if the
  /// federation is already known at this connect and we do this by asking
  /// the federation for a valid connect for a given connect handle.
  /// Here a fast index keyed by connect handle helps a lot, so we have a map here.
  ConnectHandleFederationConnectMap _connectHandleFederationConnectMap;

  /// Links those FederationConnects that contain time regulating federates
  TimeRegulatingFederationConnectList _timeRegulatingFederationConnectList;

  /// UnorderedSet of Module instances indexed by moduleHandle
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
  DimensionHandleDimensionMap _dimensionHandleDimensionMap;

  /// The appropriate HandleAllocator
  HandleAllocator<DimensionHandle> _dimensionHandleAllocator;

  /// UnorderedSet of Dimension instances indexed by name
  DimensionNameDimensionMap _dimensionNameDimensionMap;

  /// UnorderedSet of UpdateRate instances indexed by updateRateHandle
  UpdateRateHandleUpdateRateMap _updateRateHandleUpdateRateMap;

  /// The appropriate HandleAllocator
  HandleAllocator<UpdateRateHandle> _updateRateHandleAllocator;

  /// UnorderedSet of UpdateRate instances indexed by name
  UpdateRateNameUpdateRateMap _updateRateNameUpdateRateMap;

  /// UnorderedSet of InteractionClass instances indexed by interactionClassHandle
  InteractionClassHandleInteractionClassMap _interactionClassHandleInteractionClassMap;

  /// The appropriate HandleAllocator
  HandleAllocator<InteractionClassHandle> _interactionClassHandleAllocator;

  /// UnorderedSet of InteractionClass instances indexed by name
  InteractionClassNameInteractionClassMap _interactionClassNameInteractionClassMap;

  /// UnorderedSet of ObjectClass instances indexed by objectClassHandle
  ObjectClassHandleObjectClassMap _objectClassHandleObjectClassMap;

  /// The appropriate HandleAllocator
  HandleAllocator<ObjectClassHandle> _objectClassHandleAllocator;

  /// UnorderedSet of ObjectClass instances indexed by name
  ObjectClassNameObjectClassMap _objectClassNameObjectClassMap;

  /// UnorderedSet of Federate instances indexed by federateHandle
  FederateHandleFederateMap _federateHandleFederateMap;

  /// The appropriate HandleAllocator
  HandleAllocator<FederateHandle> _federateHandleAllocator;

  /// UnorderedSet of Federate instances indexed by name
  FederateNameFederateMap _federateNameFederateMap;

  /// UnorderedSet of ObjectInstance instances indexed by objectInstanceHandle
  ObjectInstanceHandleObjectInstanceMap _objectInstanceHandleObjectInstanceMap;

  /// The appropriate HandleAllocator
  HandleAllocator<ObjectInstanceHandle> _objectInstanceHandleAllocator;

  /// UnorderedSet of ObjectInstance instances indexed by name
  ObjectInstanceNameObjectInstanceMap _objectInstanceNameObjectInstanceMap;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_Federation_h
