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

#ifndef OpenRTI_ServerModel_AttributeDefinitionModule_h
#define OpenRTI_ServerModel_AttributeDefinitionModule_h

#include "Intrusive.h"

namespace OpenRTI {
namespace ServerModel {

class Module;
class ObjectClass;

class OPENRTI_LOCAL AttributeDefinitionModule :
    public Intrusive::ListLink<AttributeDefinitionModule, Intrusive::ParentTag<Module> >,
    public Intrusive::ListLink<AttributeDefinitionModule, Intrusive::ParentTag<ObjectClass> >
{
public:
  AttributeDefinitionModule(ObjectClass& objectClass, Module& module);
  ~AttributeDefinitionModule();

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
  AttributeDefinitionModule(AttributeDefinitionModule const&) = delete;
  AttributeDefinitionModule(AttributeDefinitionModule&&) = delete;
  AttributeDefinitionModule& operator=(AttributeDefinitionModule const&) = delete;
  AttributeDefinitionModule& operator=(AttributeDefinitionModule&&) = delete;
#else
  AttributeDefinitionModule(AttributeDefinitionModule const&);
  AttributeDefinitionModule& operator=(AttributeDefinitionModule const&);
#if 200610L <= __cpp_rvalue_reference
  AttributeDefinitionModule(AttributeDefinitionModule&&);
  AttributeDefinitionModule& operator=(AttributeDefinitionModule&&);
#endif
#endif

  ObjectClass& _objectClass;

  Module& _module;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_AttributeDefinitionModule_h
