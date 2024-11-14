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

} // namespace ServerModel
} // namespace OpenRTI
