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

#ifndef OpenRTI_ServerModel_ClassAttribute_h
#define OpenRTI_ServerModel_ClassAttribute_h

#include "Intrusive.h"

#include "Handle.h"
#include "PublishSubscribe.h"

namespace OpenRTI {
namespace ServerModel {

class AttributeDefinition;
class ObjectClass;

class OPENRTI_LOCAL ClassAttribute :
    public Intrusive::UnorderedSetLink<ClassAttribute, Intrusive::ParentTag<ObjectClass> >,
    public Intrusive::ListLink<ClassAttribute, Intrusive::ParentTag<AttributeDefinition> >,
    public PublishSubscribe
{
public:
  ClassAttribute(ObjectClass& objectClass, AttributeDefinition& attributeDefinition);
  ~ClassAttribute();

  ObjectClass const& getObjectClass() const
  { return _objectClass; }
  ObjectClass& getObjectClass()
  { return _objectClass; }

  AttributeDefinition const& getAttributeDefinition() const
  { return _attributeDefinition; }
  AttributeDefinition& getAttributeDefinition()
  { return _attributeDefinition; }

  AttributeHandle const& getAttributeHandle() const
  { return _attributeHandle; }

  template<typename Link>
  struct IntrusiveKey;

private:
#if 201103L <= __cplusplus
  ClassAttribute(ClassAttribute const&) = delete;
  ClassAttribute(ClassAttribute&&) = delete;
  ClassAttribute& operator=(ClassAttribute const&) = delete;
  ClassAttribute& operator=(ClassAttribute&&) = delete;
#else
  ClassAttribute(ClassAttribute const&);
  ClassAttribute& operator=(ClassAttribute const&);
#if 200610L <= __cpp_rvalue_reference
  ClassAttribute(ClassAttribute&&);
  ClassAttribute& operator=(ClassAttribute&&);
#endif
#endif

  ObjectClass& _objectClass;

  AttributeDefinition& _attributeDefinition;

  AttributeHandle const _attributeHandle;
};

template<>
struct ClassAttribute::IntrusiveKey<Intrusive::UnorderedSetLink<ClassAttribute, Intrusive::ParentTag<ObjectClass> > > {
  static AttributeHandle const& get(ClassAttribute const& classAttribute)
  { return classAttribute.getAttributeHandle(); }
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_ClassAttribute_h
