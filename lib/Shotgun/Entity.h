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

#ifndef __ENTITY_H__
#define __ENTITY_H__

#include <iostream>
#include <string>

#include <Shotgun/types.h>
#include <Shotgun/exceptions.h>
#include <Shotgun/Method.h>
#include <Shotgun/FilterBy.h>
#include <Shotgun/SortBy.h>
#include <Shotgun/List.h>
#include <Shotgun/Dict.h>
#include <Shotgun/Fields.h>

namespace Shotgun {

class Shotgun;
class TaskMixin;
class NoteMixin;

// *****************************************************************************
class Entity
{
public:
    virtual ~Entity();

    // -------------------------------------------------------------------------
    const Shotgun *sg() const { return m_sg; }
    Shotgun *sg() { return m_sg; }

    // -------------------------------------------------------------------------
    // These are the few common attributes shared by all entities
    virtual const int sgId() const { return getAttrValueAsInt("id"); }
    virtual const time_t sgDateCreated() const { return getAttrValueAsUTCtime("created_at"); }
    virtual const time_t sgDateUpdated() const { return getAttrValueAsUTCtime("updated_at"); }

    // -------------------------------------------------------------------------
    // These two have to be overridden for Project entity
    virtual const std::string sgProjectName() const { return getProjectName(); }
    virtual const std::string sgProjectCode() const { return getProjectCode(); }

    // -------------------------------------------------------------------------
    const std::string entityType() const { return m_type; }
    const xmlrpc_c::value &attrs() const { return *m_attrs; }
    const std::string str() const { return toStdString(*m_attrs); }

    // -------------------------------------------------------------------------
    // This returns a mini link map of the entity. It looks like this:
    //     { 
    //         "type" : "Project",
    //         "id" : 68,
    //     }
    // -------------------------------------------------------------------------
    const Dict asLink() const;

    // -------------------------------------------------------------------------
    // If an entity has a link attribute, this returns the entity type of the link.
    const std::string linkEntityType(const std::string &linkName) const;
    
    // -------------------------------------------------------------------------
    // Get a attribute's value as a generic xmlrpc_c::value type
    virtual const xmlrpc_c::value getAttrValue(const std::string &attrName) const;
    static const xmlrpc_c::value getAttrValue(const std::string &attrName, 
                                              const Dict &attrsMap);

    // -------------------------------------------------------------------------
    // Get a attribute's value of a specific data type.
    virtual const int getAttrValueAsInt(const std::string &attrName) const;
    virtual const int getAttrValueAsInt(const std::string &attrName, 
                                        const int defaultVal) const; 
    static const int getAttrValueAsInt(const std::string &attrName, 
                                       const Dict &attrsMap);
    static const int getAttrValueAsInt(const std::string &attrName, 
                                       const Dict &attrsMap,
                                       const int defaultVal); 

    // -------------------------------------------------------------------------
    virtual const bool getAttrValueAsBool(const std::string &attrName) const; 
    virtual const bool getAttrValueAsBool(const std::string &attrName, 
                                          const bool defaultVal) const; 
    static const bool getAttrValueAsBool(const std::string &attrName, 
                                         const Dict &attrsMap);
    static const bool getAttrValueAsBool(const std::string &attrName, 
                                         const Dict &attrsMap,
                                         const bool defaultVal); 

    // -------------------------------------------------------------------------
    virtual const double getAttrValueAsDouble(const std::string &attrName) const;
    virtual const double getAttrValueAsDouble(const std::string &attrName, 
                                              const double defaultVal) const; 
    static const double getAttrValueAsDouble(const std::string &attrName, 
                                             const Dict &attrsMap);
    static const double getAttrValueAsDouble(const std::string &attrName, 
                                             const Dict &attrsMap,
                                             const double defaultVal);

