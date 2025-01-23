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

#ifndef OpenRTI_Intrusive_Tag_h
#define OpenRTI_Intrusive_Tag_h

#include "OpenRTIConfig.h"
#include "Export.h"

namespace OpenRTI {
namespace Intrusive {

/// Tags for use with containers.
/// Note that all these tags never get instantiated.

template<typename>
struct Tag;

template<typename, std::size_t = ~std::size_t(0)>
struct ParentTag;
template<typename, std::size_t = ~std::size_t(0)>
struct ReferenceTag;

template<std::size_t>
struct NumericTag;

/// For unordered sets.
template<typename Tag, std::size_t = ~std::size_t(0)>
struct ChainTag;
template<typename Tag, std::size_t = ~std::size_t(0)>
struct LocalTag;

} // namespace Intrusive
} // namespace OpenRTI

#endif
