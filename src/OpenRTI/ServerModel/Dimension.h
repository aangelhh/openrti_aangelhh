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

#ifndef OpenRTI_ServerModel_Dimension_h
#define OpenRTI_ServerModel_Dimension_h

#include "OpenRTIConfig.h"

#include "Intrusive.h"

#include "Handle.h"
#include "Message.h"

namespace OpenRTI {
namespace ServerModel {

class DimensionModule;
class Federation;

class OPENRTI_LOCAL Dimension :
    public Intrusive::UnorderedSetLink<Dimension, Intrusive::ParentTag<Federation> >,
    public Intrusive::UnorderedSetLink<Dimension, Intrusive::ParentTag<Federation, 1> >
{
public:
  Dimension(Federation& federation, DimensionHandle const& dimensionHandle, std::string const& name);
  ~Dimension();

  Federation const& getFederation() const
  { return _federation; }
  Federation& getFederation()
  { return _federation; }

  DimensionHandle const& getDimensionHandle() const
  { return _dimensionHandle; }

  std::string const& getName() const
  { return _name; }

  /// The upper bound
  Unsigned getUpperBound() const
  { return _upperBound; }
  void setUpperBound(Unsigned upperBound);

  /// The list of Modules referencing this Dimension
  typedef Intrusive::List<Intrusive::ListLink<DimensionModule, Intrusive::ParentTag<Dimension> > > DimensionModuleList;
  bool getIsReferencedByAnyModule() const;
  void insert(DimensionModule& dimensionModule);

  template<typename Link>
  struct IntrusiveKey;

private:
#if 201103L <= __cplusplus
  Dimension(Dimension const&) = delete;
  Dimension(Dimension&&) = delete;
  Dimension& operator=(Dimension const&) = delete;
  Dimension& operator=(Dimension&&) = delete;
#else
  Dimension(Dimension const&);
  Dimension& operator=(Dimension const&);
#if 200610L <= __cpp_rvalue_reference
  Dimension(Dimension&&);
  Dimension& operator=(Dimension&&);
#endif
#endif

  Federation& _federation;

  DimensionHandle const _dimensionHandle;

  std::string const _name;

  /// The upper bound
  Unsigned _upperBound;

  /// The list of Modules referencing this Dimension
  DimensionModuleList _dimensionModuleList;
};

template<>
struct Dimension::IntrusiveKey<Intrusive::UnorderedSetLink<Dimension, Intrusive::ParentTag<Federation> > > {
  static DimensionHandle const& get(Dimension const& dimension)
  { return dimension.getDimensionHandle(); }
};

template<>
struct Dimension::IntrusiveKey<Intrusive::UnorderedSetLink<Dimension, Intrusive::ParentTag<Federation, 1> > > {
  static std::string const& get(Dimension const& dimension)
  { return dimension.getName(); }
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_Dimension_h
