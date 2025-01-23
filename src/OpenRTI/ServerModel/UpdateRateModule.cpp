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

#include "UpdateRateModule.h"

#include "Module.h"
#include "UpdateRate.h"

namespace OpenRTI {
namespace ServerModel {

UpdateRateModule::UpdateRateModule(UpdateRate& updateRate, Module& module) :
  _updateRate(updateRate),
  _module(module)
{
  _module._insertUpdateRateModuleList(*this);
  _updateRate._insertUpdateRateModuleList(*this);
}

UpdateRateModule::~UpdateRateModule()
{
  _updateRate._unlinkUpdateRateModuleList(*this);
  _module._unlinkUpdateRateModuleList(*this);
}

} // namespace ServerModel
} // namespace OpenRTI
