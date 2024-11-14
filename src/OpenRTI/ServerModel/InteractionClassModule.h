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

#ifndef OpenRTI_ServerModel_InteractionClassModule_h
#define OpenRTI_ServerModel_InteractionClassModule_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

namespace OpenRTI {
namespace ServerModel {

class InteractionClass;
class Module;

class OPENRTI_LOCAL InteractionClassModule :
    public IntrusiveList<InteractionClassModule, 0>::Hook,
    public IntrusiveList<InteractionClassModule, 1>::Hook
{
public:
  typedef IntrusiveList<InteractionClassModule, 0> FirstList;
  typedef IntrusiveList<InteractionClassModule, 1> SecondList;

  InteractionClassModule(InteractionClass& interactionClass, Module& module);
  ~InteractionClassModule();

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
  InteractionClassModule(InteractionClassModule const&) = delete;
  InteractionClassModule(InteractionClassModule&&) = delete;
  InteractionClassModule& operator=(InteractionClassModule const&) = delete;
  InteractionClassModule& operator=(InteractionClassModule&&) = delete;
#else
  InteractionClassModule(InteractionClassModule const&);
  InteractionClassModule& operator=(InteractionClassModule const&);
#if 200610L <= __cpp_rvalue_reference
  InteractionClassModule(InteractionClassModule&&);
  InteractionClassModule& operator=(InteractionClassModule&&);
#endif
#endif

  InteractionClass& _interactionClass;

  Module& _module;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_InteractionClassModule_h
