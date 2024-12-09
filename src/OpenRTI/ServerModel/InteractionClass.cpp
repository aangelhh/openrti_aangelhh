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

#include "InteractionClass.h"

namespace OpenRTI {
namespace ServerModel {

InteractionClass::InteractionClass(Federation& federation, InteractionClassHandle const& interactionClassHandle, StringVector const& name, InteractionClass* parentInteractionClass) :
  IntrusiveUnorderedMap<InteractionClassHandle const, InteractionClass>::Hook(interactionClassHandle),
  IntrusiveUnorderedMap<StringVector const, InteractionClass>::Hook(name),
  _federation(federation),
  _orderType(RECEIVE),
  _transportationType(RELIABLE),
  _parentInteractionClass(parentInteractionClass)
{
  if (_parentInteractionClass) {
    _parentInteractionClass->_childInteractionClassList.push_back(*this);

    for (ParameterHandleClassParameterMap::iterator i = _parentInteractionClass->_parameterHandleClassParameterMap.begin();
         i != _parentInteractionClass->_parameterHandleClassParameterMap.end(); ++i) {
      insertClassParameterFor(i->getParameterDefinition());
    }
  }
}

InteractionClass::~InteractionClass()
{
  _parameterHandleClassParameterMap.clear();
  eraseParameterDefinitions();

  OpenRTIAssert(_parameterHandleClassParameterMap.empty());
  OpenRTIAssert(_parameterDefinitionModuleList.empty());
  OpenRTIAssert(_parameterNameParameterDefinitionMap.empty());
  OpenRTIAssert(_parameterHandleParameterDefinitionMap.empty());
  OpenRTIAssert(_interactionClassModuleList.empty());
  OpenRTIAssert(_childInteractionClassList.empty());
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

ParameterDefinition const*
InteractionClass::getParameterDefinition(ParameterHandle const& parameterHandle) const
{
  ParameterHandleParameterDefinitionMap::const_iterator i;
  i = _parameterHandleParameterDefinitionMap.find(parameterHandle);
  if (i == _parameterHandleParameterDefinitionMap.end())
    return 0;
  return i.get();
}

ParameterDefinition*
InteractionClass::getParameterDefinition(ParameterHandle const& parameterHandle)
{
  ParameterHandleParameterDefinitionMap::iterator i;
  i = _parameterHandleParameterDefinitionMap.find(parameterHandle);
  if (i == _parameterHandleParameterDefinitionMap.end())
    return 0;
  return i.get();
}

ParameterDefinition const*
InteractionClass::getParameterDefinition(std::string const& name) const
{
  ParameterNameParameterDefinitionMap::const_iterator i;
  i = _parameterNameParameterDefinitionMap.find(name);
  if (i == _parameterNameParameterDefinitionMap.end())
    return 0;
  return i.get();
}

ParameterDefinition*
InteractionClass::getParameterDefinition(std::string const& name)
{
  ParameterNameParameterDefinitionMap::iterator i;
  i = _parameterNameParameterDefinitionMap.find(name);
  if (i == _parameterNameParameterDefinitionMap.end())
    return 0;
  return i.get();
}

void
InteractionClass::eraseParameterDefinitions()
{
  _parameterNameParameterDefinitionMap.clear();
  OpenRTIAssert(_parameterNameParameterDefinitionMap.empty());
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
  _parameterNameParameterDefinitionMap.insert(parameterDefinition);
  insertClassParameterFor(parameterDefinition);
}

bool
InteractionClass::getAreParametersReferencedByAnyModule() const
{
  return !_parameterDefinitionModuleList.empty();
}

ClassParameter const*
InteractionClass::getClassParameter(ParameterHandle const& parameterHandle) const
{
  ParameterHandleClassParameterMap::const_iterator i;
  i = _parameterHandleClassParameterMap.find(parameterHandle);
  if (i == _parameterHandleClassParameterMap.end())
    return 0;
  return i.get();
}

ClassParameter*
InteractionClass::getClassParameter(ParameterHandle const& parameterHandle)
{
  ParameterHandleClassParameterMap::iterator i;
  i = _parameterHandleClassParameterMap.find(parameterHandle);
  if (i == _parameterHandleClassParameterMap.end())
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

  for (ChildInteractionClassList::iterator i = _childInteractionClassList.begin(); i != _childInteractionClassList.end(); ++i)
    i->insertClassParameterFor(parameterDefinition);
}

} // namespace ServerModel
} // namespace OpenRTI
