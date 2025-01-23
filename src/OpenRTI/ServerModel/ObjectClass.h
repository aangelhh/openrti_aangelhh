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

#ifndef OpenRTI_ServerModel_ObjectClass_h
#define OpenRTI_ServerModel_ObjectClass_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

#include "ClassAttribute.h"
#include "Handle.h"
#include "InstanceAttribute.h"
#include "ObjectInstance.h"
#include "StringUtils.h"

namespace OpenRTI {
namespace ServerModel {

class AttributeDefinition;
class AttributeDefinitionModule;
class ClassAttribute;
class Federation;
class ObjectClassModule;
class ObjectInstance;

class OPENRTI_LOCAL ObjectClass :
    public Intrusive::UnorderedSetLink<ObjectClass, Intrusive::ParentTag<Federation> >,
    public Intrusive::UnorderedSetLink<ObjectClass, Intrusive::ParentTag<Federation, 1> >,
    public Intrusive::ListLink<ObjectClass, Intrusive::ParentTag<ObjectClass> >
{
public:
  ObjectClass(Federation& federation, ObjectClassHandle const& objectClassHandle, StringVector const& name, ObjectClass* parentObjectClass = 0);
  ~ObjectClass();

  Federation const& getFederation() const
  { return _federation; }
  Federation& getFederation()
  { return _federation; }

  ObjectClassHandle const& getObjectClassHandle() const
  { return _objectClassHandle; }

  StringVector const& getName() const
  { return _name; }

  ObjectClass const* getParentObjectClass() const
  { return _parentObjectClass; }
  ObjectClass* getParentObjectClass()
  { return _parentObjectClass; }
  ObjectClassHandle getParentObjectClassHandle() const;

  /// List of ObjectClass instances belonging to this ObjectClass
  typedef Intrusive::List<Intrusive::ListLink<ObjectClass, Intrusive::ParentTag<ObjectClass> > > ChildObjectClassList;
  /// Get the list of ObjectClass instances
  ChildObjectClassList const& getChildObjectClassList() const
  { return _childObjectClassList; }
  ChildObjectClassList& getChildObjectClassList()
  { return _childObjectClassList; }

  /// The list of Modules referencing this ObjectClass
  typedef Intrusive::List<Intrusive::ListLink<ObjectClassModule, Intrusive::ParentTag<ObjectClass> > > ObjectClassModuleList;
  /// Get the list of ObjectClassModule instances
  ObjectClassModuleList const& getObjectClassModuleList() const
  { return _objectClassModuleList; }
  ObjectClassModuleList& getObjectClassModuleList()
  { return _objectClassModuleList; }
  void insert(ObjectClassModule& objectClassModule);
  bool getIsReferencedByAnyModule() const;

  /// UnorderedSet of AttributeDefinition instances indexed by attributeHandle
  typedef Intrusive::UnorderedSet<AttributeHandle, Intrusive::UnorderedSetLink<AttributeDefinition, Intrusive::ParentTag<ObjectClass> > > AttributeHandleAttributeDefinitionMap;
  /// Get the set of AttributeDefinition instances
  AttributeHandleAttributeDefinitionMap const& getAttributeHandleAttributeDefinitionMap() const
  { return _attributeHandleAttributeDefinitionMap; }
  AttributeHandleAttributeDefinitionMap& getAttributeHandleAttributeDefinitionMap()
  { return _attributeHandleAttributeDefinitionMap; }
  /// Get one AttributeDefinition instance matching attributeHandle
  AttributeDefinition const* getAttributeDefinition(AttributeHandle const& attributeHandle) const;
  AttributeDefinition* getAttributeDefinition(AttributeHandle const& attributeHandle);

  /// UnorderedSet of AttributeDefinition instances indexed by name
  typedef Intrusive::UnorderedSet<std::string, Intrusive::UnorderedSetLink<AttributeDefinition, Intrusive::ParentTag<ObjectClass, 1> > > AttributeNameAttributeDefinitionMap;
  /// Get one AttributeDefinition instance matching name
  AttributeDefinition const* getAttributeDefinition(std::string const& name) const;
  AttributeDefinition* getAttributeDefinition(std::string const& name);

  void eraseAttributeDefinitions();
  std::size_t getNumAttributeDefinitions() const;
  AttributeHandle getFirstUnusedAttributeHandle();

  void insert(AttributeDefinition& attributeDefinition);

  /// The list of Modules referencing this ObjectClass set of AttributeDefinitions
  typedef Intrusive::List<Intrusive::ListLink<AttributeDefinitionModule, Intrusive::ParentTag<ObjectClass> > > AttributeDefinitionModuleList;
  /// Get the list of AttributeDefinitionModule instances
  AttributeDefinitionModuleList const& getAttributeDefinitionModuleList() const
  { return _attributeDefinitionModuleList; }
  AttributeDefinitionModuleList& getAttributeDefinitionModuleList()
  { return _attributeDefinitionModuleList; }
  void insert(AttributeDefinitionModule& attributeDefinitionModule);
  bool getAreAttributesReferencedByAnyModule() const;

  /// UnorderedSet of ClassAttribute instances indexed by attributeHandle
  typedef Intrusive::UnorderedSet<AttributeHandle, Intrusive::UnorderedSetLink<ClassAttribute, Intrusive::ParentTag<ObjectClass> > > AttributeHandleClassAttributeMap;
  /// Get the set of ClassAttribute instances
  AttributeHandleClassAttributeMap const& getAttributeHandleClassAttributeMap() const
  { return _attributeHandleClassAttributeMap; }
  AttributeHandleClassAttributeMap& getAttributeHandleClassAttributeMap()
  { return _attributeHandleClassAttributeMap; }
  /// Get one ClassAttribute instance matching attributeHandle
  ClassAttribute const* getClassAttribute(AttributeHandle const& attributeHandle) const;
  ClassAttribute* getClassAttribute(AttributeHandle const& attributeHandle);
  ClassAttribute* getPrivilegeToDeleteClassAttribute();
  void insertClassAttributeFor(AttributeDefinition& attributeDefinition);

  /// List of ObjectInstance instances belonging to this ObjectClass
  typedef IntrusiveList<ObjectInstance, 0> ObjectInstanceList;
  /// Get the list of ObjectInstance instances
  ObjectInstanceList const& getObjectInstanceList() const
  { return _objectInstanceList; }
  ObjectInstanceList& getObjectInstanceList()
  { return _objectInstanceList; }
  void insert(ObjectInstance& objectInstance);


  void removeConnect(ConnectHandle const& connectHandle);

  typedef std::list<ObjectInstance*> ObjectInstanceList2;

  /// since we might end in different depths for different attributes, this is done per attribute
  void updateCumulativeSubscription(ConnectHandle const& connectHandle, AttributeHandle const& attributeHandle,
                                    ObjectInstanceList2& objectInstanceList)
  {
    bool parentSubscribed = false;
    if (_parentObjectClass) {
      ClassAttribute* classAttribute = _parentObjectClass->getClassAttribute(attributeHandle);
      if (classAttribute)
        if (0 != classAttribute->_cumulativeSubscribedConnectHandleSet.count(connectHandle))
          parentSubscribed = true;
    }

    _updateCumulativeSubscription(connectHandle, attributeHandle, parentSubscribed, objectInstanceList);
  }
  void _updateCumulativeSubscription(ConnectHandle const& connectHandle, AttributeHandle const& attributeHandle,
                                     bool subscribe /*Replace with regionset or something*/, ObjectInstanceList2& objectInstanceList)
  {
    ClassAttribute* classAttribute = getClassAttribute(attributeHandle);
    subscribe |= (Unsubscribed != classAttribute->getSubscriptionType(connectHandle));
    if (!classAttribute->updateCumulativeSubscribedConnectHandleSet(connectHandle, subscribe))
      return;
    // Update the receiving connect handle set
    for (ChildObjectClassList::iterator i = _childObjectClassList.begin(); i != _childObjectClassList.end(); ++i) {
      i->_updateCumulativeSubscription(connectHandle, attributeHandle, subscribe, objectInstanceList);
    }
    /// FIXME: need to walk the objects and see how the routing for the object changes
    /// FIXME: store the object instances that are yet unknown to a connect and store these to propagate them into the connect
    /// Hmm, here is the first good use case for a visitor
    for (ObjectInstanceList::iterator i = _objectInstanceList.begin(); i != _objectInstanceList.end(); ++i) {
      InstanceAttribute* instanceAttribute = i->getInstanceAttribute(attributeHandle);
      if (!instanceAttribute)
        continue;

      // Don't add the owner to the list of connect handles that receive this attribute
      if (instanceAttribute->getOwnerConnectHandle() == connectHandle)
        continue;

      if (subscribe) {
        // Insert the connect handle into the receiving connects
        if (!instanceAttribute->_receivingConnects.insert(connectHandle).second)
          continue;

        // Note that we need to insert this object instance into this connect
        if (attributeHandle == AttributeHandle(0))
          objectInstanceList.push_back(i.get());

      } else {
        // Never remove a attribute 0 subsciption as pushing the instance information may race
        if (attributeHandle == AttributeHandle(0))
          continue;

        // Erase the connect handle from the receiving connects
        if (instanceAttribute->_receivingConnects.erase(connectHandle) == 0)
          continue;
      }
    }
  }

  void accumulateAllPublications(ConnectHandleSet& connectHandleSet)
  {
    connectHandleSet.insert(getPrivilegeToDeleteClassAttribute()->getPublishingConnectHandleSet().begin(),
                            getPrivilegeToDeleteClassAttribute()->getPublishingConnectHandleSet().end());
    for (ChildObjectClassList::iterator i = getChildObjectClassList().begin(); i != getChildObjectClassList().end(); ++i) {
      i->accumulateAllPublications(connectHandleSet);
    }
  }

  template<typename Link>
  struct IntrusiveKey;

private:
#if 201103L <= __cplusplus
  ObjectClass(ObjectClass const&) = delete;
  ObjectClass(ObjectClass&&) = delete;
  ObjectClass& operator=(ObjectClass const&) = delete;
  ObjectClass& operator=(ObjectClass&&) = delete;
#else
  ObjectClass(ObjectClass const&);
  ObjectClass& operator=(ObjectClass const&);
#if 200610L <= __cpp_rvalue_reference
  ObjectClass(ObjectClass&&);
  ObjectClass& operator=(ObjectClass&&);
#endif
#endif

  Federation& _federation;

  ObjectClassHandle const _objectClassHandle;

  StringVector const _name;

  ObjectClass* const _parentObjectClass;

  /// List of ObjectClass instances belonging to this ObjectClass
  ChildObjectClassList _childObjectClassList;

  /// The list of Modules referencing this ObjectClass
  ObjectClassModuleList _objectClassModuleList;

  /// UnorderedSet of AttributeDefinition instances indexed by attributeHandle
  AttributeHandleAttributeDefinitionMap _attributeHandleAttributeDefinitionMap;

  /// UnorderedSet of AttributeDefinition instances indexed by name
  AttributeNameAttributeDefinitionMap _attributeNameAttributeDefinitionMap;

  /// The list of Modules referencing this ObjectClass set of AttributeDefinitions
  AttributeDefinitionModuleList _attributeDefinitionModuleList;

  /// UnorderedSet of ClassAttribute instances indexed by attributeHandle
  AttributeHandleClassAttributeMap _attributeHandleClassAttributeMap;

  /// List of ObjectInstance instances belonging to this ObjectClass
  ObjectInstanceList _objectInstanceList;
};

template<>
struct ObjectClass::IntrusiveKey<Intrusive::UnorderedSetLink<ObjectClass, Intrusive::ParentTag<Federation> > > {
  static ObjectClassHandle const& get(ObjectClass const& objectClass)
  { return objectClass.getObjectClassHandle(); }
};

template<>
struct ObjectClass::IntrusiveKey<Intrusive::UnorderedSetLink<ObjectClass, Intrusive::ParentTag<Federation, 1> > > {
  static StringVector const& get(ObjectClass const& objectClass)
  { return objectClass.getName(); }
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_ObjectClass_h
