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

#ifndef OpenRTI_ServerModel_ObjectInstance_h
#define OpenRTI_ServerModel_ObjectInstance_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

#include "Handle.h"
#include "ObjectInstanceConnect.h"
#include "ServerModel.h"

namespace OpenRTI {
namespace ServerModel {

class Federation;
class InstanceAttribute;
class ObjectClass;

class OPENRTI_LOCAL ObjectInstance :
    public IntrusiveUnorderedMap<ObjectInstanceHandle, ObjectInstance>::Hook,
    public IntrusiveUnorderedMap<std::string, ObjectInstance>::Hook,
    public IntrusiveList<ObjectInstance, 0>::Hook
{
public:
  typedef IntrusiveUnorderedMap<ObjectInstanceHandle, ObjectInstance> HandleMap;
  typedef IntrusiveUnorderedMap<std::string, ObjectInstance> NameMap;
  typedef IntrusiveList<ObjectInstance, 0> FirstList;

  ObjectInstance(Federation& federation);
  ~ObjectInstance();

  Federation const& getFederation() const
  { return _federation; }
  Federation& getFederation()
  { return _federation; }

  ObjectInstanceHandle const& getObjectInstanceHandle() const
  { return IntrusiveUnorderedMap<ObjectInstanceHandle, ObjectInstance>::Hook::getKey(); }
  void setObjectInstanceHandle(ObjectInstanceHandle const& objectInstanceHandle);

  std::string const& getName() const
  { return IntrusiveUnorderedMap<std::string, ObjectInstance>::Hook::getKey(); }
  void setName(std::string const& name);

  /// The pointer to the object class this object is an instance of, can be zero
  ObjectClass const* getObjectClass() const
  { return _objectClass; }
  ObjectClass* getObjectClass()
  { return _objectClass; }
  void setObjectClass(ObjectClass* objectClass);

  /// UnorderedSet of ObjectInstanceConnect instances indexed by connectHandle
  typedef IntrusiveUnorderedMap<ConnectHandle, ObjectInstanceConnect> ConnectHandleObjectInstanceConnectMap;
  /// Get the set of ObjectInstanceConnect instances
  ConnectHandleObjectInstanceConnectMap const& getConnectHandleObjectInstanceConnectMap() const
  { return _connectHandleObjectInstanceConnectMap; }
  ConnectHandleObjectInstanceConnectMap& getConnectHandleObjectInstanceConnectMap()
  { return _connectHandleObjectInstanceConnectMap; }
  /// List of object instance handle/name references at this connect.
  void insert(ObjectInstanceConnect& objectInstanceConnect)
  { _connectHandleObjectInstanceConnectMap.insert(objectInstanceConnect); }
  /// Mark the name handle pair also represented with this as used in the federationConnect
  void reference(FederationConnect& federationConnect);
  /// Releases the ObjectInstanceConnect entry belonging to the connectHandle
  bool unreference(ConnectHandle const& connectHandle);

  void removeConnect(ConnectHandle const& connectHandle);

  /// Return the connect that owns this object
  ConnectHandle getOwnerConnectHandle()
  {
    InstanceAttribute* instanceAttribute = getInstanceAttribute(AttributeHandle(0));
    if (!instanceAttribute)
      return ConnectHandle();
    return instanceAttribute->getOwnerConnectHandle();
  }
  void setOwnerConnectHandle(ConnectHandle const& connectHandle)
  {
    InstanceAttribute* instanceAttribute = getInstanceAttribute(AttributeHandle(0));
    if (!instanceAttribute)
      return;
    instanceAttribute->setOwnerConnectHandle(connectHandle);
  }

  /// UnorderedSet of InstanceAttribute instances indexed by attributeHandle
  typedef IntrusiveUnorderedMap<AttributeHandle, InstanceAttribute> AttributeHandleInstanceAttributeMap;
  /// Get the set of InstanceAttribute instances
  AttributeHandleInstanceAttributeMap const& getAttributeHandleInstanceAttributeMap() const
  { return _attributeHandleInstanceAttributeMap; }
  AttributeHandleInstanceAttributeMap& getAttributeHandleInstanceAttributeMap()
  { return _attributeHandleInstanceAttributeMap; }
  /// Get one InstanceAttribute instance matching attributeHandle
  InstanceAttribute const* getInstanceAttribute(AttributeHandle const& attributeHandle) const;
  InstanceAttribute* getInstanceAttribute(AttributeHandle const& attributeHandle);
  InstanceAttribute* getPrivilegeToDeleteInstanceAttribute();
  void insert(InstanceAttribute& instanceAttribute);

private:
#if 201103L <= __cplusplus
  ObjectInstance(ObjectInstance const&) = delete;
  ObjectInstance(ObjectInstance&&) = delete;
  ObjectInstance& operator=(ObjectInstance const&) = delete;
  ObjectInstance& operator=(ObjectInstance&&) = delete;
#else
  ObjectInstance(ObjectInstance const&);
  ObjectInstance& operator=(ObjectInstance const&);
#if 200610L <= __cpp_rvalue_reference
  ObjectInstance(ObjectInstance&&);
  ObjectInstance& operator=(ObjectInstance&&);
#endif
#endif

  Federation& _federation;

  /// The pointer to the object class this object is an instance of, can be zero
  ObjectClass* _objectClass;

  /// UnorderedSet of ObjectInstanceConnect instances indexed by connectHandle
  ConnectHandleObjectInstanceConnectMap _connectHandleObjectInstanceConnectMap;

  /// UnorderedSet of InstanceAttribute instances indexed by attributeHandle
  AttributeHandleInstanceAttributeMap _attributeHandleInstanceAttributeMap;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_ObjectInstance_h
