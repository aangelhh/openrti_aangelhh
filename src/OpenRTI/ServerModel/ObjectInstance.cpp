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

#include "ObjectInstance.h"

#include "ClassAttribute.h"
#include "FederationConnect.h"
#include "ObjectClass.h"

namespace OpenRTI {
namespace ServerModel {

ObjectInstance::ObjectInstance(Federation& federation, ObjectInstanceHandle const& objectInstanceHandle, std::string const& name) :
  IntrusiveUnorderedMap<ObjectInstanceHandle const, ObjectInstance>::Hook(objectInstanceHandle),
  IntrusiveUnorderedMap<std::string const, ObjectInstance>::Hook(name),
  _federation(federation),
  _objectClass(0)
{
}

ObjectInstance::~ObjectInstance()
{
  _attributeHandleInstanceAttributeMap.clear();
  _connectHandleObjectInstanceConnectMap.clear();

  OpenRTIAssert(_attributeHandleInstanceAttributeMap.empty());
  OpenRTIAssert(_connectHandleObjectInstanceConnectMap.empty());
}

void
ObjectInstance::setObjectClass(ObjectClass* objectClass)
{
  if (objectClass == getObjectClass())
    return;
  OpenRTIAssert(!_objectClass);
  _objectClass = objectClass;
  _objectClass->insert(*this);

  ObjectClass::AttributeHandleClassAttributeMap& attributeHandleClassAttributeMap = objectClass->getAttributeHandleClassAttributeMap();
  for (ObjectClass::AttributeHandleClassAttributeMap::iterator i = attributeHandleClassAttributeMap.begin();
       i != attributeHandleClassAttributeMap.end(); ++i) {
    InstanceAttribute* instanceAttribute = new InstanceAttribute(*this, *i);
    insert(*instanceAttribute);
  }
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
ObjectInstance::unreference(ConnectHandle const& connectHandle)
{
  ConnectHandleObjectInstanceConnectMap::iterator i = _connectHandleObjectInstanceConnectMap.find(connectHandle);
  OpenRTIAssert(i != _connectHandleObjectInstanceConnectMap.end());
  _connectHandleObjectInstanceConnectMap.erase(i);
  return _connectHandleObjectInstanceConnectMap.empty();
}

void
ObjectInstance::removeConnect(ConnectHandle const& connectHandle)
{
  // FIXME make that an assert at some time
  // OpenRTIAssert(_connectHandleSet.find(connectHandle) == _connectHandleSet.end());
  for (AttributeHandleInstanceAttributeMap::iterator i = _attributeHandleInstanceAttributeMap.begin();
       i != _attributeHandleInstanceAttributeMap.end(); ++i) {
    i->removeConnect(connectHandle);
  }
}

InstanceAttribute const*
ObjectInstance::getInstanceAttribute(AttributeHandle const& attributeHandle) const
{
  AttributeHandleInstanceAttributeMap::const_iterator i;
  i = _attributeHandleInstanceAttributeMap.find(attributeHandle);
  if (i == _attributeHandleInstanceAttributeMap.end())
    return 0;
  return i.get();
}

InstanceAttribute*
ObjectInstance::getInstanceAttribute(AttributeHandle const& attributeHandle)
{
  AttributeHandleInstanceAttributeMap::iterator i;
  i = _attributeHandleInstanceAttributeMap.find(attributeHandle);
  if (i == _attributeHandleInstanceAttributeMap.end())
    return 0;
  return i.get();
}

InstanceAttribute*
ObjectInstance::getPrivilegeToDeleteInstanceAttribute()
{
  // AttributeHandleInstanceAttributeMap::iterator i = _attributeHandleInstanceAttributeMap.begin();
  AttributeHandleInstanceAttributeMap::iterator i = _attributeHandleInstanceAttributeMap.find(AttributeHandle(0));
  if (i == _attributeHandleInstanceAttributeMap.end())
    return 0;
  OpenRTIAssert(i->getAttributeHandle() == AttributeHandle(0));
  return i.get();
}

void
ObjectInstance::insert(InstanceAttribute& instanceAttribute)
{
  _attributeHandleInstanceAttributeMap.insert(instanceAttribute);
}

} // namespace ServerModel
} // namespace OpenRTI
