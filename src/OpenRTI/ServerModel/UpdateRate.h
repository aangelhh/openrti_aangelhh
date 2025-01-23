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

#ifndef OpenRTI_ServerModel_UpdateRate_h
#define OpenRTI_ServerModel_UpdateRate_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

#include "Handle.h"

namespace OpenRTI {
namespace ServerModel {

class Federation;
class UpdateRateModule;

class OPENRTI_LOCAL UpdateRate :
    public Intrusive::UnorderedSetLink<UpdateRate, Intrusive::ParentTag<Federation> >,
    public Intrusive::UnorderedSetLink<UpdateRate, Intrusive::ParentTag<Federation, 1> >
{
public:
  UpdateRate(Federation& federation, UpdateRateHandle const& updateRateHandle, std::string const& name);
  ~UpdateRate();

  Federation const& getFederation() const
  { return _federation; }
  Federation& getFederation()
  { return _federation; }

  UpdateRateHandle const& getUpdateRateHandle() const
  { return _updateRateHandle; }

  std::string const& getName() const
  { return _name; }

  double getRate() const
  { return _rate; }
  void setRate(double rate);

  /// The list of Modules referencing this UpdateRate
  typedef Intrusive::List<Intrusive::ListLink<UpdateRateModule, Intrusive::ParentTag<UpdateRate> > > UpdateRateModuleList;
  bool getIsReferencedByAnyModule() const;

  void insert(UpdateRateModule& updateRateModule);

  template<typename Link>
  struct IntrusiveKey;

private:
#if 201103L <= __cplusplus
  UpdateRate(UpdateRate const&) = delete;
  UpdateRate(UpdateRate&&) = delete;
  UpdateRate& operator=(UpdateRate const&) = delete;
  UpdateRate& operator=(UpdateRate&&) = delete;
#else
  UpdateRate(UpdateRate const&);
  UpdateRate& operator=(UpdateRate const&);
#if 200610L <= __cpp_rvalue_reference
  UpdateRate(UpdateRate&&);
  UpdateRate& operator=(UpdateRate&&);
#endif
#endif

  Federation& _federation;

  UpdateRateHandle const _updateRateHandle;

  std::string const _name;

  double _rate;

  /// The list of Modules referencing this UpdateRate
  UpdateRateModuleList _updateRateModuleList;
};

template<>
struct UpdateRate::IntrusiveKey<Intrusive::UnorderedSetLink<UpdateRate, Intrusive::ParentTag<Federation> > > {
  static UpdateRateHandle const& get(UpdateRate const& updateRate)
  { return updateRate.getUpdateRateHandle(); }
};

template<>
struct UpdateRate::IntrusiveKey<Intrusive::UnorderedSetLink<UpdateRate, Intrusive::ParentTag<Federation, 1> > > {
  static std::string const& get(UpdateRate const& updateRate)
  { return updateRate.getName(); }
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_UpdateRate_h
