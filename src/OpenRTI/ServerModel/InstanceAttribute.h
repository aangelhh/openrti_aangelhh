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

#ifndef OpenRTI_ServerModel_InstanceAttribute_h
#define OpenRTI_ServerModel_InstanceAttribute_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

#include "Handle.h"

namespace OpenRTI {
namespace ServerModel {

class ClassAttribute;
class ObjectInstance;

class OPENRTI_LOCAL InstanceAttribute :
    public IntrusiveUnorderedMap<AttributeHandle, InstanceAttribute>::Hook
{
public:
  InstanceAttribute(ObjectInstance& objectInstance, ClassAttribute& classAttribute);
  ~InstanceAttribute();

  ObjectInstance const& getObjectInstance() const
  { return _objectInstance; }
  ObjectInstance& getObjectInstance()
  { return _objectInstance; }

  ClassAttribute const& getClassAttribute() const
  { return _classAttribute; }
  ClassAttribute& getClassAttribute()
  { return _classAttribute; }

  AttributeHandle const& getAttributeHandle() const
  { return IntrusiveUnorderedMap<AttributeHandle, InstanceAttribute>::Hook::getKey(); }

  /// Get the ConnectHandle this attribute is owned
  ConnectHandle const& getOwnerConnectHandle() const
  { return _ownerConnectHandle; }
  void setOwnerConnectHandle(ConnectHandle const& connectHandle)
  {
    _receivingConnects.erase(connectHandle);
    _ownerConnectHandle = connectHandle;
  }

  void removeConnect(ConnectHandle const& connectHandle)
  {
    _receivingConnects.erase(connectHandle);
    if (_ownerConnectHandle == connectHandle)
      _ownerConnectHandle = ConnectHandle();
  }

  /// Because of attribute ownership, it is clear for an object attribute where the update
  /// stems from, so just have a set of connect handles that want to receive the updates
  ConnectHandleSet _receivingConnects;

  /// The connect this attribute is owned by
  ConnectHandle _ownerConnectHandle;

private:
#if 201103L <= __cplusplus
  InstanceAttribute(InstanceAttribute const&) = delete;
  InstanceAttribute(InstanceAttribute&&) = delete;
  InstanceAttribute& operator=(InstanceAttribute const&) = delete;
  InstanceAttribute& operator=(InstanceAttribute&&) = delete;
#else
  InstanceAttribute(InstanceAttribute const&);
  InstanceAttribute& operator=(InstanceAttribute const&);
#if 200610L <= __cpp_rvalue_reference
  InstanceAttribute(InstanceAttribute&&);
  InstanceAttribute& operator=(InstanceAttribute&&);
#endif
#endif

  ObjectInstance& _objectInstance;

  ClassAttribute& _classAttribute;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_InstanceAttribute_h
