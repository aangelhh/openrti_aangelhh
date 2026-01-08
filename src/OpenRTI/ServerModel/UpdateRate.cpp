/* -*-c++-*- OpenRTI - Copyright (C) 2009-2026 Mathias Froehlich
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

#include "UpdateRate.h"

#include "Federation.h"
#include "UpdateRateModule.h"

namespace OpenRTI {
namespace ServerModel {

UpdateRate::UpdateRate(Federation& federation, UpdateRateHandle const& updateRateHandle, std::string const& name) :
  _federation(federation),
  _updateRateHandle(updateRateHandle),
  _name(name),
  _rate(0)
{
  _federation._insertUpdateRateHandleUpdateRateMap(*this);
  _federation._insertUpdateRateNameUpdateRateMap(*this);
}

UpdateRate::~UpdateRate()
{
  _federation._unlinkUpdateRateNameUpdateRateMap(*this);
  _federation._unlinkUpdateRateHandleUpdateRateMap(*this);

  OpenRTIAssert(_updateRateModuleList.empty());
}

void
UpdateRate::setRate(double rate)
{
  _rate = rate;
}

bool
UpdateRate::getIsReferencedByAnyModule() const
{
  return !_updateRateModuleList.empty();
}

void
UpdateRate::_insertUpdateRateModuleList(UpdateRateModule& updateRateModule)
{
  _updateRateModuleList.push_back(updateRateModule);
}

void
UpdateRate::_unlinkUpdateRateModuleList(UpdateRateModule& updateRateModule)
{
  _updateRateModuleList.unlink(updateRateModule);
}

} // namespace ServerModel
} // namespace OpenRTI
