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

#include "ParameterDefinitionModule.h"

#include "InteractionClass.h"
#include "Module.h"

namespace OpenRTI {
namespace ServerModel {

ParameterDefinitionModule::ParameterDefinitionModule(InteractionClass& interactionClass, Module& module) :
  _interactionClass(interactionClass),
  _module(module)
{
  _module._insertParameterDefinitionModuleList(*this);
  _interactionClass._insertParameterDefinitionModuleList(*this);
}

ParameterDefinitionModule::~ParameterDefinitionModule()
{
  _interactionClass._unlinkParameterDefinitionModuleList(*this);
  _module._unlinkParameterDefinitionModuleList(*this);
}

} // namespace ServerModel
} // namespace OpenRTI
