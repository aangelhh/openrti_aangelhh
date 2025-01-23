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

#ifndef OpenRTI_ServerModel_Module_h
#define OpenRTI_ServerModel_Module_h

#include "IntrusiveList.h"
#include "IntrusiveUnorderedMap.h"

#include "Handle.h"
#include "Message.h"

namespace OpenRTI {
namespace ServerModel {

class AttributeDefinitionModule;
class Dimension;
class DimensionModule;
class Federation;
class InteractionClass;
class InteractionClassModule;
class ObjectClass;
class ObjectClassModule;
class ParameterDefinitionModule;
class UpdateRate;
class UpdateRateModule;

class OPENRTI_LOCAL Module :
    public Intrusive::UnorderedSetLink<Module, Intrusive::ParentTag<Federation> >
{
public:
  Module(Federation& federation, ModuleHandle const& moduleHandle);
  ~Module();

  Federation const& getFederation() const
  { return _federation; }
  Federation& getFederation()
  { return _federation; }

  ModuleHandle const& getModuleHandle() const
  { return _moduleHandle; }

  std::string const& getContent() const
  { return _content; }
  void setContent(std::string const& content);

  bool getArtificialInteractionRoot() const
  { return _artificialInteractionRoot; }
  void setArtificialInteractionRoot(bool artificialInteractionRoot);

  bool getArtificialObjectRoot() const
  { return _artificialObjectRoot; }
  void setArtificialObjectRoot(bool artificialObjectRoot);

  /// All Dimensions that are referenced by this Module
  typedef Intrusive::List<Intrusive::ListLink<DimensionModule, Intrusive::ParentTag<Module> > > DimensionModuleList;
  /// Get the list of DimensionModule instances
  DimensionModuleList const& getDimensionModuleList() const
  { return _dimensionModuleList; }
  DimensionModuleList& getDimensionModuleList()
  { return _dimensionModuleList; }
  void insert(DimensionModule& dimensionModule);

  /// All UpdateRates that are referenced by this Module
  typedef IntrusiveList<UpdateRateModule, 0> UpdateRateModuleList;
  /// Get the list of UpdateRateModule instances
  UpdateRateModuleList const& getUpdateRateModuleList() const
  { return _updateRateModuleList; }
  UpdateRateModuleList& getUpdateRateModuleList()
  { return _updateRateModuleList; }
  void insert(UpdateRateModule& updateRateModule);

  /// All InteractionClasses that are referenced by this Module
  typedef IntrusiveList<InteractionClassModule, 0> InteractionClassModuleList;
  /// Get the list of InteractionClassModule instances
  InteractionClassModuleList const& getInteractionClassModuleList() const
  { return _interactionClassModuleList; }
  InteractionClassModuleList& getInteractionClassModuleList()
  { return _interactionClassModuleList; }
  void insert(InteractionClassModule& interactionClassModule);

  /// All InteractionClasses whos ParameterDefinitions are referenced by this Module
  typedef IntrusiveList<ParameterDefinitionModule, 0> ParameterDefinitionModuleList;
  /// Get the list of ParameterDefinitionModule instances
  ParameterDefinitionModuleList const& getParameterDefinitionModuleList() const
  { return _parameterDefinitionModuleList; }
  ParameterDefinitionModuleList& getParameterDefinitionModuleList()
  { return _parameterDefinitionModuleList; }
  void insert(ParameterDefinitionModule& parameterDefinitionModule);

  /// All ObjectClasses that are referenced by this Module
  typedef IntrusiveList<ObjectClassModule, 0> ObjectClassModuleList;
  /// Get the list of ObjectClassModule instances
  ObjectClassModuleList const& getObjectClassModuleList() const
  { return _objectClassModuleList; }
  ObjectClassModuleList& getObjectClassModuleList()
  { return _objectClassModuleList; }
  void insert(ObjectClassModule& objectClassModule);

  /// All ObjectClasses whos AttributeDefinitions are referenced by this Module
  typedef IntrusiveList<AttributeDefinitionModule, 0> AttributeDefinitionModuleList;
  /// Get the list of AttributeDefinitionModule instances
  AttributeDefinitionModuleList const& getAttributeDefinitionModuleList() const
  { return _attributeDefinitionModuleList; }
  AttributeDefinitionModuleList& getAttributeDefinitionModuleList()
  { return _attributeDefinitionModuleList; }
  void insert(AttributeDefinitionModule& attributeDefinitionModule);

  /// Read back the FOMModule context to send this with a message
  void getModule(FOMModule& module) const;

  /// insert into this Module
  DimensionModule* insert(Dimension& dimension);
  UpdateRateModule* insert(UpdateRate& updateRate);
  InteractionClassModule* insert(InteractionClass& interactionClass);
  ParameterDefinitionModule* insertParameters(InteractionClass& interactionClass);
  ObjectClassModule* insert(ObjectClass& objectClass);
  AttributeDefinitionModule* insertAttributes(ObjectClass& objectClass);

  template<typename Link>
  struct IntrusiveKey;

private:
#if 201103L <= __cplusplus
  Module(Module const&) = delete;
  Module(Module&&) = delete;
  Module& operator=(Module const&) = delete;
  Module& operator=(Module&&) = delete;
#else
  Module(Module const&);
  Module& operator=(Module const&);
#if 200610L <= __cpp_rvalue_reference
  Module(Module&&);
  Module& operator=(Module&&);
#endif
#endif

  Federation& _federation;

  ModuleHandle const _moduleHandle;

  std::string _content;

  bool _artificialInteractionRoot;

  bool _artificialObjectRoot;

  /// All Dimensions that are referenced by this Module
  DimensionModuleList _dimensionModuleList;

  /// All UpdateRates that are referenced by this Module
  UpdateRateModuleList _updateRateModuleList;

  /// All InteractionClasses that are referenced by this Module
  InteractionClassModuleList _interactionClassModuleList;

  /// All InteractionClasses whos ParameterDefinitions are referenced by this Module
  ParameterDefinitionModuleList _parameterDefinitionModuleList;

  /// All ObjectClasses that are referenced by this Module
  ObjectClassModuleList _objectClassModuleList;

  /// All ObjectClasses whos AttributeDefinitions are referenced by this Module
  AttributeDefinitionModuleList _attributeDefinitionModuleList;
};

template<>
struct Module::IntrusiveKey<Intrusive::UnorderedSetLink<Module, Intrusive::ParentTag<Federation> > > {
  static ModuleHandle const& get(Module const& module)
  { return module.getModuleHandle(); }
};

} // namespace ServerModel
} // namespace OpenRTI

#endif // OpenRTI_ServerModel_Module_h
