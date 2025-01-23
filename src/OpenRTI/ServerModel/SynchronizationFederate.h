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

#ifndef OpenRTI_ServerModel_SynchronizationFederate_h
#define OpenRTI_ServerModel_SynchronizationFederate_h

#include "OpenRTIConfig.h"

#include "Intrusive.h"

#include "Handle.h"

namespace OpenRTI {
namespace ServerModel {

class Federate;
class Synchronization;

class OPENRTI_LOCAL SynchronizationFederate :
    public Intrusive::UnorderedSetLink<SynchronizationFederate, Intrusive::ParentTag<Synchronization> >,
    public Intrusive::ListLink<SynchronizationFederate, Intrusive::ParentTag<Federate> >
{
public:
  SynchronizationFederate(Synchronization& synchronization, Federate& federate);
  ~SynchronizationFederate();

  Synchronization const& getSynchronization() const
  { return _synchronization; }
  Synchronization& getSynchronization()
  { return _synchronization; }

  Federate const& getFederate() const
  { return _federate; }
  Federate& getFederate()
  { return _federate; }

  FederateHandle const& getFederateHandle() const
  { return _federateHandle; }

  bool getSuccessful() const
  { return _successful; }
  void setSuccessful(bool successful);

  template<typename Link>
  struct IntrusiveKey;

private:
#if 201103L <= __cplusplus
  SynchronizationFederate(SynchronizationFederate const&) = delete;
  SynchronizationFederate(SynchronizationFederate&&) = delete;
  SynchronizationFederate& operator=(SynchronizationFederate const&) = delete;
  SynchronizationFederate& operator=(SynchronizationFederate&&) = delete;
#else
  SynchronizationFederate(SynchronizationFederate const&);
  SynchronizationFederate& operator=(SynchronizationFederate const&);
#if 200610L <= __cpp_rvalue_reference
  SynchronizationFederate(SynchronizationFederate&&);
  SynchronizationFederate& operator=(SynchronizationFederate&&);
#endif
#endif

  Synchronization& _synchronization;

  Federate& _federate;

  FederateHandle const _federateHandle;

  bool _successful;
};

template<>
struct SynchronizationFederate::IntrusiveKey<Intrusive::UnorderedSetLink<SynchronizationFederate, Intrusive::ParentTag<Synchronization> > > {
  static FederateHandle const& get(SynchronizationFederate const& synchronizationFederate)
  { return synchronizationFederate.getFederateHandle(); }
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_SynchronizationFederate_h
