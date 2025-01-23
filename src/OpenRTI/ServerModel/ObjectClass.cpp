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

#include "ObjectClass.h"

#include "AttributeDefinition.h"
#include "AttributeDefinitionModule.h"
#include "Federation.h"
#include "ObjectClassModule.h"

namespace OpenRTI {
namespace ServerModel {

ObjectClass::ObjectClass(Federation& federation, ObjectClassHandle const& objectClassHandle, StringVector const& name, ObjectClass* parentObjectClass) :
  _federation(federation),
  _objectClassHandle(objectClassHandle),
  _name(name),
  _parentObjectClass(parentObjectClass)
{
  _federation._insertObjectClassHandleObjectClassMap(*this);
  _federation._insertObjectClassNameObjectClassMap(*this);
  if (_parentObjectClass) {
    _parentObjectClass->_insertChildObjectClassList(*this);

    for (AttributeHandleClassAttributeMap::iterator i = _parentObjectClass->_attributeHandleClassAttributeMap.begin();
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

  if (ObjectClass* parentObjectClass = _parentObjectClass)
    parentObjectClass->_unlinkChildObjectClassList(*this);
  _federation._unlinkObjectClassNameObjectClassMap(*this);
  _federation._unlinkObjectClassHandleObjectClassMap(*this);

  OpenRTIAssert(_attributeHandleClassAttributeMap.empty());
  OpenRTIAssert(_attributeDefinitionModuleList.empty());
  OpenRTIAssert(_attributeNameAttributeDefinitionMap.empty());
  OpenRTIAssert(_attributeHandleAttributeDefinitionMap.empty());
  OpenRTIAssert(_objectClassModuleList.empty());
  OpenRTIAssert(_childObjectClassList.empty());
}

ObjectClassHandle
ObjectClass::getParentObjectClassHandle() const
{
  if (!_parentObjectClass)
    return ObjectClassHandle();
  return _parentObjectClass->getObjectClassHandle();
}

bool
ObjectClass::getObjectClassIsReferencedByAnyModule() const
{
  return !_objectClassModuleList.empty();
}

bool
ObjectClass::getIsReferencedByAnyModule() const
{
  return getObjectClassIsReferencedByAnyModule() || getAreAttributesReferencedByAnyModule();
}

AttributeDefinition const*
ObjectClass::getAttributeDefinition(AttributeHandle const& attributeHandle) const
{
  AttributeHandleAttributeDefinitionMap::const_iterator i;
  i = _attributeHandleAttributeDefinitionMap.find(attributeHandle);
  if (i == _attributeHandleAttributeDefinitionMap.end())
    return 0;
  return i.get();
}

AttributeDefinition*
ObjectClass::getAttributeDefinition(AttributeHandle const& attributeHandle)
{
  AttributeHandleAttributeDefinitionMap::iterator i;
  i = _attributeHandleAttributeDefinitionMap.find(attributeHandle);
  if (i == _attributeHandleAttributeDefinitionMap.end())
    return 0;
  return i.get();
}

std::size_t
ObjectClass::getNumAttributeDefinitions() const
{
  // FIXME O(N)
  return _attributeHandleAttributeDefinitionMap.size();
}

AttributeDefinition const*
ObjectClass::getAttributeDefinition(std::string const& name) const
{
  AttributeNameAttributeDefinitionMap::const_iterator i;
  i = _attributeNameAttributeDefinitionMap.find(name);
  if (i == _attributeNameAttributeDefinitionMap.end())
    return 0;
  return i.get();
}

AttributeDefinition*
ObjectClass::getAttributeDefinition(std::string const& name)
{
  AttributeNameAttributeDefinitionMap::iterator i;
  i = _attributeNameAttributeDefinitionMap.find(name);
  if (i == _attributeNameAttributeDefinitionMap.end())
    return 0;
  return i.get();
}

void
ObjectClass::eraseAttributeDefinitions()
{
  _attributeHandleAttributeDefinitionMap.clear();
  OpenRTIAssert(_attributeNameAttributeDefinitionMap.empty());
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
ObjectClass::insert(AttributeDefinitionModule& attributeDefinitionModule)
{
  _attributeDefinitionModuleList.push_back(attributeDefinitionModule);
}

bool
ObjectClass::getAreAttributesReferencedByAnyModule() const
{
  return !_attributeDefinitionModuleList.empty();
}

ClassAttribute const*
ObjectClass::getClassAttribute(AttributeHandle const& attributeHandle) const
{
  AttributeHandleClassAttributeMap::const_iterator i;
  i = _attributeHandleClassAttributeMap.find(attributeHandle);
  if (i == _attributeHandleClassAttributeMap.end())
    return 0;
  return i.get();
}

ClassAttribute*
ObjectClass::getClassAttribute(AttributeHandle const& attributeHandle)
{
  AttributeHandleClassAttributeMap::iterator i;
  i = _attributeHandleClassAttributeMap.find(attributeHandle);
  if (i == _attributeHandleClassAttributeMap.end())
    return 0;
  return i.get();
}

ClassAttribute*
ObjectClass::getPrivilegeToDeleteClassAttribute()
{
  // AttributeHandleClassAttributeMap::iterator i = _attributeHandleClassAttributeMap.begin();
  AttributeHandleClassAttributeMap::iterator i = _attributeHandleClassAttributeMap.find(AttributeHandle(0));
  if (i == _attributeHandleClassAttributeMap.end())
    return 0;
  OpenRTIAssert(i->getAttributeHandle() == AttributeHandle(0));
  return i.get();
}

void
ObjectClass::insertClassAttributeFor(AttributeDefinition& attributeDefinition)
{
  ClassAttribute* classAttribute = new ClassAttribute(*this, attributeDefinition);
  _attributeHandleClassAttributeMap.insert(*classAttribute);
  attributeDefinition.insert(*classAttribute);

  for (ChildObjectClassList::iterator i = _childObjectClassList.begin(); i != _childObjectClassList.end(); ++i)
    i->insertClassAttributeFor(attributeDefinition);
}

void
ObjectClass::insert(ObjectInstance& objectInstance)
{
  _objectInstanceList.push_back(objectInstance);
}

void
ObjectClass::removeConnect(ConnectHandle const& connectHandle)
{
  for (AttributeHandleClassAttributeMap::iterator i = _attributeHandleClassAttributeMap.begin();
       i != _attributeHandleClassAttributeMap.end(); ++i)
    i->removeConnect(connectHandle);
}

void
ObjectClass::_insertChildObjectClassList(ObjectClass& objectClass)
{
  _childObjectClassList.push_back(objectClass);
}

void
ObjectClass::_unlinkChildObjectClassList(ObjectClass& objectClass)
{
  _childObjectClassList.unlink(objectClass);
}

void
ObjectClass::_insertObjectClassModuleList(ObjectClassModule& objectClassModule)
{
  _objectClassModuleList.push_back(objectClassModule);
}

void
ObjectClass::_unlinkObjectClassModuleList(ObjectClassModule& objectClassModule)
{
  _objectClassModuleList.unlink(objectClassModule);
}

void
ObjectClass::_insertAttributeHandleAttributeDefinitionMap(AttributeDefinition& attributeDefinition)
{
  _attributeHandleAttributeDefinitionMap.insert(attributeDefinition);
}

void
ObjectClass::_unlinkAttributeHandleAttributeDefinitionMap(AttributeDefinition& attributeDefinition)
{
  _attributeHandleAttributeDefinitionMap.unlink(attributeDefinition);
}

void
ObjectClass::_insertAttributeNameAttributeDefinitionMap(AttributeDefinition& attributeDefinition)
{
  _attributeNameAttributeDefinitionMap.insert(attributeDefinition);
}

void
ObjectClass::_unlinkAttributeNameAttributeDefinitionMap(AttributeDefinition& attributeDefinition)
{
  _attributeNameAttributeDefinitionMap.unlink(attributeDefinition);
}

} // namespace ServerModel
} // namespace OpenRTI