    // -------------------------------------------------------------------------
    virtual const time_t getAttrValueAsUTCtime(const std::string &attrName) const;
    virtual const time_t getAttrValueAsUTCtime(const std::string &attrName, 
                                               const time_t defaultVal) const;
    static const time_t getAttrValueAsUTCtime(const std::string &attrName, 
                                              const Dict &attrsMap);
    static const time_t getAttrValueAsUTCtime(const std::string &attrName, 
                                              const Dict &attrsMap,
                                              const time_t defaultVal);

    // -------------------------------------------------------------------------
    virtual const struct tm getAttrValueAsLocaltime(const std::string &attrName) const;
    virtual const struct tm getAttrValueAsLocaltime(const std::string &attrName, 
                                                    const struct tm &defaultVal) const;
    static const struct tm getAttrValueAsLocaltime(const std::string &attrName, 
                                                   const Dict &attrsMap);
    static const struct tm getAttrValueAsLocaltime(const std::string &attrName, 
                                                   const Dict &attrsMap,
                                                   const struct tm &defaultVal);

    // -------------------------------------------------------------------------
    virtual const std::string getAttrValueAsString(const std::string &attrName) const;
    virtual const std::string getAttrValueAsString(const std::string &attrName, 
                                                   const std::string &defaultVal) const;
    static const std::string getAttrValueAsString(const std::string &attrName, 
                                                  const Dict &attrsMap);
    static const std::string getAttrValueAsString(const std::string &attrName, 
                                                  const Dict &attrsMap,
                                                  const std::string &defaultVal);

    // -------------------------------------------------------------------------
    virtual const List getAttrValueAsList(const std::string &attrName) const; 
    virtual const List getAttrValueAsList(const std::string &attrName, 
                                          const List &defaultVal) const;
    static const List getAttrValueAsList(const std::string &attrName, 
                                         const Dict &attrsMap);
    static const List getAttrValueAsList(const std::string &attrName, 
                                         const Dict &attrsMap,
                                         const List &defaultVal);

    // -------------------------------------------------------------------------
    // This member function can't take a default value because it has the same 
    // type as the attrsMap, and therefore creates an ambiguous overload.
    //const Dict getAttrValueAsDict(const std::string &attrName, 
    //                              const Dict &defaultVal) const;
    virtual const Dict getAttrValueAsDict(const std::string &attrName) const;
    static const Dict getAttrValueAsDict(const std::string &attrName, 
                                         const Dict &attrsMap);
    static const Dict getAttrValueAsDict(const std::string &attrName, 
                                         const Dict &attrsMap,
                                         const Dict &defaultVal);

    // -------------------------------------------------------------------------
    virtual const Strings getAttrValueAsTags(const std::string &attrName) const;
    static const Strings getAttrValueAsTags(const std::string &attrName,
                                            const Dict &attrsMap);

    // -------------------------------------------------------------------------
    virtual const Entity *getAttrValueAsEntity(const std::string &attrName) const;
    static const Entity *getAttrValueAsEntity(Shotgun *sg,
                                              const std::string &attrName,
                                              const Dict &attrsMap);
    template <class T>
    const T *getAttrValueAsEntity(const std::string &attrName) const;

    template <class T>
    static const T *getAttrValueAsEntity(Shotgun *sg,
                                         const std::string &attrName,
                                         const Dict &attrsMap);

    // -------------------------------------------------------------------------
    virtual const std::vector<Entity *> getAttrValueAsEntities(const std::string &attrName) const;
    static const std::vector<Entity *> getAttrValueAsEntities(Shotgun *sg,
                                                              const std::string &attrName,
                                                              const Dict &attrsMap);
    template <class T>
    const std::vector<T *> getAttrValueAsEntities(const std::string &attrName) const;

    template <class T>
    static const std::vector<T *> getAttrValueAsEntities(Shotgun *sg,
                                                         const std::string &attrName,
                                                         const Dict &attrsMap);

