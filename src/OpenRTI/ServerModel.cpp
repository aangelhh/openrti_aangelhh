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

#include "ServerModel.h"

#include "ServerOptions.h"
#include "ServerModel/Node.h"

namespace OpenRTI {
namespace ServerModel {

////////////////////////////////////////////////////////////

Region::Region()
{
}

Region::~Region()
{
}

void
Region::setRegionHandle(const LocalRegionHandle& regionHandle)
{
  HandleEntity<Region, LocalRegionHandle>::_setHandle(regionHandle);
}

////////////////////////////////////////////////////////////

InstanceAttribute::InstanceAttribute(ObjectInstance& objectInstance, ClassAttribute& classAttribute) :
  _objectInstance(objectInstance),
  _classAttribute(classAttribute)
{
  setAttributeHandle(_classAttribute.getAttributeHandle());
  /// FIXME
  _receivingConnects = _classAttribute._cumulativeSubscribedConnectHandleSet;
}

InstanceAttribute::~InstanceAttribute()
{
}

void
InstanceAttribute::setAttributeHandle(const AttributeHandle& attributeHandle)
{
  HandleEntity<InstanceAttribute, AttributeHandle>::_setHandle(attributeHandle);
}

////////////////////////////////////////////////////////////

ObjectInstanceConnect::ObjectInstanceConnect(ObjectInstance& objectInstance, FederationConnect& federationConnect) :
  _objectInstance(objectInstance),
  _federationConnect(federationConnect)
{
  setConnectHandle(federationConnect.getConnectHandle());
}

ObjectInstanceConnect::~ObjectInstanceConnect()
{
}

void
ObjectInstanceConnect::setConnectHandle(const ConnectHandle& connectHandle)
{
  HandleMap::Hook::setKey(connectHandle);
}

////////////////////////////////////////////////////////////

ObjectInstance::ObjectInstance(Federation& federation) :
  _federation(federation),
  _objectClass(0)
{
}

ObjectInstance::~ObjectInstance()
{
  _connectHandleObjectInstanceConnectMap.clear();
  _attributeHandleInstanceAttributeMap.clear();
}

void
ObjectInstance::setObjectInstanceHandle(const ObjectInstanceHandle& objectInstanceHandle)
{
  HandleStringEntity<ObjectInstance, ObjectInstanceHandle>::_setHandle(objectInstanceHandle);
}

void
ObjectInstance::setName(const std::string& name)
{
  HandleStringEntity<ObjectInstance, ObjectInstanceHandle>::_setString(name);
}

void
ObjectInstance::insert(InstanceAttribute& instanceAttribute)
{
  _attributeHandleInstanceAttributeMap.insert(instanceAttribute);
}

InstanceAttribute*
ObjectInstance::getInstanceAttribute(const AttributeHandle& attributeHandle)
{
  InstanceAttribute::HandleMap::iterator i = _attributeHandleInstanceAttributeMap.find(attributeHandle);
  if (i == _attributeHandleInstanceAttributeMap.end())
    return 0;
  return i.get();
}

InstanceAttribute*
ObjectInstance::getPrivilegeToDeleteInstanceAttribute()
{
  // InstanceAttribute::HandleMap::iterator i = _attributeHandleInstanceAttributeMap.begin();
  InstanceAttribute::HandleMap::iterator i = _attributeHandleInstanceAttributeMap.find(AttributeHandle(0));
  if (i == _attributeHandleInstanceAttributeMap.end())
    return 0;
  OpenRTIAssert(i->getAttributeHandle() == AttributeHandle(0));
  return i.get();
}

void
ObjectInstance::reference(FederationConnect& federationConnect)
{
  // OpenRTIAssert(_connectHandleObjectInstanceConnectMap.find(federationConnect.getConnectHandle()) == _connectHandleObjectInstanceConnectMap.end());
  if (_connectHandleObjectInstanceConnectMap.find(federationConnect.getConnectHandle()) != _connectHandleObjectInstanceConnectMap.end())
    return;
  ObjectInstanceConnect* objectInstanceConnect;
  objectInstanceConnect = new ObjectInstanceConnect(*this, federationConnect);
  insert(*objectInstanceConnect);
  federationConnect.insert(*objectInstanceConnect);
}

bool
ObjectInstance::unreference(const ConnectHandle& connectHandle)
{
  ObjectInstanceConnect::HandleMap::iterator i = _connectHandleObjectInstanceConnectMap.find(connectHandle);
  OpenRTIAssert(i != _connectHandleObjectInstanceConnectMap.end());
  _connectHandleObjectInstanceConnectMap.erase(i);
  return _connectHandleObjectInstanceConnectMap.empty();
}

void
ObjectInstance::removeConnect(const ConnectHandle& connectHandle)
{
  // FIXME make that an assert at some time
  // OpenRTIAssert(_connectHandleSet.find(connectHandle) == _connectHandleSet.end());
  for (InstanceAttribute::HandleMap::iterator i = _attributeHandleInstanceAttributeMap.begin();
       i != _attributeHandleInstanceAttributeMap.end(); ++i) {
    i->removeConnect(connectHandle);
  }
}

void
ObjectInstance::setObjectClass(ObjectClass* objectClass)
{
  if (objectClass == _objectClass)
    return;
  OpenRTIAssert(!_objectClass);
  _objectClass = objectClass;
  _objectClass->insert(*this);

  ClassAttribute::HandleMap& attributeHandleClassAttributeMap = objectClass->getAttributeHandleClassAttributeMap();
  for (ClassAttribute::HandleMap::iterator i = attributeHandleClassAttributeMap.begin();
       i != attributeHandleClassAttributeMap.end(); ++i) {
    ServerModel::InstanceAttribute* instanceAttribute = new ServerModel::InstanceAttribute(*this, *i);
    insert(*instanceAttribute);
  }
}

////////////////////////////////////////////////////////////

SynchronizationFederate::SynchronizationFederate(Synchronization& synchronization, Federate& federate) :
  _synchronization(synchronization),
  _federate(federate),
  _successful(false)
{
}

SynchronizationFederate::~SynchronizationFederate()
{
}

void
SynchronizationFederate::setFederateHandle(const FederateHandle& federateHandle)
{
  HandleListEntity<SynchronizationFederate, FederateHandle>::_setHandle(federateHandle);
}

void
SynchronizationFederate::setSuccessful(bool successful)
{
  _successful = successful;
}

////////////////////////////////////////////////////////////

Synchronization::Synchronization()
{
}

Synchronization::~Synchronization()
{
  _achievedFederateSynchronizationMap.clear();
  _waitingFederateSynchronizationMap.clear();
}

void
Synchronization::setLabel(const std::string& label)
{
  NameMap::Hook::setKey(label);
}

void
Synchronization::setTag(const VariableLengthData& tag)
{
  _tag = tag;
}

void
Synchronization::setAddJoiningFederates(bool addJoiningFederates)
{
  _addJoiningFederates = addJoiningFederates;
}

bool
Synchronization::getIsWaitingFor(const FederateHandle& federateHandle)
{
  return _waitingFederateSynchronizationMap.find(federateHandle) != _waitingFederateSynchronizationMap.end();
}

void
Synchronization::insert(Federate& federate)
{
  OpenRTIAssert(_waitingFederateSynchronizationMap.find(federate.getFederateHandle()) == _waitingFederateSynchronizationMap.end());
  OpenRTIAssert(_achievedFederateSynchronizationMap.find(federate.getFederateHandle()) == _achievedFederateSynchronizationMap.end());
  if (federate.getResignPending())
    return;
  SynchronizationFederate* synchronizationFederate = new SynchronizationFederate(*this, federate);
  synchronizationFederate->setFederateHandle(federate.getFederateHandle());
  federate.insert(*synchronizationFederate);
  _waitingFederateSynchronizationMap.insert(*synchronizationFederate);
}

void
Synchronization::achieved(const FederateHandle& federateHandle, bool successful)
{
  ServerModel::SynchronizationFederate::HandleMap::iterator i;
  i = _waitingFederateSynchronizationMap.find(federateHandle);
  if (i == _waitingFederateSynchronizationMap.end())
    return;
  i->setSuccessful(successful);
  // OpenRTIAssert(_achievedFederateSynchronizationMap.find(i->getFederateHandle()) == _achievedFederateSynchronizationMap.end());
  // Note that no matter where we are currently linked,
  // this removes the entry from one of the maps
  SynchronizationFederate::HandleMap::unlink(*i);
  _achievedFederateSynchronizationMap.insert(*i);
}

////////////////////////////////////////////////////////////

Federate::Federate(Federation& federation) :
  _federation(federation),
  _resignAction(CANCEL_THEN_DELETE_THEN_DIVEST),
  _resignPending(false),
  _federationConnect(0),
  _commitId()
{
}

Federate::~Federate()
{
  _synchronizationFederateList.clear();
  _federationConnect = 0;
}

void
Federate::setName(const std::string& name)
{
  HandleStringEntity<Federate, FederateHandle>::_setString(name);
}

void
Federate::setFederateHandle(const FederateHandle& federateHandle)
{
  HandleStringEntity<Federate, FederateHandle>::_setHandle(federateHandle);
}

void
Federate::setFederateType(const std::string& federateType)
{
  _federateType = federateType;
}

void
Federate::setResignAction(ResignAction resignAction)
{
  _resignAction = resignAction;
}

void
Federate::insert(SynchronizationFederate& synchronizationFederate)
{
  _synchronizationFederateList.push_back(synchronizationFederate);
}

void
Federate::setResignPending(bool resignPending)
{
  _resignPending = resignPending;
}

Region*
Federate::getRegion(const LocalRegionHandle& regionHandle)
{
  Region::HandleMap::iterator i = _regionHandleRegionMap.find(regionHandle);
  if (i == _regionHandleRegionMap.end())
    return 0;
  return i.get();
}

bool
Federate::getIsTimeRegulating() const
{
  // OpenRTIAssert(!FederationConnect::TimeRegulatingFederateList::Hook::is_linked() || _federationConnect->_permitTimeRegulation);
  return FederationConnect::TimeRegulatingFederateList::Hook::is_linked();
}

void
Federate::setTimeAdvanceTimeStamp(const VariableLengthData& timeAdvanceTimeStamp)
{
  _timeAdvanceTimeStamp = timeAdvanceTimeStamp;
}

void
Federate::setNextMessageTimeStamp(const VariableLengthData& nextMessageTimeStamp)
{
  _nextMessageTimeStamp = nextMessageTimeStamp;
}

void
Federate::setCommitId(Unsigned commitId)
{
  _commitId = commitId;
}

ConnectHandle
Federate::getConnectHandle() const
{
  if (!_federationConnect)
    return ConnectHandle();
  return _federationConnect->getConnectHandle();
}

void
Federate::send(const SharedPtr<const AbstractMessage>& message)
{
  if (!_federationConnect)
    return;
  _federationConnect->send(message);
}

////////////////////////////////////////////////////////////

DimensionModule::DimensionModule(Dimension& dimension, Module& module) :
  _dimension(dimension),
  _module(module)
{
}

DimensionModule::~DimensionModule()
{
}

////////////////////////////////////////////////////////////

Dimension::Dimension(Federation& federation) :
  _federation(federation),
  _upperBound(~Unsigned(0))
{
}

Dimension::~Dimension()
{
  OpenRTIAssert(_dimensionModuleList.empty());
}

void
Dimension::setName(const std::string& name)
{
  ModuleEntity<Dimension, DimensionHandle>::_setString(name);
}

void
Dimension::setDimensionHandle(const DimensionHandle& dimensionHandle)
{
  ModuleEntity<Dimension, DimensionHandle>::_setHandle(dimensionHandle);
}

void
Dimension::setUpperBound(const Unsigned& upperBound)
{
  _upperBound = upperBound;
}

bool
Dimension::getIsReferencedByAnyModule() const
{
  return !_dimensionModuleList.empty();
}

////////////////////////////////////////////////////////////

UpdateRateModule::UpdateRateModule(UpdateRate& updateRate, Module& module) :
  _updateRate(updateRate), _module(module)
{
}

UpdateRateModule::~UpdateRateModule()
{
}

////////////////////////////////////////////////////////////

UpdateRate::UpdateRate(Federation& federation) :
  _federation(federation),
  _rate(0)
{
}

UpdateRate::~UpdateRate()
{
  OpenRTIAssert(_updateRateModuleList.empty());
}

void
UpdateRate::setName(const std::string& name)
{
  ModuleEntity<UpdateRate, UpdateRateHandle>::_setString(name);
}

void
UpdateRate::setUpdateRateHandle(const UpdateRateHandle& updateRateHandle)
{
  ModuleEntity<UpdateRate, UpdateRateHandle>::_setHandle(updateRateHandle);
}

void
UpdateRate::setRate(const double& rate)
{
  _rate = rate;
}

bool
UpdateRate::getIsReferencedByAnyModule() const
{
  return !_updateRateModuleList.empty();
}

////////////////////////////////////////////////////////////

InteractionClassModule::InteractionClassModule(InteractionClass& interactionClass, Module& module) :
  _interactionClass(interactionClass), _module(module)
{
}

InteractionClassModule::~InteractionClassModule()
{
}

////////////////////////////////////////////////////////////

ParameterDefinitionModule::ParameterDefinitionModule(InteractionClass& interactionClass, Module& module) :
  _interactionClass(interactionClass), _module(module)
{
}

ParameterDefinitionModule::~ParameterDefinitionModule()
{
}

////////////////////////////////////////////////////////////

ClassParameter::ClassParameter(InteractionClass& interactionClass, ParameterDefinition& parameterDefinition) :
  _interactionClass(interactionClass),
  _parameterDefinition(parameterDefinition)
{
}

ClassParameter::~ClassParameter()
{
}

void
ClassParameter::setParameterHandle(const ParameterHandle& parameterHandle)
{
  HandleListEntity<ClassParameter, ParameterHandle>::_setHandle(parameterHandle);
}

////////////////////////////////////////////////////////////

ParameterDefinition::ParameterDefinition(InteractionClass& interactionClass) :
  _interactionClass(interactionClass)
{
}

ParameterDefinition::~ParameterDefinition()
{
  _classParameterList.clear();
}

void
ParameterDefinition::setName(const std::string& name)
{
  HandleStringEntity<ParameterDefinition, ParameterHandle>::_setString(name);
}

void
ParameterDefinition::setParameterHandle(const ParameterHandle& parameterHandle)
{
  HandleStringEntity<ParameterDefinition, ParameterHandle>::_setHandle(parameterHandle);
}

////////////////////////////////////////////////////////////

InteractionClass::InteractionClass(Federation& federation, InteractionClass* parentInteractionClass) :
  _federation(federation),
  _parentInteractionClass(parentInteractionClass),
  _orderType(),
  _transportationType()
{
  if (_parentInteractionClass) {
    _parentInteractionClass->_childInteractionClassList.push_back(*this);

    for (ClassParameter::HandleMap::iterator i = _parentInteractionClass->_parameterHandleClassParameterMap.begin();
         i != _parentInteractionClass->_parameterHandleClassParameterMap.end(); ++i) {
      insertClassParameterFor(i->getParameterDefinition());
    }
  }
}

InteractionClass::~InteractionClass()
{
  _parameterHandleClassParameterMap.clear();
  eraseParameterDefinitions();
  OpenRTIAssert(_parameterHandleParameterDefinitionMap.empty());
  OpenRTIAssert(_parameterNameParameterMap.empty());

  OpenRTIAssert(_childInteractionClassList.empty());
  OpenRTIAssert(_interactionClassModuleList.empty());
  OpenRTIAssert(_parameterDefinitionModuleList.empty());
  _parentInteractionClass = 0;
}

void
InteractionClass::setName(const StringVector& name)
{
  ModuleClassEntity<InteractionClass, InteractionClassHandle>::_setString(name);
}

void
InteractionClass::setInteractionClassHandle(const InteractionClassHandle& interactionClassHandle)
{
  ModuleClassEntity<InteractionClass, InteractionClassHandle>::_setHandle(interactionClassHandle);
}

void
InteractionClass::setOrderType(OrderType orderType)
{
  _orderType = orderType;
}

void
InteractionClass::setTransportationType(TransportationType transportationType)
{
  _transportationType = transportationType;
}

InteractionClass*
InteractionClass::getParentInteractionClass()
{
  return _parentInteractionClass;
}

InteractionClassHandle
InteractionClass::getParentInteractionClassHandle() const
{
  if (!_parentInteractionClass)
    return InteractionClassHandle();
  return _parentInteractionClass->getInteractionClassHandle();
}

bool
InteractionClass::getIsReferencedByAnyModule() const
{
  return !_interactionClassModuleList.empty() || !_parameterDefinitionModuleList.empty();
}

bool
InteractionClass::getAreParametersReferencedByAnyModule() const
{
  return !_parameterDefinitionModuleList.empty();
}

void
InteractionClass::eraseParameterDefinitions()
{
  _parameterNameParameterMap.clear();
  OpenRTIAssert(_parameterNameParameterMap.empty());
}

std::size_t
InteractionClass::getNumParameterDefinitions() const
{
  // FIXME O(N)
  return _parameterHandleParameterDefinitionMap.size();
}

ParameterHandle
InteractionClass::getFirstUnusedParameterHandle()
{
  // FIXME this is O(N)
  std::size_t numParameters = 0;
  if (_parentInteractionClass)
    numParameters += _parentInteractionClass->getFirstUnusedParameterHandle().getHandle();
  numParameters += getNumParameterDefinitions();
  return ParameterHandle(numParameters);
}

void
InteractionClass::insert(ParameterDefinition& parameterDefinition)
{
  _parameterHandleParameterDefinitionMap.insert(parameterDefinition);
  _parameterNameParameterMap.insert(parameterDefinition);
  insertClassParameterFor(parameterDefinition);
}

ParameterDefinition*
InteractionClass::getParameterDefinition(const std::string& name)
{
  ParameterDefinition::NameMap::iterator i = _parameterNameParameterMap.find(name);
  if (i == _parameterNameParameterMap.end())
    return 0;
  return i.get();
}

ParameterDefinition*
InteractionClass::getParameterDefinition(const ParameterHandle& parameterHandle)
{
  ParameterDefinition::HandleMap::iterator i = _parameterHandleParameterDefinitionMap.find(parameterHandle);
  if (i == _parameterHandleParameterDefinitionMap.end())
    return 0;
  return i.get();
}

void
InteractionClass::insertClassParameterFor(ParameterDefinition& parameterDefinition)
{
  ClassParameter* classParameter = new ClassParameter(*this, parameterDefinition);
  classParameter->setParameterHandle(parameterDefinition.getParameterHandle());
  _parameterHandleClassParameterMap.insert(*classParameter);
  parameterDefinition.insert(*classParameter);

  for (ChildList::iterator i = _childInteractionClassList.begin(); i != _childInteractionClassList.end(); ++i)
    i->insertClassParameterFor(parameterDefinition);
}

ClassParameter*
InteractionClass::getClassParameter(const ParameterHandle& parameterHandle)
{
  ClassParameter::HandleMap::iterator i = _parameterHandleClassParameterMap.find(parameterHandle);
  if (i == _parameterHandleClassParameterMap.end())
    return 0;
  return i.get();
}

////////////////////////////////////////////////////////////

ObjectClassModule::ObjectClassModule(ObjectClass& objectClass, Module& module) :
  _objectClass(objectClass), _module(module)
{
}

ObjectClassModule::~ObjectClassModule()
{
}

////////////////////////////////////////////////////////////

AttributeDefinitionModule::AttributeDefinitionModule(ObjectClass& objectClass, Module& module) :
  _objectClass(objectClass), _module(module)
{
}

AttributeDefinitionModule::~AttributeDefinitionModule()
{
}

////////////////////////////////////////////////////////////

ClassAttribute::ClassAttribute(ObjectClass& objectClass, AttributeDefinition& attributeDefinition) :
  _objectClass(objectClass),
  _attributeDefinition(attributeDefinition)
{
}

ClassAttribute::~ClassAttribute()
{
}

void
ClassAttribute::setAttributeHandle(const AttributeHandle& attributeHandle)
{
  HandleListEntity<ClassAttribute, AttributeHandle>::_setHandle(attributeHandle);
}

////////////////////////////////////////////////////////////

AttributeDefinition::AttributeDefinition(ObjectClass& objectClass) :
  _objectClass(objectClass),
  _orderType(),
  _transportationType()
{
}

AttributeDefinition::~AttributeDefinition()
{
  _classAttributeList.clear();
}

void
AttributeDefinition::setName(const std::string& name)
{
  HandleStringEntity<AttributeDefinition, AttributeHandle>::_setString(name);
}

void
AttributeDefinition::setAttributeHandle(const AttributeHandle& attributeHandle)
{
  HandleStringEntity<AttributeDefinition, AttributeHandle>::_setHandle(attributeHandle);
}

void
AttributeDefinition::setOrderType(OrderType orderType)
{
  _orderType = orderType;
}

void
AttributeDefinition::setTransportationType(TransportationType transportationType)
{
  _transportationType = transportationType;
}

////////////////////////////////////////////////////////////

ObjectClass::ObjectClass(Federation& federation, ObjectClass* parentObjectClass) :
  _federation(federation),
  _parentObjectClass(parentObjectClass)
{
  if (_parentObjectClass) {
    _parentObjectClass->_childObjectClassList.push_back(*this);

    for (ClassAttribute::HandleMap::iterator i = _parentObjectClass->_attributeHandleClassAttributeMap.begin();
         i != _parentObjectClass->_attributeHandleClassAttributeMap.end(); ++i) {
      insertClassAttributeFor(i->getAttributeDefinition());
    }
  }
}

ObjectClass::~ObjectClass()
{
  _objectInstanceList.unlink();
  _attributeHandleClassAttributeMap.clear();
  eraseAttributeDefinitions();
  OpenRTIAssert(_childObjectClassList.empty());
  OpenRTIAssert(_objectClassModuleList.empty());
  OpenRTIAssert(_attributeDefinitionModuleList.empty());
  _parentObjectClass = 0;
}

void
ObjectClass::setName(const StringVector& name)
{
  ModuleClassEntity<ObjectClass, ObjectClassHandle>::_setString(name);
}

void
ObjectClass::setObjectClassHandle(const ObjectClassHandle& objectClassHandle)
{
  ModuleClassEntity<ObjectClass, ObjectClassHandle>::_setHandle(objectClassHandle);
}

ObjectClass*
ObjectClass::getParentObjectClass()
{
  return _parentObjectClass;
}

ObjectClassHandle
ObjectClass::getParentObjectClassHandle() const
{
  if (!_parentObjectClass)
    return ObjectClassHandle();
  return _parentObjectClass->getObjectClassHandle();
}

bool
ObjectClass::getIsReferencedByAnyModule() const
{
  return !_objectClassModuleList.empty() || !_attributeDefinitionModuleList.empty();
}

bool
ObjectClass::getAreAttributesReferencedByAnyModule() const
{
  return !_attributeDefinitionModuleList.empty();
}

void
ObjectClass::eraseAttributeDefinitions()
{
  _attributeHandleAttributeDefinitionMap.clear();
  OpenRTIAssert(_attributeNameAttributeDefinitionMap.empty());
}

std::size_t
ObjectClass::getNumAttributeDefinitions() const
{
  // FIXME O(N)
  return _attributeHandleAttributeDefinitionMap.size();
}

AttributeHandle
ObjectClass::getFirstUnusedAttributeHandle()
{
  // FIXME this is O(N)
  std::size_t numAttributes = 0;
  if (_parentObjectClass)
    numAttributes += _parentObjectClass->getFirstUnusedAttributeHandle().getHandle();
  numAttributes += getNumAttributeDefinitions();
  return AttributeHandle(numAttributes);
}

void
ObjectClass::insert(AttributeDefinition& attributeDefinition)
{
  _attributeHandleAttributeDefinitionMap.insert(attributeDefinition);
  _attributeNameAttributeDefinitionMap.insert(attributeDefinition);
  insertClassAttributeFor(attributeDefinition);
}

AttributeDefinition*
ObjectClass::getAttributeDefinition(const std::string& name)
{
  AttributeDefinition::NameMap::iterator i = _attributeNameAttributeDefinitionMap.find(name);
  if (i == _attributeNameAttributeDefinitionMap.end())
    return 0;
  return i.get();
}

AttributeDefinition*
ObjectClass::getAttributeDefinition(const AttributeHandle& attributeHandle)
{
  AttributeDefinition::HandleMap::iterator i = _attributeHandleAttributeDefinitionMap.find(attributeHandle);
  if (i == _attributeHandleAttributeDefinitionMap.end())
    return 0;
  return i.get();
}

void
ObjectClass::insertClassAttributeFor(AttributeDefinition& attributeDefinition)
{
  ClassAttribute* classAttribute = new ClassAttribute(*this, attributeDefinition);
  classAttribute->setAttributeHandle(attributeDefinition.getAttributeHandle());
  _attributeHandleClassAttributeMap.insert(*classAttribute);
  attributeDefinition.insert(*classAttribute);

  for (ChildList::iterator i = _childObjectClassList.begin(); i != _childObjectClassList.end(); ++i)
    i->insertClassAttributeFor(attributeDefinition);
}

ClassAttribute*
ObjectClass::getClassAttribute(const AttributeHandle& attributeHandle)
{
  ClassAttribute::HandleMap::iterator i = _attributeHandleClassAttributeMap.find(attributeHandle);
  if (i == _attributeHandleClassAttributeMap.end())
    return 0;
  return i.get();
}

ClassAttribute*
ObjectClass::getPrivilegeToDeleteClassAttribute()
{
  // ClassAttribute::HandleMap::iterator i = _attributeHandleClassAttributeMap.begin();
  ClassAttribute::HandleMap::iterator i = _attributeHandleClassAttributeMap.find(AttributeHandle(0));
  if (i == _attributeHandleClassAttributeMap.end())
    return 0;
  OpenRTIAssert(i->getAttributeHandle() == AttributeHandle(0));
  return i.get();
}

void
ObjectClass::insert(ObjectClassModule& objectClassModule)
{
  _objectClassModuleList.push_back(objectClassModule);
}

void
ObjectClass::insert(AttributeDefinitionModule& attributeDefinitionModule)
{
  _attributeDefinitionModuleList.push_back(attributeDefinitionModule);
}

void
ObjectClass::insert(ObjectInstance& objectInstance)
{
  _objectInstanceList.push_back(objectInstance);
}

void
ObjectClass::removeConnect(const ConnectHandle& connectHandle)
{
  for (ClassAttribute::HandleMap::iterator i = _attributeHandleClassAttributeMap.begin();
       i != _attributeHandleClassAttributeMap.end(); ++i)
    i->removeConnect(connectHandle);
}

} // namespace ServerModel
} // namespace OpenRTI
