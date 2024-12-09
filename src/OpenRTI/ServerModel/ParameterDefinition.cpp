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

#include "ParameterDefinition.h"

namespace OpenRTI {
namespace ServerModel {

ParameterDefinition::ParameterDefinition(InteractionClass& interactionClass, ParameterHandle const& parameterHandle, std::string const& name) :
  IntrusiveUnorderedMap<ParameterHandle const, ParameterDefinition>::Hook(parameterHandle),
  IntrusiveUnorderedMap<std::string const, ParameterDefinition>::Hook(name),
  _interactionClass(interactionClass)
{
}

ParameterDefinition::~ParameterDefinition()
{
  _classParameterList.clear();

  OpenRTIAssert(_classParameterList.empty());
}

} // namespace ServerModel
} // namespace OpenRTI