    // -------------------------------------------------------------------------
    virtual const std::string getAttrValueAsUserLogin(const std::string &attrName) const;
    static const std::string getAttrValueAsUserLogin(Shotgun *sg,
                                                     const std::string &attrName,
                                                     const Dict &attrsMap);

    // -------------------------------------------------------------------------
    // The structure to set a field's value is like this:
    //     "fields" : [
    //            {
    //                "field_name" : <string>,            // Required
    //                "value" : <field_value>,            // Required
    //                "multi_entity_update_mode" : "set" | "add" | "remove",   // Optional
    //                "parent_entity": {"type" : <string>, "id" : <int>}       // Optional
    //            },
    //            ...
    //      ]
    // -------------------------------------------------------------------------
    virtual void setAttrValue(const Fields &fields); 

    // -------------------------------------------------------------------------
    Entity &operator=(const Entity &that)
    {
        if (this != &that)
        {
            m_sg = that.m_sg;
            m_type = that.m_type;

            delete m_attrs;
            m_attrs = new xmlrpc_c::value(*(that.m_attrs));
        }

        return *this;
    }

protected:
    Entity(Shotgun *sg);

    // Create a new Shotgun entity
    static xmlrpc_c::value createSGEntity(Shotgun *sg, const Dict &createMap);

    // Find a list of Shotgun entities that match the search map
    static List findSGEntities(Shotgun *sg, 
                               Dict &findMap,
                               const int limit = 0);

    // Update an attribute of an existing Shotgun entity
    static xmlrpc_c::value updateSGEntity(Shotgun *sg,
                                          const std::string &entityType,
                                          const int entityId,
                                          const Fields &fieldsToUpdate);

    // Delete an existing Shotgun entity
    static bool deleteSGEntity(Shotgun *sg, 
                               const std::string &entityType,
                               const int entityId);

    // Build a data map for creating a new Shotgun entity
    static Dict buildCreateMap(const std::string &entityType,
                               const Dict &data,
                               const List &extraReturnFields = List());

    // Build a filter map for finding one or more Shotgun entities
    static Dict buildFindMap(const std::string &entityType,
                             const FilterBy &filterList = FilterBy(),
                             const List &extraReturnFields = List(),
                             const bool retiredOnly = false,
                             const int limit = 0,
                             const SortBy &order = SortBy());

    // Get the value for a given attribute. If the attribute is not included in
    // the entity's return fields, fetch its value directly from Shotgun.
    // In the case where there is an error when retrieving the attribute value, 
    // if the overloaded function is passed a "defaultVal", it will use it as 
    // the default attribute value. Otherwise, an exception will be thrown.
    template <typename T>
    const T getAttrValue(const std::string &attrName) const;

    template <typename T>
    const T getAttrValue(const std::string &attrName,
                         const T defaultVal) const;

    // Get the value for a given attribute from the given attribute map.
    template <typename T>
    static const T getAttrValue(const std::string &attrName,
                                const Dict &attrsMap);

    template <typename T>
    static const T getAttrValue(const std::string &attrName,
                                const Dict &attrsMap,
                                const T defaultVal);

    // The result of the method call is a struct. This function is to extract 
    // a list of entities from the find result struct.
    static List getFindResultEntityList(xmlrpc_c::value &rawResult);

    // Get the paging info from the raw result of the method calls
    static Dict getResultPagingInfo(xmlrpc_c::value &rawResult);

    // Validate a link map and see if it contains the required fields, such as
    // "type", "id" and "name" - (Not sure if "name" is mandatory or optional?)
    static void validateLink(const Dict &link);
    static void validateLink(const xmlrpc_c::value &link);

    const std::string getProjectName() const;
    const std::string getProjectCode() const;

protected:
    Shotgun *m_sg; 
    std::string m_type;

