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

#ifndef OpenRTI_ServerModel_UpdateRateModule_h
#define OpenRTI_ServerModel_UpdateRateModule_h

#include "Intrusive.h"

namespace OpenRTI {
namespace ServerModel {

class Module;
class UpdateRate;

class OPENRTI_LOCAL UpdateRateModule :
    public Intrusive::ListLink<UpdateRateModule, Intrusive::ParentTag<Module> >,
    public Intrusive::ListLink<UpdateRateModule, Intrusive::ParentTag<UpdateRate> >
{
public:
  UpdateRateModule(UpdateRate& updateRate, Module& module);
  ~UpdateRateModule();

  UpdateRate const& getUpdateRate() const
  { return _updateRate; }
  UpdateRate& getUpdateRate()
  { return _updateRate; }

  Module const& getModule() const
  { return _module; }
  Module& getModule()
  { return _module; }

private:
#if 201103L <= __cplusplus
  UpdateRateModule(UpdateRateModule const&) = delete;
  UpdateRateModule(UpdateRateModule&&) = delete;
  UpdateRateModule& operator=(UpdateRateModule const&) = delete;
  UpdateRateModule& operator=(UpdateRateModule&&) = delete;
#else
  UpdateRateModule(UpdateRateModule const&);
  UpdateRateModule& operator=(UpdateRateModule const&);
#if 200610L <= __cpp_rvalue_reference
  UpdateRateModule(UpdateRateModule&&);
  UpdateRateModule& operator=(UpdateRateModule&&);
#endif
#endif

  UpdateRate& _updateRate;

  Module& _module;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_UpdateRateModule_h
