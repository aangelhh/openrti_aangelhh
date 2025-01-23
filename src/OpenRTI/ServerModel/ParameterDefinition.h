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

#ifndef OpenRTI_ServerModel_ParameterDefinition_h
#define OpenRTI_ServerModel_ParameterDefinition_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

#include "Handle.h"

namespace OpenRTI {
namespace ServerModel {

class ClassParameter;
class InteractionClass;

class OPENRTI_LOCAL ParameterDefinition :
    public Intrusive::UnorderedSetLink<ParameterDefinition, Intrusive::ParentTag<InteractionClass> >,
    public Intrusive::UnorderedSetLink<ParameterDefinition, Intrusive::ParentTag<InteractionClass, 1> >
{
public:
  ParameterDefinition(InteractionClass& interactionClass, ParameterHandle const& parameterHandle, std::string const& name);
  ~ParameterDefinition();

  InteractionClass const& getInteractionClass() const
  { return _interactionClass; }
  InteractionClass& getInteractionClass()
  { return _interactionClass; }

  ParameterHandle const& getParameterHandle() const
  { return _parameterHandle; }

  std::string const& getName() const
  { return _name; }

  /// List of ClassParameter instances belonging to this ParameterDefinition
  typedef IntrusiveList<ClassParameter, 0> ClassParameterList;
  /// Get the list of ClassParameter instances
  ClassParameterList const& getClassParameterList() const
  { return _classParameterList; }
  ClassParameterList& getClassParameterList()
  { return _classParameterList; }
  void insert(ClassParameter& classParameter);

  template<typename Link>
  struct IntrusiveKey;

private:
#if 201103L <= __cplusplus
  ParameterDefinition(ParameterDefinition const&) = delete;
  ParameterDefinition(ParameterDefinition&&) = delete;
  ParameterDefinition& operator=(ParameterDefinition const&) = delete;
  ParameterDefinition& operator=(ParameterDefinition&&) = delete;
#else
  ParameterDefinition(ParameterDefinition const&);
  ParameterDefinition& operator=(ParameterDefinition const&);
#if 200610L <= __cpp_rvalue_reference
  ParameterDefinition(ParameterDefinition&&);
  ParameterDefinition& operator=(ParameterDefinition&&);
#endif
#endif

  InteractionClass& _interactionClass;

  ParameterHandle const _parameterHandle;

  std::string const _name;

  /// List of ClassParameter instances belonging to this ParameterDefinition
  ClassParameterList _classParameterList;
};

template<>
struct ParameterDefinition::IntrusiveKey<Intrusive::UnorderedSetLink<ParameterDefinition, Intrusive::ParentTag<InteractionClass> > > {
  static ParameterHandle const& get(ParameterDefinition const& parameterDefinition)
  { return parameterDefinition.getParameterHandle(); }
};

template<>
struct ParameterDefinition::IntrusiveKey<Intrusive::UnorderedSetLink<ParameterDefinition, Intrusive::ParentTag<InteractionClass, 1> > > {
  static std::string const& get(ParameterDefinition const& parameterDefinition)
  { return parameterDefinition.getName(); }
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_ParameterDefinition_h
