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

#include "ObjectInstance.h"

#include "ClassAttribute.h"
#include "Federation.h"
#include "FederationConnect.h"
#include "InstanceAttribute.h"
#include "ObjectClass.h"
#include "ObjectInstanceConnect.h"

namespace OpenRTI {
namespace ServerModel {

ObjectInstance::ObjectInstance(Federation& federation, ObjectInstanceHandle const& objectInstanceHandle, std::string const& name) :
  _federation(federation),
  _objectInstanceHandle(objectInstanceHandle),
  _name(name),
  _objectClass(0)
{
  _federation._insertObjectInstanceHandleObjectInstanceMap(*this);
  _federation._insertObjectInstanceNameObjectInstanceMap(*this);
}

ObjectInstance::~ObjectInstance()
{
  _attributeHandleInstanceAttributeMap.clear();
  _connectHandleObjectInstanceConnectMap.clear();

  setObjectClass(0);
  _federation._unlinkObjectInstanceNameObjectInstanceMap(*this);
  _federation._unlinkObjectInstanceHandleObjectInstanceMap(*this);

  OpenRTIAssert(_attributeHandleInstanceAttributeMap.empty());
  OpenRTIAssert(_connectHandleObjectInstanceConnectMap.empty());
}

void
ObjectInstance::setObjectClass(ObjectClass* objectClass)
{
  if (objectClass == getObjectClass())
    return;
  OpenRTIAssert(!_objectClass || !objectClass);
  _unlinkContainerForObjectClassChange();
  _objectClass = objectClass;
  _insertContainerForObjectClassChange();

  if (!objectClass)
    return;

  ObjectClass::AttributeHandleClassAttributeMap& attributeHandleClassAttributeMap = objectClass->getAttributeHandleClassAttributeMap();
  for (ObjectClass::AttributeHandleClassAttributeMap::iterator i = attributeHandleClassAttributeMap.begin();
       i != attributeHandleClassAttributeMap.end(); ++i) {
    InstanceAttribute* instanceAttribute = createInstanceAttribute(*i);
  }
}

void
ObjectInstance::reference(FederationConnect& federationConnect)
{
  // OpenRTIAssert(_connectHandleObjectInstanceConnectMap.find(federationConnect.getConnectHandle()) == _connectHandleObjectInstanceConnectMap.end());
  if (_connectHandleObjectInstanceConnectMap.find(federationConnect.getConnectHandle()) != _connectHandleObjectInstanceConnectMap.end())
    return;
  ObjectInstanceConnect* objectInstanceConnect = federationConnect.createObjectInstanceConnect(*this);
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

ConnectHandle
ObjectInstance::getOwnerConnectHandle()
{
  InstanceAttribute* instanceAttribute = getInstanceAttribute(AttributeHandle(0));
  if (!instanceAttribute)
    return ConnectHandle();
  return instanceAttribute->getOwnerConnectHandle();
}

void
ObjectInstance::setOwnerConnectHandle(ConnectHandle const& connectHandle)
{
  InstanceAttribute* instanceAttribute = getInstanceAttribute(AttributeHandle(0));
  if (!instanceAttribute)
    return;
  instanceAttribute->setOwnerConnectHandle(connectHandle);
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

InstanceAttribute*
ObjectInstance::createInstanceAttribute(ClassAttribute& classAttribute)
{
  return new InstanceAttribute(*this, classAttribute);
}

void
ObjectInstance::_insertContainerForObjectClassChange()
{
  if (ObjectClass* objectClass = _objectClass)
    objectClass->_insertObjectInstanceList(*this);
}

void
ObjectInstance::_unlinkContainerForObjectClassChange()
{
  if (ObjectClass* objectClass = _objectClass)
    objectClass->_unlinkObjectInstanceList(*this);
}

void
ObjectInstance::_insertConnectHandleObjectInstanceConnectMap(ObjectInstanceConnect& objectInstanceConnect)
{
  _connectHandleObjectInstanceConnectMap.insert(objectInstanceConnect);
}

void
ObjectInstance::_unlinkConnectHandleObjectInstanceConnectMap(ObjectInstanceConnect& objectInstanceConnect)
{
  _connectHandleObjectInstanceConnectMap.unlink(objectInstanceConnect);
}

void
ObjectInstance::_insertAttributeHandleInstanceAttributeMap(InstanceAttribute& instanceAttribute)
{
  _attributeHandleInstanceAttributeMap.insert(instanceAttribute);
}

void
ObjectInstance::_unlinkAttributeHandleInstanceAttributeMap(InstanceAttribute& instanceAttribute)
{
  _attributeHandleInstanceAttributeMap.unlink(instanceAttribute);
}

} // namespace ServerModel
} // namespace OpenRTI
