/* -*-c++-*- OpenRTI - Copyright (C) 2009-2026 Mathias Froehlich
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

#include "Module.h"

#include "AttributeDefinition.h"
#include "AttributeDefinitionModule.h"
#include "Dimension.h"
#include "DimensionModule.h"
#include "Federation.h"
#include "InteractionClass.h"
#include "InteractionClassModule.h"
#include "ObjectClass.h"
#include "ObjectClassModule.h"
#include "ParameterDefinition.h"
#include "ParameterDefinitionModule.h"
#include "UpdateRate.h"
#include "UpdateRateModule.h"

namespace OpenRTI {
namespace ServerModel {

Module::Module(Federation& federation, ModuleHandle const& moduleHandle) :
  _federation(federation),
  _moduleHandle(moduleHandle),
  _artificialInteractionRoot(false),
  _artificialObjectRoot(false)
{
  _federation._insertModuleHandleModuleMap(*this);
}

Module::~Module()
{
  _federation._unlinkModuleHandleModuleMap(*this);

  OpenRTIAssert(_attributeDefinitionModuleList.empty());
  OpenRTIAssert(_objectClassModuleList.empty());
  OpenRTIAssert(_parameterDefinitionModuleList.empty());
  OpenRTIAssert(_interactionClassModuleList.empty());
  OpenRTIAssert(_updateRateModuleList.empty());
  OpenRTIAssert(_dimensionModuleList.empty());
}

void
Module::setContent(std::string const& content)
{
  _content = content;
}

void
Module::setArtificialInteractionRoot(bool artificialInteractionRoot)
{
  _artificialInteractionRoot = artificialInteractionRoot;
}

void
Module::setArtificialObjectRoot(bool artificialObjectRoot)
{
  _artificialObjectRoot = artificialObjectRoot;
}

void
Module::getModule(FOMModule& module) const
{
  module.setModuleHandle(getModuleHandle());
  /// FIXME
  // FOMSwitchList _switchList;
  module.setArtificialInteractionRoot(getArtificialInteractionRoot());
  module.setArtificialObjectRoot(getArtificialObjectRoot());
  module.setContent(getContent());

  module.getDimensionList().reserve(_dimensionModuleList.size());
  for (DimensionModuleList::const_iterator i = _dimensionModuleList.begin();
       i != _dimensionModuleList.end(); ++i) {
    const Dimension& dimension = i->getDimension();
    module.getDimensionList().push_back(FOMDimension());
    FOMDimension& fomDimension = module.getDimensionList().back();
    fomDimension.setName(dimension.getName());
    fomDimension.setDimensionHandle(dimension.getDimensionHandle());
    fomDimension.setUpperBound(dimension.getUpperBound());
  }

  module.getUpdateRateList().reserve(_updateRateModuleList.size());
  for (UpdateRateModuleList::const_iterator i = _updateRateModuleList.begin();
       i != _updateRateModuleList.end(); ++i) {
    const UpdateRate& updateRate = i->getUpdateRate();
    module.getUpdateRateList().push_back(FOMUpdateRate());
    FOMUpdateRate& fomUpdateRate = module.getUpdateRateList().back();
    fomUpdateRate.setName(updateRate.getName());
    fomUpdateRate.setUpdateRateHandle(updateRate.getUpdateRateHandle());
    fomUpdateRate.setRate(updateRate.getRate());
  }

  {
    module.getInteractionClassList().reserve(_interactionClassModuleList.size());
    ParameterDefinitionModuleList::const_iterator j = _parameterDefinitionModuleList.begin();
    for (InteractionClassModuleList::const_iterator i = _interactionClassModuleList.begin();
         i != _interactionClassModuleList.end(); ++i) {
      const InteractionClass& interactionClass = i->getInteractionClass();
      module.getInteractionClassList().push_back(FOMInteractionClass());
      FOMInteractionClass& fomInteractionClass = module.getInteractionClassList().back();
      fomInteractionClass.setName(interactionClass.getName().back());
      fomInteractionClass.setInteractionClassHandle(interactionClass.getInteractionClassHandle());
      fomInteractionClass.setParentInteractionClassHandle(interactionClass.getParentInteractionClassHandle());
      fomInteractionClass.setOrderType(interactionClass.getOrderType());
      fomInteractionClass.setTransportationType(interactionClass.getTransportationType());
      fomInteractionClass.setDimensionHandleSet(interactionClass._dimensionHandleSet);

      // Check if we also reference the parameters with this module
      if (j == _parameterDefinitionModuleList.end())
        continue;
      if (interactionClass.getInteractionClassHandle() != j->getInteractionClass().getInteractionClassHandle())
        continue;
      // If so, add them too
      ++j;
      fomInteractionClass.getParameterList().reserve(interactionClass.getNumParameterDefinitions());
      for (InteractionClass::ParameterHandleParameterDefinitionMap::const_iterator k = interactionClass.getParameterHandleParameterDefinitionMap().begin();
           k != interactionClass.getParameterHandleParameterDefinitionMap().end(); ++k) {
        fomInteractionClass.getParameterList().push_back(FOMParameter());
        FOMParameter& fomParameter = fomInteractionClass.getParameterList().back();
        fomParameter.setName(k->getName());
        fomParameter.setParameterHandle(k->getParameterHandle());
      }
    }
  }

  {
    module.getObjectClassList().reserve(_objectClassModuleList.size());
    AttributeDefinitionModuleList::const_iterator j = _attributeDefinitionModuleList.begin();
    for (ObjectClassModuleList::const_iterator i = _objectClassModuleList.begin();
         i != _objectClassModuleList.end(); ++i) {
      const ObjectClass& objectClass = i->getObjectClass();
      module.getObjectClassList().push_back(FOMObjectClass());
      FOMObjectClass& fomObjectClass = module.getObjectClassList().back();
      fomObjectClass.setName(objectClass.getName().back());
      fomObjectClass.setObjectClassHandle(objectClass.getObjectClassHandle());
      fomObjectClass.setParentObjectClassHandle(objectClass.getParentObjectClassHandle());

      // Check if we also reference the attributes with this module
      if (j == _attributeDefinitionModuleList.end())
        continue;
      if (objectClass.getObjectClassHandle() != j->getObjectClass().getObjectClassHandle())
        continue;
      // If so, add them too
      ++j;
      fomObjectClass.getAttributeList().reserve(objectClass.getNumAttributeDefinitions());
      for (ObjectClass::AttributeHandleAttributeDefinitionMap::const_iterator k = objectClass.getAttributeHandleAttributeDefinitionMap().begin();
           k != objectClass.getAttributeHandleAttributeDefinitionMap().end(); ++k) {
        fomObjectClass.getAttributeList().push_back(FOMAttribute());
        FOMAttribute& fomAttribute = fomObjectClass.getAttributeList().back();
        fomAttribute.setName(k->getName());
        fomAttribute.setAttributeHandle(k->getAttributeHandle());
        fomAttribute.setOrderType(k->getOrderType());
        fomAttribute.setTransportationType(k->getTransportationType());
        fomAttribute.setDimensionHandleSet(k->_dimensionHandleSet);
      }
    }
  }
}

DimensionModule*
Module::createDimensionModule(Dimension& dimension)
{
  return new DimensionModule(dimension, *this);
}

UpdateRateModule*
Module::createUpdateRateModule(UpdateRate& updateRate)
{
  return new UpdateRateModule(updateRate, *this);
}

InteractionClassModule*
Module::createInteractionClassModule(InteractionClass& interactionClass)
{
  return new InteractionClassModule(interactionClass, *this);
}

ParameterDefinitionModule*
Module::createParameterDefinitionModule(InteractionClass& interactionClass)
{
  return new ParameterDefinitionModule(interactionClass, *this);
}

ObjectClassModule*
Module::createObjectClassModule(ObjectClass& objectClass)
{
  return new ObjectClassModule(objectClass, *this);
}

AttributeDefinitionModule*
Module::createAttributeDefinitionModule(ObjectClass& objectClass)
{
  return new AttributeDefinitionModule(objectClass, *this);
}

void
Module::_insertDimensionModuleList(DimensionModule& dimensionModule)
{
  _dimensionModuleList.push_back(dimensionModule);
}

void
Module::_unlinkDimensionModuleList(DimensionModule& dimensionModule)
{
  _dimensionModuleList.unlink(dimensionModule);
}

void
Module::_insertUpdateRateModuleList(UpdateRateModule& updateRateModule)
{
  _updateRateModuleList.push_back(updateRateModule);
}

void
Module::_unlinkUpdateRateModuleList(UpdateRateModule& updateRateModule)
{
  _updateRateModuleList.unlink(updateRateModule);
}

void
Module::_insertInteractionClassModuleList(InteractionClassModule& interactionClassModule)
{
  _interactionClassModuleList.push_back(interactionClassModule);
}

void
Module::_unlinkInteractionClassModuleList(InteractionClassModule& interactionClassModule)
{
  _interactionClassModuleList.unlink(interactionClassModule);
}

void
Module::_insertParameterDefinitionModuleList(ParameterDefinitionModule& parameterDefinitionModule)
{
  _parameterDefinitionModuleList.push_back(parameterDefinitionModule);
}

void
Module::_unlinkParameterDefinitionModuleList(ParameterDefinitionModule& parameterDefinitionModule)
{
  _parameterDefinitionModuleList.unlink(parameterDefinitionModule);
}

void
Module::_insertObjectClassModuleList(ObjectClassModule& objectClassModule)
{
  _objectClassModuleList.push_back(objectClassModule);
}

void
Module::_unlinkObjectClassModuleList(ObjectClassModule& objectClassModule)
{
  _objectClassModuleList.unlink(objectClassModule);
}

void
Module::_insertAttributeDefinitionModuleList(AttributeDefinitionModule& attributeDefinitionModule)
{
  _attributeDefinitionModuleList.push_back(attributeDefinitionModule);
}

void
Module::_unlinkAttributeDefinitionModuleList(AttributeDefinitionModule& attributeDefinitionModule)
{
  _attributeDefinitionModuleList.unlink(attributeDefinitionModule);
}

} // namespace ServerModel
} // namespace OpenRTI
