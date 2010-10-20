/*
-----------------------------------------------------------------------------
Copyright (c) 2010 Tippett Studio
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

3. The name of the author may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
*/

#include <iostream>
#include <map>

#include <time.h>

#include <Shotgun/Shotgun.h>
#include <Shotgun/utils.h>

// External global variables that contain the info of the local time zone
extern char* tzname[2];
extern long timezone;
extern int daylight;

namespace SG {

// *****************************************************************************
Shotgun::Shotgun(const std::string &serverURL,
                 const std::string &authName,
                 const std::string &authKey)
    : m_serverURL(serverURL),
      m_authName(authName),
      m_authKey(authKey),
      m_client(NULL)
{
    // Client
    m_client = new xmlrpc_c::client_xml(&m_transport);

    // Authetication info comes from Shotgun Admin Pages->Scripts
    m_authMap.clear();
    m_authMap.add("script_name", m_authName)
             .add("script_key", m_authKey);

    // Set the correct "TZ" time zone environment variable, this is needed by
    // some of the datetime calls to find the correct local time zone info.
    //setTimeZoneEnv();

    // Register the classes
    registerClass("Asset",        &Asset::entityType,        &Asset::factory,        &Asset::defaultReturnFields);
    registerClass("Delivery",     &Delivery::entityType,     &Delivery::factory,     &Delivery::defaultReturnFields);
    registerClass("Element",      &Element::entityType,      &Element::factory,      &Element::defaultReturnFields);
    registerClass("Group",        &Group::entityType,        &Group::factory,        &Group::defaultReturnFields);
    registerClass("HumanUser",    &HumanUser::entityType,    &HumanUser::factory,    &HumanUser::defaultReturnFields);
    registerClass("Note",         &Note::entityType,         &Note::factory,         &Note::defaultReturnFields);
    registerClass("Playlist",     &Playlist::entityType,     &Playlist::factory,     &Playlist::defaultReturnFields);
    registerClass("Project",      &Project::entityType,      &Project::factory,      &Project::defaultReturnFields);
    registerClass("PublishEvent", &PublishEvent::entityType, &PublishEvent::factory, &PublishEvent::defaultReturnFields);
    registerClass("Review",       &Review::entityType,       &Review::factory,       &Review::defaultReturnFields);
    registerClass("ReviewItem",   &ReviewItem::entityType,   &ReviewItem::factory,   &ReviewItem::defaultReturnFields);
    registerClass("Sequence",     &Sequence::entityType,     &Sequence::factory,     &Sequence::defaultReturnFields);
    registerClass("Shot",         &Shot::entityType,         &Shot::factory,         &Shot::defaultReturnFields);
    registerClass("Task",         &Task::entityType,         &Task::factory,         &Task::defaultReturnFields);
    registerClass("Version",      &Version::entityType,      &Version::factory,      &Version::defaultReturnFields);
}

// *****************************************************************************
Shotgun::~Shotgun()
{
    delete m_client;
}

// *****************************************************************************
void Shotgun::registerClass(const std::string &classType,
                            const EntityTypeFunc &entityTypeFunc,
                            const FactoryFunc &factoryFunc,
                            const DefaultReturnFieldsFunc &defaultReturnFieldsFunc)
{
#if 0
    m_classRegistry[classType] = RegistryFuncPair(factoryFunc, defaultReturnFieldsFunc);
#else
    m_classRegistry[classType].entityTypeFunc = entityTypeFunc;
    m_classRegistry[classType].factoryFunc = factoryFunc;
    m_classRegistry[classType].defaultReturnFieldsFunc = defaultReturnFieldsFunc;
#endif
}

// *****************************************************************************
EntityPtrs Shotgun::entityFactoryFind(const std::string &classType, 
                                      Dict &findMap,
                                      const int limit)
{
    EntityPtrs entities;

    // Find the registered functions for the given type of class.
    ClassRegistry::iterator foundRegistryIter = m_classRegistry.find(classType);

    if (foundRegistryIter == m_classRegistry.end())
    {
        throw SgEntityClassNotRegisteredError(classType);
    }

    // The set of registered functions 
    RegistryFuncs registryFuncs = (*foundRegistryIter).second;

    // ------------------------------------------------------------------------
    // If the given findMap already has a "return_fields", merge its contents 
    // with the poupulated default return Fields of the given entity type. 
    // Shotgun will ignore the duplicated fields when it returns the search result. 
    // To update the findMap's "return_fields", erase it first since the 
    // xmlrpc_c::value type can't be reassigned once it's been instantiated. 
    // ------------------------------------------------------------------------

    // Populate the default return fields and add the extra return fields
    // before passing them to the findMap
    List returnFields = (*(registryFuncs.defaultReturnFieldsFunc))();
    try
    {
        // Check to see if the findMap has "return_fields" already
        returnFields.extend(findMap.value<List>("return_fields"));
        findMap.erase("return_fields");
    }
    catch (SgDictKeyNotFoundError)
    {
        // Do nothing
    }

    findMap.add("return_fields", returnFields);

    // If the findMap already has a "type" field, override it with the
    // entityType that corresponds to the given "classType" to ensure 
    // the type will not conflict with the factory function.
    if (findMap.find("type"))
    {
        findMap.erase("type");
    }
    findMap.add("type", (*(registryFuncs.entityTypeFunc))());

    // Find the shotgun entities by the findMap
    List xmlrpcFindResult = Entity::findSGEntities(this, findMap, limit); 
    
    // Create entity class object.
    for (size_t i = 0; i < xmlrpcFindResult.size(); i++)
    {
        entities.push_back((*(registryFuncs.factoryFunc))(this, xmlrpcFindResult[i]));
    }

    return entities;
}

// *****************************************************************************
Entity *Shotgun::entityFactoryCreate(const std::string &classType, Dict &createMap)
{
    // Find the registered functions for the given type of class.
    ClassRegistry::iterator foundRegistryIter = m_classRegistry.find(classType);

    if (foundRegistryIter == m_classRegistry.end())
    {
        throw SgEntityClassNotRegisteredError(classType);
    }

    // The set of registered functions 
    RegistryFuncs registryFuncs = (*foundRegistryIter).second;

    // ------------------------------------------------------------------------
    // If the given createMap already has a "return_fields", merge its contents 
    // with the poupulated default return fields of the given entity type. 
    // Shotgun will ignore the duplicated fields when it returns the search result. 
    // To update the createMap's "return_fields", erase it first since the 
    // xmlrpc_c::value type can't be reassigned once it's been instantiated. 
    // ------------------------------------------------------------------------

    // Populate the default return fields and add the extra return fields
    // before passing them to the createMap
    List returnFields = (*(registryFuncs.defaultReturnFieldsFunc))();
    try
    {
        // Check to see if the createMap has "return_fields" already
        returnFields.extend(createMap.value<List>("return_fields"));
        createMap.erase("return_fields");
    }
    catch (SgDictKeyNotFoundError)
    {
        // Do nothing
    }

    createMap.add("return_fields", returnFields);

    // If the createMap already has a "type" field, override it with the
    // entityType that corresponds to the given "classType" to ensure the 
    // type will not conflict with the factory function.
    if (createMap.find("type"))
    {
        createMap.erase("type");
    }
    createMap.add("type", (*(registryFuncs.entityTypeFunc))());

    // Create the shotgun entity by the createMap
    xmlrpc_c::value xmlrpcCreateResult = Entity::createSGEntity(this, createMap); 
    
    // Create entity class object.
    if (xmlrpcCreateResult.type() != xmlrpc_c::value::TYPE_NIL)
    {
        return (*(registryFuncs.factoryFunc))(this, xmlrpcCreateResult);
    }
    else
    {
        throw SgEntityCreateError(classType);
    }
}

// *****************************************************************************
Entity *Shotgun::createEntity(const std::string &classType,
                              const Dict &data,
                              const List &extraReturnFields)
{
    Dict createMap = Entity::buildCreateMap(data,
                                            extraReturnFields);

    return this->entityFactoryCreate(classType, createMap);
}
 
// *****************************************************************************
Entity *Shotgun::findEntity(const std::string &classType,
                            const FilterBy &filterList,
                            const List &extraReturnFields,
                            const bool retiredOnly,
                            const SortBy &order)
{
    Dict findMap = Entity::buildFindMap(filterList,
                                        extraReturnFields,
                                        retiredOnly,
                                        1, // limit
                                        order);

    EntityPtrs entities = this->entityFactoryFind(classType, findMap, 1);
    if (entities.size() > 0)
    {
        return entities[0];
    }
    else
    {
        throw SgEntityNotFoundError(classType);
    }
}

// *****************************************************************************
EntityPtrs Shotgun::findEntities(const std::string &classType,
                                 const FilterBy &filterList,
                                 const int limit,
                                 const List &extraReturnFields,
                                 const bool retiredOnly,
                                 const SortBy &order)
{
    Dict findMap = Entity::buildFindMap(filterList,
                                        extraReturnFields,
                                        retiredOnly,
                                        limit,
                                        order);

    return this->entityFactoryFind(classType, findMap, limit);
}

// *****************************************************************************
bool Shotgun::deleteEntity(const std::string &classType, const int id)
{
    // Find the registered functions for the given type of class.
    ClassRegistry::iterator foundRegistryIter = m_classRegistry.find(classType);

    if (foundRegistryIter == m_classRegistry.end())
    {
        throw SgEntityClassNotRegisteredError(classType);
    }

    // The set of registered functions 
    RegistryFuncs registryFuncs = (*foundRegistryIter).second;

    return Entity::deleteSGEntity(this, (*(registryFuncs.entityTypeFunc))(), id);
}

// *****************************************************************************
void Shotgun::setTimeZoneEnv()
{
    time_t rawTime;
    struct tm *localTime;

    // These calls are necessary to get the updated values for the external
    // time zone variables that contain the correct local time zone info.
    time(&rawTime);
    localTime = localtime(&rawTime);

    char envValue[80];
    if (daylight)
    {
        sprintf(envValue, "%s%d%s", tzname[0], timezone/3600, tzname[1]);
    }
    else
    {
        sprintf(envValue, "%s%d", tzname[0], timezone/3600);
    }

    setenv("TZ", envValue, 1);
}
    
} // End namespace SG
