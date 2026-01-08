/* -*-c++-*- OpenRTI - Copyright (C) 2009-2026 Mathias Froehlich
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

#ifndef OpenRTI_ServerModel_ClassParameter_h
#define OpenRTI_ServerModel_ClassParameter_h

#include "OpenRTIConfig.h"

#include "Intrusive.h"

#include "Handle.h"

namespace OpenRTI {
namespace ServerModel {

class InteractionClass;
class ParameterDefinition;

class OPENRTI_LOCAL ClassParameter :
    public Intrusive::UnorderedSetLink<ClassParameter, Intrusive::ParentTag<InteractionClass> >,
    public Intrusive::ListLink<ClassParameter, Intrusive::ParentTag<ParameterDefinition> >
{
public:
  ClassParameter(InteractionClass& interactionClass, ParameterDefinition& parameterDefinition);
  ~ClassParameter();

  InteractionClass const& getInteractionClass() const
  { return _interactionClass; }
  InteractionClass& getInteractionClass()
  { return _interactionClass; }

  ParameterDefinition const& getParameterDefinition() const
  { return _parameterDefinition; }
  ParameterDefinition& getParameterDefinition()
  { return _parameterDefinition; }

  ParameterHandle const& getParameterHandle() const
  { return _parameterHandle; }

  template<typename Link>
  struct IntrusiveKey;

private:
#if 201103L <= __cplusplus
  ClassParameter(ClassParameter const&) = delete;
  ClassParameter(ClassParameter&&) = delete;
  ClassParameter& operator=(ClassParameter const&) = delete;
  ClassParameter& operator=(ClassParameter&&) = delete;
#else
  ClassParameter(ClassParameter const&);
  ClassParameter& operator=(ClassParameter const&);
#if 200610L <= __cpp_rvalue_reference
  ClassParameter(ClassParameter&&);
  ClassParameter& operator=(ClassParameter&&);
#endif
#endif

  InteractionClass& _interactionClass;

  ParameterDefinition& _parameterDefinition;

  ParameterHandle const _parameterHandle;
};

template<>
struct ClassParameter::IntrusiveKey<Intrusive::UnorderedSetLink<ClassParameter, Intrusive::ParentTag<InteractionClass> > > {
  static ParameterHandle const& get(ClassParameter const& classParameter)
  { return classParameter.getParameterHandle(); }
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_ClassParameter_h
