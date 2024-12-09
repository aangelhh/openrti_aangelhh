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

#include "UpdateRate.h"

namespace OpenRTI {
namespace ServerModel {

UpdateRate::UpdateRate(Federation& federation, UpdateRateHandle const& updateRateHandle, std::string const& name) :
  IntrusiveUnorderedMap<UpdateRateHandle const, UpdateRate>::Hook(updateRateHandle),
  IntrusiveUnorderedMap<std::string const, UpdateRate>::Hook(name),
  _federation(federation),
  _rate(0)
{
}

UpdateRate::~UpdateRate()
{
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

} // namespace ServerModel
} // namespace OpenRTI
