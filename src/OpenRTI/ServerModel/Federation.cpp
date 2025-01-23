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

#include "Federation.h"

#include "LogStream.h"

#include "AttributeDefinition.h"
#include "AttributeDefinitionModule.h"
#include "Dimension.h"
#include "DimensionModule.h"
#include "Federate.h"
#include "FederationConnect.h"
#include "InteractionClass.h"
#include "InteractionClassModule.h"
#include "Module.h"
#include "Node.h"
#include "ObjectClass.h"
#include "ObjectClassModule.h"
#include "ObjectInstance.h"
#include "ParameterDefinition.h"
#include "ParameterDefinitionModule.h"
#include "Synchronization.h"
#include "UpdateRate.h"
#include "UpdateRateModule.h"

namespace OpenRTI {
namespace ServerModel {

Federation::Federation(Node& serverNode, FederationHandle const& federationHandle, std::string const& name) :
  _serverNode(serverNode),
  _federationHandle(federationHandle),
  _name(name),
  _objectInstanceHandleObjectInstanceMap(16384/*hash size*/)
{
  _serverNode._insertFederationHandleFederationMap(*this);
  _serverNode._insertFederationNameFederationMap(*this);
}

Federation::~Federation()
{
  // In case of an exception tis can be non empty
  _objectInstanceHandleObjectInstanceMap.clear();

  // There is no need that this gets cleaned up. We can have open synchronizations ...
  // OpenRTIAssert(_synchronizationNameSynchronizationMap.empty());
  _synchronizationNameSynchronizationMap.clear();

  _federateHandleFederateMap.clear();

  // Throw away the base modules
  while (!_moduleHandleModuleMap.empty()) {
    Federation::erase(_moduleHandleModuleMap.back());
  }

  OpenRTIAssert(!hasChildConnects());
  OpenRTIAssert(_connectHandleFederationConnectMap.size() <= 1);
  _connectHandleFederationConnectMap.clear();

  if (getNameIsLinked())
    _serverNode._unlinkFederationNameFederationMap(*this);
  _serverNode._unlinkFederationHandleFederationMap(*this);

  OpenRTIAssert(_objectInstanceNameObjectInstanceMap.empty());
  OpenRTIAssert(_objectInstanceHandleObjectInstanceMap.empty());
  OpenRTIAssert(_federateNameFederateMap.empty());
  OpenRTIAssert(_federateHandleFederateMap.empty());
  OpenRTIAssert(_synchronizationNameSynchronizationMap.empty());
  OpenRTIAssert(_objectClassNameObjectClassMap.empty());
  OpenRTIAssert(_objectClassHandleObjectClassMap.empty());
  OpenRTIAssert(_interactionClassNameInteractionClassMap.empty());
  OpenRTIAssert(_interactionClassHandleInteractionClassMap.empty());
  OpenRTIAssert(_updateRateNameUpdateRateMap.empty());
  OpenRTIAssert(_updateRateHandleUpdateRateMap.empty());
  OpenRTIAssert(_dimensionNameDimensionMap.empty());
  OpenRTIAssert(_dimensionHandleDimensionMap.empty());
  OpenRTIAssert(_moduleHandleModuleMap.empty());
  OpenRTIAssert(_timeRegulatingFederationConnectList.empty());
  OpenRTIAssert(_connectHandleFederationConnectMap.empty());
}

std::string const&
Federation::getServerName() const
{
  return getServerNode().getServerName();
}

std::string const&
Federation::getServerPath() const
{
  return getServerNode().getServerPath();
}

bool
Federation::isRootServer() const
{
  return getServerNode().isRootServer();
}

bool
Federation::isParentConnect(ConnectHandle const& connectHandle) const
{
  return getServerNode().isParentConnect(connectHandle);
}

bool
Federation::hasChildConnects() const
{
  for (ConnectHandleFederationConnectMap::const_reverse_iterator i = _connectHandleFederationConnectMap.rbegin();
       i != _connectHandleFederationConnectMap.rend(); ++i) {
    if (i->getIsParentConnect())
      continue;
    return true;
  }
  return false;
}

bool
Federation::hasChildConnect(ConnectHandle const& connectHandle)
{
  ConnectHandleFederationConnectMap::iterator i = _connectHandleFederationConnectMap.find(connectHandle);
  if (i == _connectHandleFederationConnectMap.end())
    return false;
  return !i->getIsParentConnect();
}

bool
Federation::hasJoinedFederates() const
{
  OpenRTIAssert(_federateHandleFederateMap.empty() == (!_federateHandleAllocator.used()));
  return !_federateHandleFederateMap.empty();
}

bool
Federation::hasJoinedChildren() /*const*/
{
  for (FederateHandleFederateMap::const_iterator i = getFederateHandleFederateMap().begin();
       i != getFederateHandleFederateMap().end(); ++i) {
    FederationConnect const* federationConnect = i->getFederationConnect();
    if (!federationConnect)
      continue;
    if (federationConnect->getIsParentConnect())
      continue;
    // Ok, not even the still pending resign request for invalid connect handles is treated as valid child.
    if (i->getResignPending())
      continue;
    return true;
  }
  return false;
}

void
Federation::setNameIsLinked(bool nameIsLinked)
{
  if (nameIsLinked == getNameIsLinked())
    return;
  if (nameIsLinked) {
    _serverNode._insertFederationNameFederationMap(*this);
  } else {
    OpenRTIAssert(!hasJoinedChildren());
    Log(ServerFederation, Info) << getServerPath() << ": Destroyed federation execution in child server for \""
                                << getName() << "\"!" << std::endl;
    _serverNode._unlinkFederationNameFederationMap(*this);
  }
}

void
Federation::setLogicalTimeFactoryName(std::string const& logicalTimeFactoryName)
{
  _logicalTimeFactoryName = logicalTimeFactoryName;
}

FederationConnect const*
Federation::getFederationConnect(ConnectHandle const& connectHandle) const
{
  ConnectHandleFederationConnectMap::const_iterator i;
  i = _connectHandleFederationConnectMap.find(connectHandle);
  if (i == _connectHandleFederationConnectMap.end())
    return 0;
  return i.get();
}

FederationConnect*
Federation::getFederationConnect(ConnectHandle const& connectHandle)
{
  ConnectHandleFederationConnectMap::iterator i;
  i = _connectHandleFederationConnectMap.find(connectHandle);
  if (i == _connectHandleFederationConnectMap.end())
    return 0;
  return i.get();
}

void
Federation::removeConnect(ConnectHandle const& connectHandle)
{
  for (ObjectInstanceHandleObjectInstanceMap::iterator i = _objectInstanceHandleObjectInstanceMap.begin();
       i != _objectInstanceHandleObjectInstanceMap.end(); ++i) {
    i->removeConnect(connectHandle);
  }
  for (ObjectClassHandleObjectClassMap::iterator i = _objectClassHandleObjectClassMap.begin();
       i != _objectClassHandleObjectClassMap.end(); ++i) {
    i->removeConnect(connectHandle);
  }
  for (InteractionClassHandleInteractionClassMap::iterator i = _interactionClassHandleInteractionClassMap.begin();
       i != _interactionClassHandleInteractionClassMap.end(); ++i) {
    i->removeConnect(connectHandle);
  }
}

void
Federation::send(ConnectHandle const& connectHandle, const SharedPtr<const AbstractMessage>& message)
{
  ConnectHandleFederationConnectMap::iterator i = _connectHandleFederationConnectMap.find(connectHandle);
  if (i == _connectHandleFederationConnectMap.end())
    return;
  i->send(message);
}

void
Federation::send(FederateHandle const& federateHandle, const SharedPtr<const AbstractMessage>& message)
{
  FederateHandleFederateMap::iterator i = _federateHandleFederateMap.find(federateHandle);
  if (i == _federateHandleFederateMap.end())
    return;
  i->send(message);
}

void
Federation::sendToParent(const SharedPtr<const AbstractMessage>& message)
{
  _serverNode.sendToParent(message);
}

void
Federation::broadcast(const SharedPtr<const AbstractMessage>& message)
{
  for (ConnectHandleFederationConnectMap::iterator i = _connectHandleFederationConnectMap.begin();
       i != _connectHandleFederationConnectMap.end(); ++i) {
    i->send(message);
  }
}

void
Federation::broadcast(ConnectHandle const& connectHandle, const SharedPtr<const AbstractMessage>& message)
{
  for (ConnectHandleFederationConnectMap::iterator i = _connectHandleFederationConnectMap.begin();
       i != _connectHandleFederationConnectMap.end(); ++i) {
    if (i->getConnectHandle() == connectHandle)
      continue;
    i->send(message);
  }
}

void
Federation::broadcastToChildren(const SharedPtr<const AbstractMessage>& message)
{
  for (ConnectHandleFederationConnectMap::iterator i = _connectHandleFederationConnectMap.begin();
       i != _connectHandleFederationConnectMap.end(); ++i) {
    if (i->getIsParentConnect())
      continue;
    i->send(message);
  }
}

void
Federation::broadcastToChildren(ConnectHandle const& connectHandle, const SharedPtr<const AbstractMessage>& message)
{
  for (ConnectHandleFederationConnectMap::iterator i = _connectHandleFederationConnectMap.begin();
       i != _connectHandleFederationConnectMap.end(); ++i) {
    if (i->getIsParentConnect())
      continue;
    if (i->getConnectHandle() == connectHandle)
      continue;
    i->send(message);
  }
}

Module const*
Federation::getModule(ModuleHandle const& moduleHandle) const
{
  ModuleHandleModuleMap::const_iterator i;
  i = _moduleHandleModuleMap.find(moduleHandle);
  if (i == _moduleHandleModuleMap.end())
    return 0;
  return i.get();
}

Module*
Federation::getModule(ModuleHandle const& moduleHandle)
{
  ModuleHandleModuleMap::iterator i;
  i = _moduleHandleModuleMap.find(moduleHandle);
  if (i == _moduleHandleModuleMap.end())
    return 0;
  return i.get();
}

OrderType
Federation::resolveOrderType(std::string const& orderType)
{
  if (caseCompare(orderType, "timestamp"))
    return TIMESTAMP;
  else if (caseCompare(orderType, "receive"))
    return RECEIVE;
  else if (orderType.empty())
    return TIMESTAMP;
  else
    throw InconsistentFDD(std::string("Unknown order type \"") + orderType + "\".");
}

TransportationType
Federation::resolveTransportationType(std::string const& transportationType)
{
  if (transportationType == "HLAreliable")
    return RELIABLE;
  else if (transportationType == "HLAbestEffort")
    return BEST_EFFORT;
  else if (transportationType.empty())
    return RELIABLE;
  else
    throw InconsistentFDD(std::string("Unknown transportation type \"") + transportationType + "\".");
}

Dimension const*
Federation::getDimension(DimensionHandle const& dimensionHandle) const
{
  DimensionHandleDimensionMap::const_iterator i;
  i = _dimensionHandleDimensionMap.find(dimensionHandle);
  if (i == _dimensionHandleDimensionMap.end())
    return 0;
  return i.get();
}

Dimension*
Federation::getDimension(DimensionHandle const& dimensionHandle)
{
  DimensionHandleDimensionMap::iterator i;
  i = _dimensionHandleDimensionMap.find(dimensionHandle);
  if (i == _dimensionHandleDimensionMap.end())
    return 0;
  return i.get();
}

Dimension const*
Federation::getDimension(std::string const& name) const
{
  DimensionNameDimensionMap::const_iterator i;
  i = _dimensionNameDimensionMap.find(name);
  if (i == _dimensionNameDimensionMap.end())
    return 0;
  return i.get();
}

Dimension*
Federation::getDimension(std::string const& name)
{
  DimensionNameDimensionMap::iterator i;
  i = _dimensionNameDimensionMap.find(name);
  if (i == _dimensionNameDimensionMap.end())
    return 0;
  return i.get();
}

UpdateRate const*
Federation::getUpdateRate(UpdateRateHandle const& updateRateHandle) const
{
  UpdateRateHandleUpdateRateMap::const_iterator i;
  i = _updateRateHandleUpdateRateMap.find(updateRateHandle);
  if (i == _updateRateHandleUpdateRateMap.end())
    return 0;
  return i.get();
}

UpdateRate*
Federation::getUpdateRate(UpdateRateHandle const& updateRateHandle)
{
  UpdateRateHandleUpdateRateMap::iterator i;
  i = _updateRateHandleUpdateRateMap.find(updateRateHandle);
  if (i == _updateRateHandleUpdateRateMap.end())
    return 0;
  return i.get();
}

UpdateRate const*
Federation::getUpdateRate(std::string const& name) const
{
  UpdateRateNameUpdateRateMap::const_iterator i;
  i = _updateRateNameUpdateRateMap.find(name);
  if (i == _updateRateNameUpdateRateMap.end())
    return 0;
  return i.get();
}

UpdateRate*
Federation::getUpdateRate(std::string const& name)
{
  UpdateRateNameUpdateRateMap::iterator i;
  i = _updateRateNameUpdateRateMap.find(name);
  if (i == _updateRateNameUpdateRateMap.end())
    return 0;
  return i.get();
}

InteractionClass const*
Federation::getInteractionClass(InteractionClassHandle const& interactionClassHandle) const
{
  InteractionClassHandleInteractionClassMap::const_iterator i;
  i = _interactionClassHandleInteractionClassMap.find(interactionClassHandle);
  if (i == _interactionClassHandleInteractionClassMap.end())
    return 0;
  return i.get();
}

InteractionClass*
Federation::getInteractionClass(InteractionClassHandle const& interactionClassHandle)
{
  InteractionClassHandleInteractionClassMap::iterator i;
  i = _interactionClassHandleInteractionClassMap.find(interactionClassHandle);
  if (i == _interactionClassHandleInteractionClassMap.end())
    return 0;
  return i.get();
}

InteractionClass const*
Federation::getInteractionClass(StringVector const& name) const
{
  InteractionClassNameInteractionClassMap::const_iterator i;
  i = _interactionClassNameInteractionClassMap.find(name);
  if (i == _interactionClassNameInteractionClassMap.end())
    return 0;
  return i.get();
}

InteractionClass*
Federation::getInteractionClass(StringVector const& name)
{
  InteractionClassNameInteractionClassMap::iterator i;
  i = _interactionClassNameInteractionClassMap.find(name);
  if (i == _interactionClassNameInteractionClassMap.end())
    return 0;
  return i.get();
}

InteractionClass*
Federation::resolveParentInteractionClass(StringVector const& interactionClassName)
{
  StringVector parentInteractionClassName = interactionClassName;
  parentInteractionClassName.pop_back();
  return getInteractionClass(parentInteractionClassName);
}

ObjectClass const*
Federation::getObjectClass(ObjectClassHandle const& objectClassHandle) const
{
  ObjectClassHandleObjectClassMap::const_iterator i;
  i = _objectClassHandleObjectClassMap.find(objectClassHandle);
  if (i == _objectClassHandleObjectClassMap.end())
    return 0;
  return i.get();
}

ObjectClass*
Federation::getObjectClass(ObjectClassHandle const& objectClassHandle)
{
  ObjectClassHandleObjectClassMap::iterator i;
  i = _objectClassHandleObjectClassMap.find(objectClassHandle);
  if (i == _objectClassHandleObjectClassMap.end())
    return 0;
  return i.get();
}

ObjectClass const*
Federation::getObjectClass(StringVector const& name) const
{
  ObjectClassNameObjectClassMap::const_iterator i;
  i = _objectClassNameObjectClassMap.find(name);
  if (i == _objectClassNameObjectClassMap.end())
    return 0;
  return i.get();
}

ObjectClass*
Federation::getObjectClass(StringVector const& name)
{
  ObjectClassNameObjectClassMap::iterator i;
  i = _objectClassNameObjectClassMap.find(name);
  if (i == _objectClassNameObjectClassMap.end())
    return 0;
  return i.get();
}

ObjectClass*
Federation::resolveParentObjectClass(StringVector const& objectClassName)
{
  StringVector parentObjectClassName = objectClassName;
  parentObjectClassName.pop_back();
  return getObjectClass(parentObjectClassName);
}

bool
Federation::insertOrCheck(Module& module, FOMStringDimension const& stringDimension)
{
  DimensionNameDimensionMap::iterator i;
  i = _dimensionNameDimensionMap.find(stringDimension.getName());
  if (i != _dimensionNameDimensionMap.end()) {
    if (stringDimension.getUpperBound() != i->getUpperBound()) {
      std::stringstream ss;
      ss << "Inconsistent Dimension \"" << stringDimension.getName() << "\": Upper bound "
         << stringDimension.getUpperBound() << " does not match the already established value of "
         << i->getUpperBound() << "!";
      throw InconsistentFDD(ss.str());
    }
    module.createDimensionModule(*i);
    return false;
  } else {
    Dimension* dimension = createDimension(_dimensionHandleAllocator.get(), stringDimension.getName());
    dimension->setUpperBound(stringDimension.getUpperBound());
    module.createDimensionModule(*dimension);
    return true;
  }
}

bool
Federation::insertOrCheck(Module& module, FOMStringUpdateRate const& stringUpdateRate)
{
  UpdateRateNameUpdateRateMap::iterator i;
  i = _updateRateNameUpdateRateMap.find(stringUpdateRate.getName());
  if (i != _updateRateNameUpdateRateMap.end()) {
    if (stringUpdateRate.getRate() != i->getRate()) {
      std::stringstream ss;
      ss << "Inconsistent UpdateRate \"" << stringUpdateRate.getName() << "\": Rate "
         << stringUpdateRate.getRate() << " does not match the already established value of "
         << i->getRate() << "!";
      throw InconsistentFDD(ss.str());
    }
    module.createUpdateRateModule(*i);
    return false;
  } else {
    UpdateRate* updateRate = createUpdateRate(_updateRateHandleAllocator.get(), stringUpdateRate.getName());
    updateRate->setRate(stringUpdateRate.getRate());
    module.createUpdateRateModule(*updateRate);
    return true;
  }
}

bool
Federation::insertOrCheck(Module& module, FOMStringInteractionClass const& stringInteractionClass)
{
  InteractionClassNameInteractionClassMap::iterator i;
  i = _interactionClassNameInteractionClassMap.find(stringInteractionClass.getName());
  if (i != _interactionClassNameInteractionClassMap.end()) {
    OpenRTIAssert(!i->getName().empty());

    if (i->getOrderType() != resolveOrderType(stringInteractionClass.getOrderType())) {
      std::stringstream ss;
      ss << "Inconsistent InteractionClass \"" << stringInteractionClass.getName() << "\": OrderType "
         << stringInteractionClass.getOrderType() << " does not match the already established value of "
         << i->getOrderType() << "!";
      throw InconsistentFDD(ss.str());
    }
    if (i->getTransportationType() != resolveTransportationType(stringInteractionClass.getTransportationType())) {
      std::stringstream ss;
      ss << "Inconsistent InteractionClass \"" << stringInteractionClass.getName() << "\": TransportationType "
         << stringInteractionClass.getTransportationType() << " does not match the already established value of "
         << i->getTransportationType() << "!";
      throw InconsistentFDD(ss.str());
    }

    DimensionHandleSet dimensionHandleSet;
    for (StringSet::const_iterator j = stringInteractionClass.getDimensionSet().begin();
         j != stringInteractionClass.getDimensionSet().end(); ++j) {
      Dimension* dimension = resolveDimension(*j);
      if (!dimension)
        throw InconsistentFDD("Cannot resolve dimension \"" + *j + "\"!");
      /// FIXME avoid using the handle sets ...
      dimensionHandleSet.insert(dimension->getDimensionHandle());
    }
    if (i->_dimensionHandleSet != dimensionHandleSet) {
      std::stringstream ss;
      ss << "Inconsistent InteractionClass \"" << stringInteractionClass.getName()
         << "\": Dimensions do not match the already established value!";
      throw InconsistentFDD(ss.str());
    }

    // In this case we need to check for the parameter list being the same on both ends.
    if (!stringInteractionClass.getParameterList().empty()) {
      std::vector<ParameterHandle> parameterHandles;
      parameterHandles.reserve(stringInteractionClass.getParameterList().size());
      for (FOMStringParameterList::const_iterator j = stringInteractionClass.getParameterList().begin();
           j != stringInteractionClass.getParameterList().end(); ++j) {
        ParameterDefinition* parameterDefinition;
        parameterDefinition = i->getParameterDefinition(j->getName());
        if (!parameterDefinition) {
          std::stringstream ss;
          ss << "Inconsistent InteractionClass \"" << stringInteractionClass.getName() << "\": Parameter \""
             << j->getName() << "\" not present in already established interaction class!";
          throw InconsistentFDD(ss.str());
        }
        parameterHandles.push_back(parameterDefinition->getParameterHandle());
      }
      std::sort(parameterHandles.begin(), parameterHandles.end());
      if (std::unique(parameterHandles.begin(), parameterHandles.end()) != parameterHandles.end()) {
        std::stringstream ss;
        ss << "Inconsistent InteractionClass \"" << stringInteractionClass.getName()
           << "\": Parameters do not match the already established value!";
        throw InconsistentFDD(ss.str());
      }
    }

    module.createInteractionClassModule(*i);
    if (!stringInteractionClass.getParameterList().empty())
      module.createParameterDefinitionModule(*i);

    return false;
  } else {
    InteractionClass* parentInteractionClass;
    parentInteractionClass = resolveParentInteractionClass(stringInteractionClass.getName());
    // This signals an error in message preparation
    if (!parentInteractionClass && 1 < stringInteractionClass.getName().size())
      throw MessageError("Cannot resolve parent interaction class name!");
    InteractionClass* interactionClass = createInteractionClass(_interactionClassHandleAllocator.get(), stringInteractionClass.getName(), parentInteractionClass);
    module.createInteractionClassModule(*interactionClass);

    interactionClass->setOrderType(resolveOrderType(stringInteractionClass.getOrderType()));
    interactionClass->setTransportationType(resolveTransportationType(stringInteractionClass.getTransportationType()));
    for (StringSet::const_iterator i = stringInteractionClass.getDimensionSet().begin();
         i != stringInteractionClass.getDimensionSet().end(); ++i) {
      Dimension* dimension = resolveDimension(*i);
      if (!dimension)
        throw InconsistentFDD("Cannot resolve dimension \"" + *i + "\"!");
      /// FIXME avoid using the handle sets ...
      interactionClass->_dimensionHandleSet.insert(dimension->getDimensionHandle());
    }

    if (!stringInteractionClass.getParameterList().empty())
      module.createParameterDefinitionModule(*interactionClass);

    ParameterHandle nextParameterHandle = interactionClass->getFirstUnusedParameterHandle();
    for (FOMStringParameterList::const_iterator i = stringInteractionClass.getParameterList().begin();
         i != stringInteractionClass.getParameterList().end(); ++i) {
      if (interactionClass->getParameterDefinition(i->getName())) {
        std::stringstream ss;
        ss << "Duplicate parameter name \"" << i->getName() << "\" in InteractionClass \""
           << interactionClass->getName() << "\"!";
        throw InconsistentFDD(ss.str());
      }

      ParameterDefinition* parameterDefinition = interactionClass->createParameterDefinition(nextParameterHandle, i->getName());
      interactionClass->insertClassParameterFor(*parameterDefinition);

      nextParameterHandle = ParameterHandle(nextParameterHandle.getHandle() + 1);
    }

    return true;
  }
}

bool
Federation::insertOrCheck(Module& module, FOMStringObjectClass const& stringObjectClass)
{
  ObjectClassNameObjectClassMap::iterator i;
  i = _objectClassNameObjectClassMap.find(stringObjectClass.getName());
  if (i != _objectClassNameObjectClassMap.end()) {
    OpenRTIAssert(!i->getName().empty());

    // In this case we need to check for the attribute list being the same on both ends.
    if (!stringObjectClass.getAttributeList().empty()) {
      std::vector<AttributeHandle> attributeHandles;
      attributeHandles.reserve(stringObjectClass.getAttributeList().size());
      for (FOMStringAttributeList::const_iterator j = stringObjectClass.getAttributeList().begin();
           j != stringObjectClass.getAttributeList().end(); ++j) {
        AttributeDefinition* attributeDefinition;
        attributeDefinition = i->getAttributeDefinition(j->getName());
        if (!attributeDefinition) {
          std::stringstream ss;
          ss << "Inconsistent ObjectClass \"" << stringObjectClass.getName() << "\": Attribute \""
             << j->getName() << "\" not present in already established object class!";
          throw InconsistentFDD(ss.str());
        }
        if (attributeDefinition->getOrderType() != resolveOrderType(j->getOrderType())) {
          std::stringstream ss;
          ss << "Inconsistent ObjectClass  \"" << stringObjectClass.getName() << "\": Attribute \""
             << j->getName() << "\": OrderType " << j->getOrderType()
             << " does not match the already established value of "
             << attributeDefinition->getOrderType() << "!";
          throw InconsistentFDD(ss.str());
        }
        if (attributeDefinition->getTransportationType() != resolveTransportationType(j->getTransportationType())) {
          std::stringstream ss;
          ss << "Inconsistent ObjectClass  \"" << stringObjectClass.getName() << "\": Attribute \""
             << j->getName() << "\": TransportationType " << j->getTransportationType()
             << " does not match the already established value of "
             << attributeDefinition->getTransportationType() << "!";
          throw InconsistentFDD(ss.str());
        }

        DimensionHandleSet dimensionHandleSet;
        for (StringSet::const_iterator k = j->getDimensionSet().begin();
             k != j->getDimensionSet().end(); ++k) {
          Dimension* dimension = resolveDimension(*k);
          if (!dimension)
            throw InconsistentFDD("Cannot resolve dimension \"" + *k + "\"!");
          /// FIXME avoid using the handle sets ...
          dimensionHandleSet.insert(dimension->getDimensionHandle());
        }
        if (attributeDefinition->_dimensionHandleSet != dimensionHandleSet) {
          std::stringstream ss;
          ss << "Inconsistent ObjectClass  \"" << stringObjectClass.getName() << "\": Attribute \""
             << j->getName() << "\": Dimensions do not match the already established value!";
          throw InconsistentFDD(ss.str());
        }

        attributeHandles.push_back(attributeDefinition->getAttributeHandle());
      }
      std::sort(attributeHandles.begin(), attributeHandles.end());
      if (std::unique(attributeHandles.begin(), attributeHandles.end()) != attributeHandles.end()) {
        std::stringstream ss;
        ss << "Inconsistent ObjectClass \"" << stringObjectClass.getName()
           << "\": Attributes do not match the already established value!";
        throw InconsistentFDD(ss.str());
      }
    }

    module.insert(*i);
    if (!stringObjectClass.getAttributeList().empty())
      module.insertAttributes(*i);

    return false;
  } else {
    ObjectClass* parentObjectClass;
    parentObjectClass = resolveParentObjectClass(stringObjectClass.getName());
    // This signals an error in message preparation
    if (!parentObjectClass && 1 < stringObjectClass.getName().size())
      throw MessageError("Cannot resolve parent object class name!");
    ObjectClass* objectClass = createObjectClass(_objectClassHandleAllocator.get(), stringObjectClass.getName(), parentObjectClass);
    module.insert(*objectClass);

    if (!stringObjectClass.getAttributeList().empty())
      module.insertAttributes(*objectClass);

    AttributeHandle nextAttributeHandle = objectClass->getFirstUnusedAttributeHandle();
    for (FOMStringAttributeList::const_iterator i = stringObjectClass.getAttributeList().begin();
         i != stringObjectClass.getAttributeList().end(); ++i) {
      if (objectClass->getAttributeDefinition(i->getName())) {
        std::stringstream ss;
        ss << "Duplicate attribute name \"" << i->getName() << "\" in ObjectClass \""
           << objectClass->getName() << "\"!";
        throw InconsistentFDD(ss.str());
      }

      AttributeDefinition* attributeDefinition;
      attributeDefinition = new AttributeDefinition(*objectClass, nextAttributeHandle, i->getName());
      objectClass->insertClassAttributeFor(*attributeDefinition);

      attributeDefinition->setOrderType(resolveOrderType(i->getOrderType()));
      attributeDefinition->setTransportationType(resolveTransportationType(i->getTransportationType()));
      for (StringSet::const_iterator j = i->getDimensionSet().begin();
           j != i->getDimensionSet().end(); ++j) {
        Dimension* dimension = resolveDimension(*j);
        if (!dimension)
          throw InconsistentFDD("Cannot resolve dimension \"" + *j + "\"!");
        /// FIXME avoid using the handle sets ...
        attributeDefinition->_dimensionHandleSet.insert(dimension->getDimensionHandle());
      }

      nextAttributeHandle = AttributeHandle(nextAttributeHandle.getHandle() + 1);
    }

    return true;
  }
}

ModuleHandle
Federation::insert(FOMStringModule const& stringModule)
{
  Module* module = createModule(_moduleHandleAllocator.get());
  module->setContent(stringModule.getContent());
  module->setArtificialInteractionRoot(stringModule.getArtificialInteractionRoot());
  module->setArtificialObjectRoot(stringModule.getArtificialObjectRoot());

  bool created = false;
  try {

    for (FOMStringDimensionList::const_iterator j = stringModule.getDimensionList().begin();
         j != stringModule.getDimensionList().end(); ++j) {
      if (insertOrCheck(*module, *j))
        created = true;
    }

    for (FOMStringUpdateRateList::const_iterator j = stringModule.getUpdateRateList().begin();
         j != stringModule.getUpdateRateList().end(); ++j) {
      if (insertOrCheck(*module, *j))
        created = true;
    }

    for (FOMStringInteractionClassList::const_iterator j = stringModule.getInteractionClassList().begin();
         j != stringModule.getInteractionClassList().end(); ++j) {
      if (insertOrCheck(*module, *j))
        created = true;
    }

    for (FOMStringObjectClassList::const_iterator j = stringModule.getObjectClassList().begin();
         j != stringModule.getObjectClassList().end(); ++j) {
      if (insertOrCheck(*module, *j))
        created = true;
    }

    if (!created) {
      erase(*module);
      return ModuleHandle();
    }

    return module->getModuleHandle();
  } catch (...) {
    erase(*module);
    throw;
  }

  return ModuleHandle();
}

void
Federation::insert(FOMStringModuleList const& stringModuleList)
{
  ModuleHandleVector moduleHandleVector;
  insert(moduleHandleVector, stringModuleList);
}

void
Federation::insert(ModuleHandleVector& moduleHandleVector, FOMStringModuleList const& stringModuleList)
{
  moduleHandleVector.reserve(stringModuleList.size());
  try {
    for (FOMStringModuleList::const_iterator i = stringModuleList.begin(); i != stringModuleList.end(); ++i) {
      ModuleHandle moduleHandle = insert(*i);
      if (moduleHandle.valid())
        moduleHandleVector.push_back(moduleHandle);
    }
  } catch (...) {
    for (ModuleHandleVector::iterator i = moduleHandleVector.begin(); i != moduleHandleVector.end(); ++i)
      erase(*i);
    moduleHandleVector.clear();
    throw;
  }
}

void
Federation::insert(Module& module, FOMDimension const& fomDimension)
{
  DimensionHandleDimensionMap::iterator i = _dimensionHandleDimensionMap.find(fomDimension.getDimensionHandle());
  if (i != _dimensionHandleDimensionMap.end()) {
    if (fomDimension.getName() != i->getName())
      throw MessageError("Dimension name does not match.");
    if (fomDimension.getUpperBound() != i->getUpperBound())
      throw MessageError("Dimension upper bound does not match.");
    module.createDimensionModule(*i);
  } else {
    _dimensionHandleAllocator.take(fomDimension.getDimensionHandle());
    Dimension* dimension = createDimension(fomDimension.getDimensionHandle(), fomDimension.getName());
    dimension->setUpperBound(fomDimension.getUpperBound());
    module.createDimensionModule(*dimension);
  }
}

void
Federation::insert(Module& module, FOMUpdateRate const& fomUpdateRate)
{
  UpdateRateHandleUpdateRateMap::iterator i = _updateRateHandleUpdateRateMap.find(fomUpdateRate.getUpdateRateHandle());
  if (i != _updateRateHandleUpdateRateMap.end()) {
    if (fomUpdateRate.getName() != i->getName())
      throw MessageError("UpdateRate name does not match.");
    if (fomUpdateRate.getRate() != i->getRate())
      throw MessageError("UpdateRate rate does not match.");
    module.createUpdateRateModule(*i);
  } else {
    _updateRateHandleAllocator.take(fomUpdateRate.getUpdateRateHandle());
    UpdateRate* updateRate = createUpdateRate(fomUpdateRate.getUpdateRateHandle(), fomUpdateRate.getName());
    updateRate->setRate(fomUpdateRate.getRate());
    module.createUpdateRateModule(*updateRate);
  }
}

void
Federation::insert(Module& module, FOMInteractionClass const& fomInteractionClass)
{
  InteractionClassHandleInteractionClassMap::iterator i;
  i = _interactionClassHandleInteractionClassMap.find(fomInteractionClass.getInteractionClassHandle());
  if (i != _interactionClassHandleInteractionClassMap.end()) {
    OpenRTIAssert(!i->getName().empty());
    if (fomInteractionClass.getName() != i->getName().back())
      throw MessageError("InteractionClass name does not match.");
    if (fomInteractionClass.getParentInteractionClassHandle() != i->getParentInteractionClassHandle())
      throw MessageError("Parent InteractionClass does not match.");
    if (fomInteractionClass.getOrderType() != i->getOrderType())
      throw MessageError("InteractionClass order type does not match.");
    if (fomInteractionClass.getTransportationType() != i->getTransportationType())
      throw MessageError("InteractionClass transportation type does not match.");
    if (fomInteractionClass.getDimensionHandleSet() != i->_dimensionHandleSet)
      throw MessageError("InteractionClass dimension handle set does not match.");
    // In this case we want to check for the parameter list being the same on both ends.
    if (!fomInteractionClass.getParameterList().empty()) {
      module.createParameterDefinitionModule(*i);
      if (i->getParameterHandleParameterDefinitionMap().empty()) {
        for (FOMParameterList::const_iterator j = fomInteractionClass.getParameterList().begin();
             j != fomInteractionClass.getParameterList().end(); ++j) {
          if (i->getParameterDefinition(j->getName()))
            throw MessageError("Duplicate InteractionClass parameter names.");
          if (i->getParameterDefinition(j->getParameterHandle()))
            throw MessageError("Duplicate InteractionClass parameter handles.");
          ParameterDefinition* parameterDefinition = i->createParameterDefinition(j->getParameterHandle(), j->getName());
          i->insertClassParameterFor(*parameterDefinition);
        }
      } else {
        std::vector<ParameterHandle> parameterHandles;
        parameterHandles.reserve(fomInteractionClass.getParameterList().size());
        for (FOMParameterList::const_iterator j = fomInteractionClass.getParameterList().begin();
             j != fomInteractionClass.getParameterList().end(); ++j) {
          ParameterDefinition* parameterDefinition;
          parameterDefinition = i->getParameterDefinition(j->getParameterHandle());
          if (!parameterDefinition)
            throw MessageError("InteractionClass parameter lists do not match.");
          if (parameterDefinition->getName() != j->getName())
            throw MessageError("InteractionClass parameter lists do not match.");
          parameterHandles.push_back(j->getParameterHandle());
        }
        std::sort(parameterHandles.begin(), parameterHandles.end());
        if (std::unique(parameterHandles.begin(), parameterHandles.end()) != parameterHandles.end())
          throw MessageError("InteractionClass parameter lists do not match.");
      }
    }
    module.createInteractionClassModule(*i);
  } else {
    InteractionClass* parentInteractionClass;
    parentInteractionClass = getInteractionClass(fomInteractionClass.getParentInteractionClassHandle());
   if (fomInteractionClass.getParentInteractionClassHandle().valid() && !parentInteractionClass)
      throw MessageError("Invalid parent InteractionClass!");
    StringVector name;
    if (parentInteractionClass)
      name = parentInteractionClass->getName();
    name.push_back(fomInteractionClass.getName());
    _interactionClassHandleAllocator.take(fomInteractionClass.getInteractionClassHandle());
    InteractionClass* interactionClass = createInteractionClass(fomInteractionClass.getInteractionClassHandle(), name, parentInteractionClass);
    module.createInteractionClassModule(*interactionClass);
    interactionClass->setOrderType(fomInteractionClass.getOrderType());
    interactionClass->setTransportationType(fomInteractionClass.getTransportationType());
    interactionClass->_dimensionHandleSet = fomInteractionClass.getDimensionHandleSet();
    if (!fomInteractionClass.getParameterList().empty())
      module.createParameterDefinitionModule(*interactionClass);
    for (FOMParameterList::const_iterator j = fomInteractionClass.getParameterList().begin();
         j != fomInteractionClass.getParameterList().end(); ++j) {
      if (interactionClass->getParameterDefinition(j->getName()))
        throw MessageError("Duplicate InteractionClass parameter names.");
      if (interactionClass->getParameterDefinition(j->getParameterHandle()))
        throw MessageError("Duplicate InteractionClass parameter handles.");
      ParameterDefinition* parameterDefinition = interactionClass->createParameterDefinition(j->getParameterHandle(), j->getName());
      interactionClass->insertClassParameterFor(*parameterDefinition);
    }
  }
}

void
Federation::insert(Module& module, FOMObjectClass const& fomObjectClass)
{
  ObjectClassHandleObjectClassMap::iterator i;
  i = _objectClassHandleObjectClassMap.find(fomObjectClass.getObjectClassHandle());
  if (i != _objectClassHandleObjectClassMap.end()) {
    OpenRTIAssert(!i->getName().empty());
    if (fomObjectClass.getName() != i->getName().back())
      throw MessageError("ObjectClass name does not match.");
    if (fomObjectClass.getParentObjectClassHandle() != i->getParentObjectClassHandle())
      throw MessageError("Parent ObjectClass does not match.");
    // In this case we want to check for the attribute list being the same on both ends.
    if (!fomObjectClass.getAttributeList().empty()) {
      module.insertAttributes(*i);
      if (i->getAttributeHandleAttributeDefinitionMap().empty()) {
        for (FOMAttributeList::const_iterator j = fomObjectClass.getAttributeList().begin();
             j != fomObjectClass.getAttributeList().end(); ++j) {
          if (i->getAttributeDefinition(j->getName()))
            throw MessageError("Duplicate ObjectClass attribute names.");
          if (i->getAttributeDefinition(j->getAttributeHandle()))
            throw MessageError("Duplicate ObjectClass attribute handles.");
          AttributeDefinition* attributeDefinition;
          attributeDefinition = new AttributeDefinition(*i, j->getAttributeHandle(), j->getName());
          i->insertClassAttributeFor(*attributeDefinition);
          attributeDefinition->setOrderType(j->getOrderType());
          attributeDefinition->setTransportationType(j->getTransportationType());
          attributeDefinition->_dimensionHandleSet = j->getDimensionHandleSet();
        }
      } else {
        std::vector<AttributeHandle> attributeHandles;
        attributeHandles.reserve(fomObjectClass.getAttributeList().size());
        for (FOMAttributeList::const_iterator j = fomObjectClass.getAttributeList().begin();
             j != fomObjectClass.getAttributeList().end(); ++j) {
          AttributeDefinition* attributeDefinition;
          attributeDefinition = i->getAttributeDefinition(j->getAttributeHandle());
          if (!attributeDefinition)
            throw MessageError("ObjectClass attribute lists do not match.");
          if (attributeDefinition->getName() != j->getName())
            throw MessageError("ObjectClass attribute lists do not match.");
          if (attributeDefinition->getOrderType() != j->getOrderType())
            throw MessageError("ObjectClass order type does not match.");
          if (attributeDefinition->getTransportationType() != j->getTransportationType())
            throw MessageError("ObjectClass transportation type does not match.");
          if (attributeDefinition->_dimensionHandleSet != j->getDimensionHandleSet())
            throw MessageError("ObjectClass dimension handle set does not match.");
          attributeHandles.push_back(j->getAttributeHandle());
        }
        std::sort(attributeHandles.begin(), attributeHandles.end());
        if (std::unique(attributeHandles.begin(), attributeHandles.end()) != attributeHandles.end())
          throw MessageError("ObjectClass attribute lists do not match.");
      }
    }
    module.insert(*i);
  } else {
    ObjectClass* parentObjectClass;
    parentObjectClass = getObjectClass(fomObjectClass.getParentObjectClassHandle());
    if (fomObjectClass.getParentObjectClassHandle().valid() && !parentObjectClass)
      throw MessageError("Invalid parent ObjectClass!");
    StringVector name;
    if (parentObjectClass)
      name = parentObjectClass->getName();
    name.push_back(fomObjectClass.getName());
    _objectClassHandleAllocator.take(fomObjectClass.getObjectClassHandle());
    ObjectClass* objectClass = createObjectClass(fomObjectClass.getObjectClassHandle(), name, parentObjectClass);
    module.insert(*objectClass);
    if (!fomObjectClass.getAttributeList().empty())
      module.insertAttributes(*objectClass);
    for (FOMAttributeList::const_iterator j = fomObjectClass.getAttributeList().begin();
         j != fomObjectClass.getAttributeList().end(); ++j) {
      if (objectClass->getAttributeDefinition(j->getName()))
        throw MessageError("Duplicate ObjectClass attribute names.");
      if (objectClass->getAttributeDefinition(j->getAttributeHandle()))
        throw MessageError("Duplicate ObjectClass attribute handles.");
      AttributeDefinition* attributeDefinition;
      attributeDefinition = new AttributeDefinition(*objectClass, j->getAttributeHandle(), j->getName());
      objectClass->insertClassAttributeFor(*attributeDefinition);
      attributeDefinition->setOrderType(j->getOrderType());
      attributeDefinition->setTransportationType(j->getTransportationType());
      attributeDefinition->_dimensionHandleSet = j->getDimensionHandleSet();
    }
  }
}

void
Federation::insert(FOMModule const& fomModule)
{
  ModuleHandleModuleMap::iterator i = _moduleHandleModuleMap.find(fomModule.getModuleHandle());
  if (i == _moduleHandleModuleMap.end()) {
    _moduleHandleAllocator.take(fomModule.getModuleHandle());
    Module* module = createModule(fomModule.getModuleHandle());
    module->setContent(fomModule.getContent());
    module->setArtificialInteractionRoot(fomModule.getArtificialInteractionRoot());
    module->setArtificialObjectRoot(fomModule.getArtificialObjectRoot());

    for (FOMDimensionList::const_iterator j = fomModule.getDimensionList().begin();
         j != fomModule.getDimensionList().end(); ++j) {
      insert(*module, *j);
    }

    for (FOMUpdateRateList::const_iterator j = fomModule.getUpdateRateList().begin();
         j != fomModule.getUpdateRateList().end(); ++j) {
      insert(*module, *j);
    }

    for (FOMInteractionClassList::const_iterator j = fomModule.getInteractionClassList().begin();
         j != fomModule.getInteractionClassList().end(); ++j) {
      insert(*module, *j);
    }

    for (FOMObjectClassList::const_iterator j = fomModule.getObjectClassList().begin();
         j != fomModule.getObjectClassList().end(); ++j) {
      insert(*module, *j);
    }
  }
}

void
Federation::insert(FOMModuleList const& fomModuleList)
{
  for (FOMModuleList::const_iterator i = fomModuleList.begin(); i != fomModuleList.end(); ++i) {
    insert(*i);
  }
}

void
Federation::erase(ModuleHandle const& moduleHandle)
{
  ModuleHandleModuleMap::iterator i = _moduleHandleModuleMap.find(moduleHandle);
  OpenRTIAssert(i != _moduleHandleModuleMap.end());
  erase(*i);
}

void
Federation::erase(Module& module)
{
  while (!module.getParameterDefinitionModuleList().empty()) {
    InteractionClass& interactionClass = module.getParameterDefinitionModuleList().back().getInteractionClass();
    module.getParameterDefinitionModuleList().pop_back();
    if (interactionClass.getAreParametersReferencedByAnyModule())
      continue;
    interactionClass.eraseParameterDefinitions();
  }
  while (!module.getInteractionClassModuleList().empty()) {
    InteractionClass& interactionClass = module.getInteractionClassModuleList().back().getInteractionClass();
    module.getInteractionClassModuleList().pop_back();
    if (interactionClass.getIsReferencedByAnyModule())
      continue;
    _interactionClassHandleAllocator.put(interactionClass.getInteractionClassHandle());
    InteractionClassHandleInteractionClassMap::erase(interactionClass);
  }

  while (!module.getAttributeDefinitionModuleList().empty()) {
    ObjectClass& objectClass = module.getAttributeDefinitionModuleList().back().getObjectClass();
    module.getAttributeDefinitionModuleList().pop_back();
    if (objectClass.getAreAttributesReferencedByAnyModule())
      continue;
    objectClass.eraseAttributeDefinitions();
  }
  while (!module.getObjectClassModuleList().empty()) {
    ObjectClass& objectClass = module.getObjectClassModuleList().back().getObjectClass();
    module.getObjectClassModuleList().pop_back();
    if (objectClass.getIsReferencedByAnyModule())
      continue;
    _objectClassHandleAllocator.put(objectClass.getObjectClassHandle());
    ObjectClassHandleObjectClassMap::erase(objectClass);
  }

  while (!module.getUpdateRateModuleList().empty()) {
    UpdateRate& updateRate = module.getUpdateRateModuleList().back().getUpdateRate();
    module.getUpdateRateModuleList().pop_back();
    if (updateRate.getIsReferencedByAnyModule())
      continue;
    _updateRateHandleAllocator.put(updateRate.getUpdateRateHandle());
    UpdateRateHandleUpdateRateMap::erase(updateRate);
  }

  while (!module.getDimensionModuleList().empty()) {
    Dimension& dimension = module.getDimensionModuleList().back().getDimension();
    module.getDimensionModuleList().pop_back();
    if (dimension.getIsReferencedByAnyModule())
      continue;
    _dimensionHandleAllocator.put(dimension.getDimensionHandle());
    DimensionHandleDimensionMap::erase(dimension);
  }

  _moduleHandleAllocator.put(module.getModuleHandle());
  ModuleHandleModuleMap::erase(module);
}

void
Federation::getModuleList(FOMModuleList& moduleList) const
{
  moduleList.reserve(_moduleHandleModuleMap.size());
  for (ModuleHandleModuleMap::const_iterator i = _moduleHandleModuleMap.begin();
       i != _moduleHandleModuleMap.end(); ++i) {
    moduleList.push_back(FOMModule());
    i->getModule(moduleList.back());
  }
}

void
Federation::getModuleList(FOMModuleList& moduleList, ModuleHandleVector const& moduleHandleVector) const
{
  for (ModuleHandleVector::const_iterator i = moduleHandleVector.begin(); i != moduleHandleVector.end(); ++i) {
    ModuleHandleModuleMap::const_iterator j = _moduleHandleModuleMap.find(*i);
    OpenRTIAssert(j != _moduleHandleModuleMap.end());
    moduleList.push_back(FOMModule());
    j->getModule(moduleList.back());
  }
}

Synchronization const*
Federation::getSynchronization(std::string const& label) const
{
  SynchronizationNameSynchronizationMap::const_iterator i;
  i = _synchronizationNameSynchronizationMap.find(label);
  if (i == _synchronizationNameSynchronizationMap.end())
    return 0;
  return i.get();
}

Synchronization*
Federation::getSynchronization(std::string const& label)
{
  SynchronizationNameSynchronizationMap::iterator i;
  i = _synchronizationNameSynchronizationMap.find(label);
  if (i == _synchronizationNameSynchronizationMap.end())
    return 0;
  return i.get();
}

Federate const*
Federation::getFederate(FederateHandle const& federateHandle) const
{
  FederateHandleFederateMap::const_iterator i;
  i = _federateHandleFederateMap.find(federateHandle);
  if (i == _federateHandleFederateMap.end())
    return 0;
  return i.get();
}

Federate*
Federation::getFederate(FederateHandle const& federateHandle)
{
  FederateHandleFederateMap::iterator i;
  i = _federateHandleFederateMap.find(federateHandle);
  if (i == _federateHandleFederateMap.end())
    return 0;
  return i.get();
}

Federate const*
Federation::getFederate(std::string const& name) const
{
  FederateNameFederateMap::const_iterator i;
  i = _federateNameFederateMap.find(name);
  if (i == _federateNameFederateMap.end())
    return 0;
  return i.get();
}

Federate*
Federation::getFederate(std::string const& name)
{
  FederateNameFederateMap::iterator i;
  i = _federateNameFederateMap.find(name);
  if (i == _federateNameFederateMap.end())
    return 0;
  return i.get();
}

bool
Federation::isFederateNameInUse(std::string const& name) const
{
  return _federateNameFederateMap.find(name) != _federateNameFederateMap.end();
}

void
Federation::erase(Federate& federate)
{
  _federateHandleAllocator.put(federate.getFederateHandle());
  FederateHandleFederateMap::erase(federate);
}

Region*
Federation::getOrCreateRegion(RegionHandle const& regionHandle)
{
  FederateHandleFederateMap::iterator i = _federateHandleFederateMap.find(regionHandle.getFederateHandle());
  if (i == _federateHandleFederateMap.end())
    return 0;
  Region* region = i->getRegion(regionHandle.getLocalRegionHandle());
  if (region)
    return region;
  return new Region(*i, regionHandle.getLocalRegionHandle());
}

Region*
Federation::getRegion(RegionHandle const& regionHandle)
{
  FederateHandleFederateMap::iterator i;
  i = _federateHandleFederateMap.find(regionHandle.getFederateHandle());
  if (i == _federateHandleFederateMap.end())
    return 0;
  return i->getRegion(regionHandle.getLocalRegionHandle());
}

ObjectInstance const*
Federation::getObjectInstance(ObjectInstanceHandle const& objectInstanceHandle) const
{
  ObjectInstanceHandleObjectInstanceMap::const_iterator i;
  i = _objectInstanceHandleObjectInstanceMap.find(objectInstanceHandle);
  if (i == _objectInstanceHandleObjectInstanceMap.end())
    return 0;
  return i.get();
}

ObjectInstance*
Federation::getObjectInstance(ObjectInstanceHandle const& objectInstanceHandle)
{
  ObjectInstanceHandleObjectInstanceMap::iterator i;
  i = _objectInstanceHandleObjectInstanceMap.find(objectInstanceHandle);
  if (i == _objectInstanceHandleObjectInstanceMap.end())
    return 0;
  return i.get();
}

ObjectInstance const*
Federation::getObjectInstance(std::string const& name) const
{
  ObjectInstanceNameObjectInstanceMap::const_iterator i;
  i = _objectInstanceNameObjectInstanceMap.find(name);
  if (i == _objectInstanceNameObjectInstanceMap.end())
    return 0;
  return i.get();
}

ObjectInstance*
Federation::getObjectInstance(std::string const& name)
{
  ObjectInstanceNameObjectInstanceMap::iterator i;
  i = _objectInstanceNameObjectInstanceMap.find(name);
  if (i == _objectInstanceNameObjectInstanceMap.end())
    return 0;
  return i.get();
}

void
Federation::erase(ObjectInstance& objectInstance)
{
  _objectInstanceHandleAllocator.put(objectInstance.getObjectInstanceHandle());
  ObjectInstanceHandleObjectInstanceMap::erase(objectInstance);
}

bool
Federation::isObjectInstanceNameInUse(std::string const& name) const
{
  return _objectInstanceNameObjectInstanceMap.find(name) != _objectInstanceNameObjectInstanceMap.end();
}

Module*
Federation::createModule(ModuleHandle const& moduleHandle)
{
  return new Module(*this, moduleHandle);
}

Dimension*
Federation::createDimension(DimensionHandle const& dimensionHandle, std::string const& name)
{
  return new Dimension(*this, dimensionHandle, name);
}

UpdateRate*
Federation::createUpdateRate(UpdateRateHandle const& updateRateHandle, std::string const& name)
{
  return new UpdateRate(*this, updateRateHandle, name);
}

InteractionClass*
Federation::createInteractionClass(InteractionClassHandle const& interactionClassHandle, StringVector const& name, InteractionClass* parentInteractionClass)
{
  return new InteractionClass(*this, interactionClassHandle, name, parentInteractionClass);
}

ObjectClass*
Federation::createObjectClass(ObjectClassHandle const& objectClassHandle, StringVector const& name, ObjectClass* parentObjectClass)
{
  return new ObjectClass(*this, objectClassHandle, name, parentObjectClass);
}

Federate*
Federation::createFederate(FederateHandle const& federateHandle, std::string const& name)
{
  FederateHandle federateHandle2 = _federateHandleAllocator.getOrTake(federateHandle);
  if (name.empty())
    return new Federate(*this, federateHandle2, federateHandle2.getReservedName("HLAfederate"));
  else
    return new Federate(*this, federateHandle2, name);
}

ObjectInstance*
Federation::createObjectInstance(ObjectInstanceHandle const& objectInstanceHandle, std::string const& name)
{
  ObjectInstanceHandle objectInstanceHandle2 = _objectInstanceHandleAllocator.getOrTake(objectInstanceHandle);
  if (name.empty())
    return new ObjectInstance(*this, objectInstanceHandle2, objectInstanceHandle2.getReservedName("HLAobjectInstance"));
  else
    return new ObjectInstance(*this, objectInstanceHandle2, name);
}

void
Federation::_insertConnectHandleFederationConnectMap(FederationConnect& federationConnect)
{
  _connectHandleFederationConnectMap.insert(federationConnect);
}

void
Federation::_unlinkConnectHandleFederationConnectMap(FederationConnect& federationConnect)
{
  _connectHandleFederationConnectMap.unlink(federationConnect);
}

void
Federation::_insertTimeRegulatingFederationConnectList(FederationConnect& federationConnect)
{
  _timeRegulatingFederationConnectList.push_back(federationConnect);
}

void
Federation::_unlinkTimeRegulatingFederationConnectList(FederationConnect& federationConnect)
{
  _timeRegulatingFederationConnectList.unlink(federationConnect);
}

void
Federation::_insertModuleHandleModuleMap(Module& module)
{
  OpenRTIAssert(_moduleHandleModuleMap.find(module.getModuleHandle()) == _moduleHandleModuleMap.end());
  _moduleHandleModuleMap.insert(module);
}

void
Federation::_unlinkModuleHandleModuleMap(Module& module)
{
  _moduleHandleModuleMap.unlink(module);
}

void
Federation::_insertDimensionHandleDimensionMap(Dimension& dimension)
{
  _dimensionHandleDimensionMap.insert(dimension);
}

void
Federation::_unlinkDimensionHandleDimensionMap(Dimension& dimension)
{
  _dimensionHandleDimensionMap.unlink(dimension);
}

void
Federation::_insertDimensionNameDimensionMap(Dimension& dimension)
{
  _dimensionNameDimensionMap.insert(dimension);
}

void
Federation::_unlinkDimensionNameDimensionMap(Dimension& dimension)
{
  _dimensionNameDimensionMap.unlink(dimension);
}

void
Federation::_insertUpdateRateHandleUpdateRateMap(UpdateRate& updateRate)
{
  _updateRateHandleUpdateRateMap.insert(updateRate);
}

void
Federation::_unlinkUpdateRateHandleUpdateRateMap(UpdateRate& updateRate)
{
  _updateRateHandleUpdateRateMap.unlink(updateRate);
}

void
Federation::_insertUpdateRateNameUpdateRateMap(UpdateRate& updateRate)
{
  _updateRateNameUpdateRateMap.insert(updateRate);
}

void
Federation::_unlinkUpdateRateNameUpdateRateMap(UpdateRate& updateRate)
{
  _updateRateNameUpdateRateMap.unlink(updateRate);
}

void
Federation::_insertInteractionClassHandleInteractionClassMap(InteractionClass& interactionClass)
{
  _interactionClassHandleInteractionClassMap.insert(interactionClass);
}

void
Federation::_unlinkInteractionClassHandleInteractionClassMap(InteractionClass& interactionClass)
{
  _interactionClassHandleInteractionClassMap.unlink(interactionClass);
}

void
Federation::_insertInteractionClassNameInteractionClassMap(InteractionClass& interactionClass)
{
  _interactionClassNameInteractionClassMap.insert(interactionClass);
}

void
Federation::_unlinkInteractionClassNameInteractionClassMap(InteractionClass& interactionClass)
{
  _interactionClassNameInteractionClassMap.unlink(interactionClass);
}

void
Federation::_insertObjectClassHandleObjectClassMap(ObjectClass& objectClass)
{
  _objectClassHandleObjectClassMap.insert(objectClass);
}

void
Federation::_unlinkObjectClassHandleObjectClassMap(ObjectClass& objectClass)
{
  _objectClassHandleObjectClassMap.unlink(objectClass);
}

void
Federation::_insertObjectClassNameObjectClassMap(ObjectClass& objectClass)
{
  _objectClassNameObjectClassMap.insert(objectClass);
}

void
Federation::_unlinkObjectClassNameObjectClassMap(ObjectClass& objectClass)
{
  _objectClassNameObjectClassMap.unlink(objectClass);
}

void
Federation::_insertSynchronizationNameSynchronizationMap(Synchronization& synchronization)
{
  _synchronizationNameSynchronizationMap.insert(synchronization);
}

void
Federation::_unlinkSynchronizationNameSynchronizationMap(Synchronization& synchronization)
{
  _synchronizationNameSynchronizationMap.unlink(synchronization);
}

void
Federation::_insertFederateHandleFederateMap(Federate& federate)
{
  _federateHandleFederateMap.insert(federate);
}

void
Federation::_unlinkFederateHandleFederateMap(Federate& federate)
{
  _federateHandleFederateMap.unlink(federate);
}

void
Federation::_insertFederateNameFederateMap(Federate& federate)
{
  _federateNameFederateMap.insert(federate);
}

void
Federation::_unlinkFederateNameFederateMap(Federate& federate)
{
  _federateNameFederateMap.unlink(federate);
}

void
Federation::_insertObjectInstanceHandleObjectInstanceMap(ObjectInstance& objectInstance)
{
  _objectInstanceHandleObjectInstanceMap.insert(objectInstance);
}

void
Federation::_unlinkObjectInstanceHandleObjectInstanceMap(ObjectInstance& objectInstance)
{
  _objectInstanceHandleObjectInstanceMap.unlink(objectInstance);
}

void
Federation::_insertObjectInstanceNameObjectInstanceMap(ObjectInstance& objectInstance)
{
  _objectInstanceNameObjectInstanceMap.insert(objectInstance);
}

void
Federation::_unlinkObjectInstanceNameObjectInstanceMap(ObjectInstance& objectInstance)
{
  _objectInstanceNameObjectInstanceMap.unlink(objectInstance);
}

} // namespace ServerModel
} // namespace OpenRTI
