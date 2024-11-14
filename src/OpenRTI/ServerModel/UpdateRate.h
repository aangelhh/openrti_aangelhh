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

#ifndef OpenRTI_ServerModel_UpdateRate_h
#define OpenRTI_ServerModel_UpdateRate_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

#include "Handle.h"
#include "ServerModel.h"

namespace OpenRTI {
namespace ServerModel {

class Federation;

class OPENRTI_LOCAL UpdateRate :
    public IntrusiveUnorderedMap<UpdateRateHandle, UpdateRate>::Hook,
    public IntrusiveUnorderedMap<std::string, UpdateRate>::Hook
{
public:
  typedef IntrusiveUnorderedMap<UpdateRateHandle, UpdateRate> HandleMap;
  typedef IntrusiveUnorderedMap<std::string, UpdateRate> NameMap;

  UpdateRate(Federation& federation);
  ~UpdateRate();

  Federation const& getFederation() const
  { return _federation; }
  Federation& getFederation()
  { return _federation; }

  UpdateRateHandle const& getUpdateRateHandle() const
  { return IntrusiveUnorderedMap<UpdateRateHandle, UpdateRate>::Hook::getKey(); }
  void setUpdateRateHandle(UpdateRateHandle const& updateRateHandle);

  std::string const& getName() const
  { return IntrusiveUnorderedMap<std::string, UpdateRate>::Hook::getKey(); }
  void setName(std::string const& name);

  double getRate() const
  { return _rate; }
  void setRate(double rate);

  /// The list of Modules referencing this UpdateRate
  typedef IntrusiveList<UpdateRateModule, 1> UpdateRateModuleList;
  bool getIsReferencedByAnyModule() const;

  void insert(UpdateRateModule& updateRateModule)
  { _updateRateModuleList.push_back(updateRateModule); }

private:
#if 201103L <= __cplusplus
  UpdateRate(UpdateRate const&) = delete;
  UpdateRate(UpdateRate&&) = delete;
  UpdateRate& operator=(UpdateRate const&) = delete;
  UpdateRate& operator=(UpdateRate&&) = delete;
#else
  UpdateRate(UpdateRate const&);
  UpdateRate& operator=(UpdateRate const&);
#if 200610L <= __cpp_rvalue_reference
  UpdateRate(UpdateRate&&);
  UpdateRate& operator=(UpdateRate&&);
#endif
#endif

  Federation& _federation;

  double _rate;

  /// The list of Modules referencing this UpdateRate
  UpdateRateModuleList _updateRateModuleList;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_UpdateRate_h
