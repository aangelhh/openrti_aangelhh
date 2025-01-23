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

#include "OpenRTIConfig.h"

#include "Intrusive.h"

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

  /// All UpdateRates that are referenced by this Module
  typedef Intrusive::List<Intrusive::ListLink<UpdateRateModule, Intrusive::ParentTag<Module> > > UpdateRateModuleList;
  /// Get the list of UpdateRateModule instances
  UpdateRateModuleList const& getUpdateRateModuleList() const
  { return _updateRateModuleList; }
  UpdateRateModuleList& getUpdateRateModuleList()
  { return _updateRateModuleList; }

  /// All InteractionClasses that are referenced by this Module
  typedef Intrusive::List<Intrusive::ListLink<InteractionClassModule, Intrusive::ParentTag<Module> > > InteractionClassModuleList;
  /// Get the list of InteractionClassModule instances
  InteractionClassModuleList const& getInteractionClassModuleList() const
  { return _interactionClassModuleList; }
  InteractionClassModuleList& getInteractionClassModuleList()
  { return _interactionClassModuleList; }

  /// All InteractionClasses whos ParameterDefinitions are referenced by this Module
  typedef Intrusive::List<Intrusive::ListLink<ParameterDefinitionModule, Intrusive::ParentTag<Module> > > ParameterDefinitionModuleList;
  /// Get the list of ParameterDefinitionModule instances
  ParameterDefinitionModuleList const& getParameterDefinitionModuleList() const
  { return _parameterDefinitionModuleList; }
  ParameterDefinitionModuleList& getParameterDefinitionModuleList()
  { return _parameterDefinitionModuleList; }

  /// All ObjectClasses that are referenced by this Module
  typedef Intrusive::List<Intrusive::ListLink<ObjectClassModule, Intrusive::ParentTag<Module> > > ObjectClassModuleList;
  /// Get the list of ObjectClassModule instances
  ObjectClassModuleList const& getObjectClassModuleList() const
  { return _objectClassModuleList; }
  ObjectClassModuleList& getObjectClassModuleList()
  { return _objectClassModuleList; }

  /// All ObjectClasses whos AttributeDefinitions are referenced by this Module
  typedef Intrusive::List<Intrusive::ListLink<AttributeDefinitionModule, Intrusive::ParentTag<Module> > > AttributeDefinitionModuleList;
  /// Get the list of AttributeDefinitionModule instances
  AttributeDefinitionModuleList const& getAttributeDefinitionModuleList() const
  { return _attributeDefinitionModuleList; }
  AttributeDefinitionModuleList& getAttributeDefinitionModuleList()
  { return _attributeDefinitionModuleList; }

  /// Read back the FOMModule context to send this with a message
  void getModule(FOMModule& module) const;

  /// insert into this Module
  ObjectClassModule* insert(ObjectClass& objectClass);
  AttributeDefinitionModule* insertAttributes(ObjectClass& objectClass);

  /// Create a reference of the Module into the Dimension.
  ///
  /// Create a new DimensionModule instance
  DimensionModule* createDimensionModule(Dimension& dimension);

  /// Create a reference of the Module into the UpdateRate.
  ///
  /// Create a new UpdateRateModule instance
  UpdateRateModule* createUpdateRateModule(UpdateRate& updateRate);

  /// Create a reference of the Module into the InteractionClass.
  ///
  /// Create a new InteractionClassModule instance
  InteractionClassModule* createInteractionClassModule(InteractionClass& interactionClass);

  /// Create a reference of the Module into the InteractionClasss ParameterDefinitions.
  ///
  /// Create a new ParameterDefinitionModule instance
  ParameterDefinitionModule* createParameterDefinitionModule(InteractionClass& interactionClass);

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
  friend class DimensionModule;
  /// Insert dimensionModule into dimensionModuleList
  void _insertDimensionModuleList(DimensionModule& dimensionModule);
  /// Unlink dimensionModule from dimensionModuleList
  void _unlinkDimensionModuleList(DimensionModule& dimensionModule);
  DimensionModuleList _dimensionModuleList;

  /// All UpdateRates that are referenced by this Module
  friend class UpdateRateModule;
  /// Insert updateRateModule into updateRateModuleList
  void _insertUpdateRateModuleList(UpdateRateModule& updateRateModule);
  /// Unlink updateRateModule from updateRateModuleList
  void _unlinkUpdateRateModuleList(UpdateRateModule& updateRateModule);
  UpdateRateModuleList _updateRateModuleList;

  /// All InteractionClasses that are referenced by this Module
  friend class InteractionClassModule;
  /// Insert interactionClassModule into interactionClassModuleList
  void _insertInteractionClassModuleList(InteractionClassModule& interactionClassModule);
  /// Unlink interactionClassModule from interactionClassModuleList
  void _unlinkInteractionClassModuleList(InteractionClassModule& interactionClassModule);
  InteractionClassModuleList _interactionClassModuleList;

  /// All InteractionClasses whos ParameterDefinitions are referenced by this Module
  friend class ParameterDefinitionModule;
  /// Insert parameterDefinitionModule into parameterDefinitionModuleList
  void _insertParameterDefinitionModuleList(ParameterDefinitionModule& parameterDefinitionModule);
  /// Unlink parameterDefinitionModule from parameterDefinitionModuleList
  void _unlinkParameterDefinitionModuleList(ParameterDefinitionModule& parameterDefinitionModule);
  ParameterDefinitionModuleList _parameterDefinitionModuleList;

  /// All ObjectClasses that are referenced by this Module
  friend class ObjectClassModule;
  /// Insert objectClassModule into objectClassModuleList
  void _insertObjectClassModuleList(ObjectClassModule& objectClassModule);
  /// Unlink objectClassModule from objectClassModuleList
  void _unlinkObjectClassModuleList(ObjectClassModule& objectClassModule);
  ObjectClassModuleList _objectClassModuleList;

  /// All ObjectClasses whos AttributeDefinitions are referenced by this Module
  friend class AttributeDefinitionModule;
  /// Insert attributeDefinitionModule into attributeDefinitionModuleList
  void _insertAttributeDefinitionModuleList(AttributeDefinitionModule& attributeDefinitionModule);
  /// Unlink attributeDefinitionModule from attributeDefinitionModuleList
  void _unlinkAttributeDefinitionModuleList(AttributeDefinitionModule& attributeDefinitionModule);
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
