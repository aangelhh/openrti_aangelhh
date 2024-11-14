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

#ifndef OpenRTI_ServerModel_InteractionClass_h
#define OpenRTI_ServerModel_InteractionClass_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

#include "Handle.h"
#include "ParameterDefinition.h"
#include "ServerModel.h"
#include "StringUtils.h"
#include "Message.h"

namespace OpenRTI {
namespace ServerModel {

class ClassParameter;
class Federation;
class ParameterDefinition;

class OPENRTI_LOCAL InteractionClass :
    public IntrusiveUnorderedMap<InteractionClassHandle, InteractionClass>::Hook,
    public IntrusiveUnorderedMap<StringVector, InteractionClass>::Hook,
    public IntrusiveList<InteractionClass, 0>::Hook,
    public PublishSubscribe
{
public:
  typedef IntrusiveUnorderedMap<InteractionClassHandle, InteractionClass> HandleMap;
  typedef IntrusiveUnorderedMap<StringVector, InteractionClass> NameMap;
  typedef IntrusiveList<InteractionClass, 0> ChildList;

  InteractionClass(Federation& federation, InteractionClass* parentInteractionClass = 0);
  ~InteractionClass();

  Federation const& getFederation() const
  { return _federation; }
  Federation& getFederation()
  { return _federation; }

  InteractionClassHandle const& getInteractionClassHandle() const
  { return IntrusiveUnorderedMap<InteractionClassHandle, InteractionClass>::Hook::getKey(); }
  void setInteractionClassHandle(InteractionClassHandle const& interactionClassHandle);

  StringVector const& getName() const
  { return IntrusiveUnorderedMap<StringVector, InteractionClass>::Hook::getKey(); }
  void setName(StringVector const& name);

  OrderType getOrderType() const
  { return _orderType; }
  void setOrderType(OrderType orderType);

  TransportationType getTransportationType() const
  { return _transportationType; }
  void setTransportationType(TransportationType transportationType);

  InteractionClass const* getParentInteractionClass() const
  { return _parentInteractionClass; }
  InteractionClass* getParentInteractionClass()
  { return _parentInteractionClass; }
  InteractionClassHandle getParentInteractionClassHandle() const;

  /// List of InteractionClass instances belonging to this InteractionClass
  typedef IntrusiveList<InteractionClass, 0> ChildInteractionClassList;
  /// Get the list of InteractionClass instances
  ChildInteractionClassList const& getChildInteractionClassList() const
  { return _childInteractionClassList; }
  ChildInteractionClassList& getChildInteractionClassList()
  { return _childInteractionClassList; }

  /// The list of Modules referencing this InteractionClass
  typedef IntrusiveList<InteractionClassModule, 1> InteractionClassModuleList;
  /// Get the list of InteractionClassModule instances
  InteractionClassModuleList const& getInteractionClassModuleList() const
  { return _interactionClassModuleList; }
  InteractionClassModuleList& getInteractionClassModuleList()
  { return _interactionClassModuleList; }
  void insert(InteractionClassModule& interactionClassModule)
  { _interactionClassModuleList.push_back(interactionClassModule); }
  bool getIsReferencedByAnyModule() const;

  /// UnorderedSet of ParameterDefinition instances indexed by parameterHandle
  typedef IntrusiveUnorderedMap<ParameterHandle, ParameterDefinition> ParameterHandleParameterDefinitionMap;
  /// Get the set of ParameterDefinition instances
  ParameterHandleParameterDefinitionMap const& getParameterHandleParameterDefinitionMap() const
  { return _parameterHandleParameterDefinitionMap; }
  ParameterHandleParameterDefinitionMap& getParameterHandleParameterDefinitionMap()
  { return _parameterHandleParameterDefinitionMap; }
  /// Get one ParameterDefinition instance matching parameterHandle
  ParameterDefinition const* getParameterDefinition(ParameterHandle const& parameterHandle) const;
  ParameterDefinition* getParameterDefinition(ParameterHandle const& parameterHandle);

  /// UnorderedSet of ParameterDefinition instances indexed by name
  typedef IntrusiveUnorderedMap<std::string, ParameterDefinition> ParameterNameParameterDefinitionMap;
  /// Get one ParameterDefinition instance matching name
  ParameterDefinition const* getParameterDefinition(std::string const& name) const;
  ParameterDefinition* getParameterDefinition(std::string const& name);

  void eraseParameterDefinitions();
  std::size_t getNumParameterDefinitions() const;
  ParameterHandle getFirstUnusedParameterHandle();

  void insert(ParameterDefinition& parameterDefinition);

  /// The list of Modules referencing this InteractionClass set of ParameterDefinitions
  typedef IntrusiveList<ParameterDefinitionModule, 1> ParameterDefinitionModuleList;
  /// Get the list of ParameterDefinitionModule instances
  ParameterDefinitionModuleList const& getParameterDefinitionModuleList() const
  { return _parameterDefinitionModuleList; }
  ParameterDefinitionModuleList& getParameterDefinitionModuleList()
  { return _parameterDefinitionModuleList; }
  void insert(ParameterDefinitionModule& parameterDefinitionModule)
  { _parameterDefinitionModuleList.push_back(parameterDefinitionModule); }
  bool getAreParametersReferencedByAnyModule() const;

  /// UnorderedSet of ClassParameter instances indexed by parameterHandle
  typedef IntrusiveUnorderedMap<ParameterHandle, ClassParameter> ParameterHandleClassParameterMap;
  /// Get the set of ClassParameter instances
  ParameterHandleClassParameterMap const& getParameterHandleClassParameterMap() const
  { return _parameterHandleClassParameterMap; }
  ParameterHandleClassParameterMap& getParameterHandleClassParameterMap()
  { return _parameterHandleClassParameterMap; }
  /// Get one ClassParameter instance matching parameterHandle
  ClassParameter const* getClassParameter(ParameterHandle const& parameterHandle) const;
  ClassParameter* getClassParameter(ParameterHandle const& parameterHandle);
  void insertClassParameterFor(ParameterDefinition& parameterDefinition);

  // FIXME temporarily in this way
  DimensionHandleSet _dimensionHandleSet;

  void updateCumulativeSubscription(ConnectHandle const& connectHandle)
  {
    bool parentSubscribed = false;
    if (_parentInteractionClass &&
        0 != _parentInteractionClass->_cumulativeSubscribedConnectHandleSet.count(connectHandle))
      parentSubscribed = true;

    _updateCumulativeSubscription(connectHandle, parentSubscribed);
  }
  void _updateCumulativeSubscription(ConnectHandle const& connectHandle, bool subscribe /*Replace with regionset or something*/)
  {
    subscribe |= (Unsubscribed != getSubscriptionType(connectHandle));
    if (!updateCumulativeSubscribedConnectHandleSet(connectHandle, subscribe))
      return;
    // Update the receiving connect handle set
    for (ChildInteractionClassList::iterator i = _childInteractionClassList.begin(); i != _childInteractionClassList.end(); ++i) {
      i->_updateCumulativeSubscription(connectHandle, subscribe);
    }
  }

private:
#if 201103L <= __cplusplus
  InteractionClass(InteractionClass const&) = delete;
  InteractionClass(InteractionClass&&) = delete;
  InteractionClass& operator=(InteractionClass const&) = delete;
  InteractionClass& operator=(InteractionClass&&) = delete;
#else
  InteractionClass(InteractionClass const&);
  InteractionClass& operator=(InteractionClass const&);
#if 200610L <= __cpp_rvalue_reference
  InteractionClass(InteractionClass&&);
  InteractionClass& operator=(InteractionClass&&);
#endif
#endif

  Federation& _federation;

  OrderType _orderType;

  TransportationType _transportationType;

  InteractionClass* const _parentInteractionClass;

  /// List of InteractionClass instances belonging to this InteractionClass
  ChildInteractionClassList _childInteractionClassList;

  /// The list of Modules referencing this InteractionClass
  InteractionClassModuleList _interactionClassModuleList;

  /// UnorderedSet of ParameterDefinition instances indexed by parameterHandle
  ParameterHandleParameterDefinitionMap _parameterHandleParameterDefinitionMap;

  /// UnorderedSet of ParameterDefinition instances indexed by name
  ParameterNameParameterDefinitionMap _parameterNameParameterDefinitionMap;

  /// The list of Modules referencing this InteractionClass set of ParameterDefinitions
  ParameterDefinitionModuleList _parameterDefinitionModuleList;

  /// UnorderedSet of ClassParameter instances indexed by parameterHandle
  ParameterHandleClassParameterMap _parameterHandleClassParameterMap;
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_InteractionClass_h
