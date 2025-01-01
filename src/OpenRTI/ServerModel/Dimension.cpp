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

#include "Dimension.h"

namespace OpenRTI {
namespace ServerModel {

Dimension::Dimension(Federation& federation, DimensionHandle const& dimensionHandle, std::string const& name) :
  IntrusiveUnorderedMap<DimensionHandle const, Dimension>::Hook(dimensionHandle),
  IntrusiveUnorderedMap<std::string const, Dimension>::Hook(name),
  _federation(federation),
  _upperBound(~Unsigned(0))
{
}

Dimension::~Dimension()
{
  OpenRTIAssert(_dimensionModuleList.empty());
}

void
Dimension::setUpperBound(Unsigned upperBound)
{
  _upperBound = upperBound;
}

bool
Dimension::getIsReferencedByAnyModule() const
{
  return !_dimensionModuleList.empty();
}

} // namespace ServerModel
} // namespace OpenRTI
