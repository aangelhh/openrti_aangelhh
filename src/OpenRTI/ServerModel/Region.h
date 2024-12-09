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

#ifndef OpenRTI_ServerModel_Region_h
#define OpenRTI_ServerModel_Region_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

#include "Handle.h"
#include "Message.h"

namespace OpenRTI {
namespace ServerModel {

class Federate;

class OPENRTI_LOCAL Region :
    public IntrusiveUnorderedMap<LocalRegionHandle, Region>::Hook
{
public:
  Region(Federate& federate);
  ~Region();

  Federate const& getFederate() const
  { return _federate; }
  Federate& getFederate()
  { return _federate; }

  LocalRegionHandle const& getRegionHandle() const
  { return IntrusiveUnorderedMap<LocalRegionHandle, Region>::Hook::getKey(); }
  void setRegionHandle(LocalRegionHandle const& regionHandle);

  RegionValue _regionValue;
  DimensionHandleSet _dimensionHandleSet;

private:
#if 201103L <= __cplusplus
  Region(Region const&) = delete;
  Region(Region&&) = delete;
  Region& operator=(Region const&) = delete;
  Region& operator=(Region&&) = delete;
#else
  Region(Region const&);
  Region& operator=(Region const&);
#if 200610L <= __cpp_rvalue_reference
  Region(Region&&);
  Region& operator=(Region&&);
#endif
#endif

  Federate& _federate;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_Region_h
