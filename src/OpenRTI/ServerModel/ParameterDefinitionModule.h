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

#ifndef OpenRTI_ServerModel_ParameterDefinitionModule_h
#define OpenRTI_ServerModel_ParameterDefinitionModule_h

#include "OpenRTIConfig.h"

#include "Intrusive.h"

namespace OpenRTI {
namespace ServerModel {

class InteractionClass;
class Module;

class OPENRTI_LOCAL ParameterDefinitionModule :
    public Intrusive::ListLink<ParameterDefinitionModule, Intrusive::ParentTag<Module> >,
    public Intrusive::ListLink<ParameterDefinitionModule, Intrusive::ParentTag<InteractionClass> >
{
public:
  ParameterDefinitionModule(InteractionClass& interactionClass, Module& module);
  ~ParameterDefinitionModule();

  InteractionClass const& getInteractionClass() const
  { return _interactionClass; }
  InteractionClass& getInteractionClass()
  { return _interactionClass; }

  Module const& getModule() const
  { return _module; }
  Module& getModule()
  { return _module; }

private:
#if 201103L <= __cplusplus
  ParameterDefinitionModule(ParameterDefinitionModule const&) = delete;
  ParameterDefinitionModule(ParameterDefinitionModule&&) = delete;
  ParameterDefinitionModule& operator=(ParameterDefinitionModule const&) = delete;
  ParameterDefinitionModule& operator=(ParameterDefinitionModule&&) = delete;
#else
  ParameterDefinitionModule(ParameterDefinitionModule const&);
  ParameterDefinitionModule& operator=(ParameterDefinitionModule const&);
#if 200610L <= __cpp_rvalue_reference
  ParameterDefinitionModule(ParameterDefinitionModule&&);
  ParameterDefinitionModule& operator=(ParameterDefinitionModule&&);
#endif
#endif

  InteractionClass& _interactionClass;

  Module& _module;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_ParameterDefinitionModule_h
