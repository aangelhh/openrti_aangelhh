/* -*-c++-*- OpenRTI - Copyright (C) 2013-2026 Mathias Froehlich
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

#ifndef OpenRTI_Intrusive_KeyAccess_h
#define OpenRTI_Intrusive_KeyAccess_h

#include "OpenRTIConfig.h"
#include "Export.h"

namespace OpenRTI {
namespace Intrusive {

// Key get struct that queries the key get struct from the actual value type.
// Special care is taken so that the containers value type is not needed on
// the declaration of the container. Instead the value type must be present
// when the key is acually queried.
// Even if not ealuated in here, keep the Link argument as this prevents a
// full specialzation for the special casing inside the containers.
template<typename Link>
struct DefaultKeyAccess;

} // namespace Intrusive
} // namespace OpenRTI

#endif