    // This has to be an pointer since xmlrpc_c::value will not allow the
    // assignment of an already-instantiated xmlrpc_c::value. For details,
    // see: http://xmlrpc-c.sourceforge.net/doc/libxmlrpc++.html
    xmlrpc_c::value *m_attrs;
};

// *****************************************************************************
template <typename T>
const T Entity::getAttrValue(const std::string &attrName) const
{
    T result;

    xmlrpc_c::value genericResult = getAttrValue(attrName);

    try
    {
        fromXmlrpcValue(genericResult, result);
    }
    catch (SgXmlrpcValueTypeError &error)
    {
        throw SgAttrTypeError(attrName,
                              error.what());
    }

    return result;
}

// *****************************************************************************
template <typename T>
const T Entity::getAttrValue(const std::string &attrName,
                             const T defaultVal) const
{
    T result;

    xmlrpc_c::value genericResult = getAttrValue(attrName);

    try
    {
        fromXmlrpcValue(genericResult, result);
    }
    catch (SgXmlrpcValueTypeError &error)
    {
        result = defaultVal;
    }

    return result;
}

// *****************************************************************************
template <typename T>
const T Entity::getAttrValue(const std::string &attrName,
                             const Dict &attrsMap)
{
    T result;

    xmlrpc_c::value genericResult = getAttrValue(attrName, attrsMap);

    try
    {
        fromXmlrpcValue(genericResult, result);
    }
    catch (SgXmlrpcValueTypeError &error)
    {
        throw SgAttrTypeError(attrName,
                              error.what());
    }

    return result;
}

// *****************************************************************************
template <typename T>
const T Entity::getAttrValue(const std::string &attrName,
                             const Dict &attrsMap,
                             const T defaultVal)
{
    T result;

    xmlrpc_c::value genericResult = getAttrValue(attrName, attrsMap);

    try
    {
        fromXmlrpcValue(genericResult, result);
    }
    catch (SgXmlrpcValueTypeError &error)
    {
        result = defaultVal;
    }

    return result;
}

// *****************************************************************************
template <class T>
const T *Entity::getAttrValueAsEntity(const std::string &attrName) const
{
    Entity *entity = getAttrValueAsEntity(attrName);

    if (T *t = dynamic_cast<T *>(entity))
    {
        return t;
    }
    else
    {
        throw SgEntityDynamicCastError(T::type());
    }
}

// *****************************************************************************
template <class T>
const T *Entity::getAttrValueAsEntity(Shotgun *sg,
                                      const std::string &attrName,
                                      const Dict &attrsMap)
{
    Entity *entity = Entity::getAttrValueAsEntity(sg, attrName, attrsMap);

    if (T *t = dynamic_cast<T *>(entity))
    {
        return t;
    }
    else
    {
        throw SgEntityDynamicCastError(T::type());
    }
}

// *****************************************************************************
template <class T>
const std::vector<T *> Entity::getAttrValueAsEntities(const std::string &attrName) const
{
    std::vector<Entity *> entities = getAttrValueAsEntities(attrName);
    std::vector<T *> entityTs;
    
    for (size_t i = 0; i < entities.size(); i++)
    {
        if (T *t = dynamic_cast<T *>(entities[i]))
        {
            entityTs.push_back(t);
        }
    }

    return entityTs;
}

// *****************************************************************************
template <class T>
const std::vector<T *> Entity::getAttrValueAsEntities(Shotgun *sg,
                                                      const std::string &attrName,
                                                      const Dict &attrsMap)
{
    std::vector<Entity *> entities = Entity::getAttrValueAsEntities(sg, attrName, attrsMap);
    std::vector<T *> entityTs;
    
    for (size_t i = 0; i < entities.size(); i++)
    {
        if (T *t = dynamic_cast<T *>(entities[i]))
        {
            entityTs.push_back(t);
        }
    }

    return entityTs;
}

// *****************************************************************************
typedef std::vector<Entity *> EntityPtrs;

} // End namespace Shotgun

#endif    // End #ifdef __ENTITY_H__
