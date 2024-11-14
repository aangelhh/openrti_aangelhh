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
#include "ServerModel/DimensionModule.h"
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

} // namespace ServerModel
} // namespace OpenRTI
