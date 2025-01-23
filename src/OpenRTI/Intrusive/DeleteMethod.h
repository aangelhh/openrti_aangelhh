/* -*-c++-*- OpenRTI - Copyright (C) 2013-2025 Mathias Froehlich
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

#ifndef OpenRTI_Intrusive_DeleteMethod_h
#define OpenRTI_Intrusive_DeleteMethod_h

#include "OpenRTIConfig.h"
#include "Export.h"

namespace OpenRTI {
namespace Intrusive {

/// Contains a method that does nothing on container destruction
struct DeleteNoop {
  template<typename C>
  static void destroy(C&)
  { }
};
/// Contains a method that unlinks all container elements on container destruction
struct DeleteUnlink {
  template<typename C>
  static void destroy(C& container)
  { container.unlink(); }
};
/// Contains a method that deletes all container elements on container destruction
struct DeleteClear {
  template<typename C>
  static void destroy(C& container)
  { container.clear(); }
};

} // namespace Intrusive
} // namespace OpenRTI

#endif
