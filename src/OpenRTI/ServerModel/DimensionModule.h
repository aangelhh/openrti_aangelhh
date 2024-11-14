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

#ifndef OpenRTI_ServerModel_DimensionModule_h
#define OpenRTI_ServerModel_DimensionModule_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

namespace OpenRTI {
namespace ServerModel {

class Dimension;
class Module;

class OPENRTI_LOCAL DimensionModule :
    public IntrusiveList<DimensionModule, 0>::Hook,
    public IntrusiveList<DimensionModule, 1>::Hook
{
public:
  typedef IntrusiveList<DimensionModule, 0> FirstList;
  typedef IntrusiveList<DimensionModule, 1> SecondList;

  DimensionModule(Dimension& dimension, Module& module);
  ~DimensionModule();

  Dimension const& getDimension() const
  { return _dimension; }
  Dimension& getDimension()
  { return _dimension; }

  Module const& getModule() const
  { return _module; }
  Module& getModule()
  { return _module; }

private:
#if 201103L <= __cplusplus
  DimensionModule(DimensionModule const&) = delete;
  DimensionModule(DimensionModule&&) = delete;
  DimensionModule& operator=(DimensionModule const&) = delete;
  DimensionModule& operator=(DimensionModule&&) = delete;
#else
  DimensionModule(DimensionModule const&);
  DimensionModule& operator=(DimensionModule const&);
#if 200610L <= __cpp_rvalue_reference
  DimensionModule(DimensionModule&&);
  DimensionModule& operator=(DimensionModule&&);
#endif
#endif

  Dimension& _dimension;

  Module& _module;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_DimensionModule_h
