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

#ifndef OpenRTI_ServerModel_ObjectInstanceConnect_h
#define OpenRTI_ServerModel_ObjectInstanceConnect_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

#include "Handle.h"

namespace OpenRTI {
namespace ServerModel {

class FederationConnect;
class ObjectInstance;

class OPENRTI_LOCAL ObjectInstanceConnect :
    public IntrusiveUnorderedMap<ConnectHandle, ObjectInstanceConnect>::Hook,
    public IntrusiveList<ObjectInstanceConnect, 0>::Hook
{
public:
  ObjectInstanceConnect(ObjectInstance& objectInstance, FederationConnect& federationConnect);
  ~ObjectInstanceConnect();

  ObjectInstance const& getObjectInstance() const
  { return _objectInstance; }
  ObjectInstance& getObjectInstance()
  { return _objectInstance; }

  FederationConnect const& getFederationConnect() const
  { return _federationConnect; }
  FederationConnect& getFederationConnect()
  { return _federationConnect; }

  /// The connect handle to identify this connect
  ConnectHandle const& getConnectHandle() const;

private:
#if 201103L <= __cplusplus
  ObjectInstanceConnect(ObjectInstanceConnect const&) = delete;
  ObjectInstanceConnect(ObjectInstanceConnect&&) = delete;
  ObjectInstanceConnect& operator=(ObjectInstanceConnect const&) = delete;
  ObjectInstanceConnect& operator=(ObjectInstanceConnect&&) = delete;
#else
  ObjectInstanceConnect(ObjectInstanceConnect const&);
  ObjectInstanceConnect& operator=(ObjectInstanceConnect const&);
#if 200610L <= __cpp_rvalue_reference
  ObjectInstanceConnect(ObjectInstanceConnect&&);
  ObjectInstanceConnect& operator=(ObjectInstanceConnect&&);
#endif
#endif

  ObjectInstance& _objectInstance;

  FederationConnect& _federationConnect;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_ObjectInstanceConnect_h
