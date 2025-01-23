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

#ifndef OpenRTI_ServerModel_ObjectClassModule_h
#define OpenRTI_ServerModel_ObjectClassModule_h

#include "OpenRTIConfig.h"

#include "Intrusive.h"

namespace OpenRTI {
namespace ServerModel {

class Module;
class ObjectClass;

class OPENRTI_LOCAL ObjectClassModule :
    public Intrusive::ListLink<ObjectClassModule, Intrusive::ParentTag<Module> >,
    public Intrusive::ListLink<ObjectClassModule, Intrusive::ParentTag<ObjectClass> >
{
public:
  ObjectClassModule(ObjectClass& objectClass, Module& module);
  ~ObjectClassModule();

  ObjectClass const& getObjectClass() const
  { return _objectClass; }
  ObjectClass& getObjectClass()
  { return _objectClass; }

  Module const& getModule() const
  { return _module; }
  Module& getModule()
  { return _module; }

private:
#if 201103L <= __cplusplus
  ObjectClassModule(ObjectClassModule const&) = delete;
  ObjectClassModule(ObjectClassModule&&) = delete;
  ObjectClassModule& operator=(ObjectClassModule const&) = delete;
  ObjectClassModule& operator=(ObjectClassModule&&) = delete;
#else
  ObjectClassModule(ObjectClassModule const&);
  ObjectClassModule& operator=(ObjectClassModule const&);
#if 200610L <= __cpp_rvalue_reference
  ObjectClassModule(ObjectClassModule&&);
  ObjectClassModule& operator=(ObjectClassModule&&);
#endif
#endif

  ObjectClass& _objectClass;

  Module& _module;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_ObjectClassModule_h
