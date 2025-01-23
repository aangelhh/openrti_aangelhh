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

#include "ParameterDefinition.h"

#include "ClassParameter.h"
#include "InteractionClass.h"

namespace OpenRTI {
namespace ServerModel {

ParameterDefinition::ParameterDefinition(InteractionClass& interactionClass, ParameterHandle const& parameterHandle, std::string const& name) :
  _interactionClass(interactionClass),
  _parameterHandle(parameterHandle),
  _name(name)
{
  _interactionClass._insertParameterHandleParameterDefinitionMap(*this);
  _interactionClass._insertParameterNameParameterDefinitionMap(*this);
}

ParameterDefinition::~ParameterDefinition()
{
  _classParameterList.clear();

  _interactionClass._unlinkParameterNameParameterDefinitionMap(*this);
  _interactionClass._unlinkParameterHandleParameterDefinitionMap(*this);

  OpenRTIAssert(_classParameterList.empty());
}

void
ParameterDefinition::_insertClassParameterList(ClassParameter& classParameter)
{
  _classParameterList.push_back(classParameter);
}

void
ParameterDefinition::_unlinkClassParameterList(ClassParameter& classParameter)
{
  _classParameterList.unlink(classParameter);
}

} // namespace ServerModel
} // namespace OpenRTI
