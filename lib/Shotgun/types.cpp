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

#include <time.h>

#include <Shotgun/types.h>
#include <Shotgun/List.h>
#include <Shotgun/Dict.h>
#include <Shotgun/FilterBy.h>
#include <Shotgun/SortBy.h>
#include <Shotgun/Fields.h>

extern char *tzname[2];
extern long timezone;
extern int daylight;

namespace SG {

// *****************************************************************************
std::string xmlrpcValueTypeStr(const xmlrpc_c::value::type_t xmlrpcType)
{
    switch (xmlrpcType)
    {
        case xmlrpc_c::value::TYPE_INT:
            return std::string("TYPE_INT");
        case xmlrpc_c::value::TYPE_BOOLEAN:
            return std::string("TYPE_BOOLEAN");
        case xmlrpc_c::value::TYPE_DOUBLE:
            return std::string("TYPE_DOUBLE");
        case xmlrpc_c::value::TYPE_DATETIME:
            return std::string("TYPE_DATETIME");
        case xmlrpc_c::value::TYPE_STRING:
            return std::string("TYPE_STRING");
        case xmlrpc_c::value::TYPE_BYTESTRING:
            return std::string("TYPE_BYTESTRING");
        case xmlrpc_c::value::TYPE_ARRAY:
            return std::string("TYPE_ARRAY");
        case xmlrpc_c::value::TYPE_STRUCT:
            return std::string("TYPE_STRUCT");
        case xmlrpc_c::value::TYPE_C_PTR:
            return std::string("TYPE_C_PTR");
        case xmlrpc_c::value::TYPE_NIL:
            return std::string("TYPE_NIL");
        case xmlrpc_c::value::TYPE_DEAD:
        default:
            return std::string("TYPE_DEAD");
    }
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const char *in)
{
    return xmlrpc_c::value(xmlrpc_c::value_string(std::string(in)));
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const std::string &in)
{
    return xmlrpc_c::value(xmlrpc_c::value_string(in));
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const int &in)
{
    return xmlrpc_c::value(xmlrpc_c::value_int(in));
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const double &in)
{
    return xmlrpc_c::value(xmlrpc_c::value_double(in));
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const bool &in)
{
    return xmlrpc_c::value(xmlrpc_c::value_boolean(in));
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const time_t &in)
{
    return xmlrpc_c::value(xmlrpc_c::value_datetime(in));
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const struct tm &in)
{
    // This converts the local time to UTC time
    time_t time = mktime((struct tm *)&in);

    return xmlrpc_c::value(xmlrpc_c::value_datetime(time));
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const std::vector<xmlrpc_c::value> &in)
{
    return xmlrpc_c::value(xmlrpc_c::value_array(in));
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const std::map<std::string, xmlrpc_c::value> &in)
{
    return xmlrpc_c::value(xmlrpc_c::value_struct(in));
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const Strings &in)
{
    std::vector<xmlrpc_c::value> strArray;
   
    for (size_t i = 0; i < in.size(); i++)
    {
        strArray.push_back(toXmlrpcValue(in[i]));
    }

    return xmlrpc_c::value(xmlrpc_c::value_array(strArray));
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const MethodSignatures &in)
{
    std::vector<xmlrpc_c::value> strArray;
   
    for (size_t i = 0; i < in.size(); i++)
    {
        strArray.push_back(toXmlrpcValue(in[i]));
    }

    return xmlrpc_c::value(xmlrpc_c::value_array(strArray));
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const List &in)
{
    return xmlrpc_c::value(xmlrpc_c::value_array(in.value()));
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const Dict &in)
{
    return xmlrpc_c::value(xmlrpc_c::value_struct(in.value()));
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const FilterBy &in)
{
    return toXmlrpcValue(in.filters());
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const SortBy &in)
{
    return toXmlrpcValue(in.sorts());
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const Fields &in)
{
    return toXmlrpcValue(in.fields());
}

// *****************************************************************************
xmlrpc_c::value toXmlrpcValue(const xmlrpc_c::value &in)
{
    return in;
}

// *****************************************************************************
void fromXmlrpcValue(const xmlrpc_c::value &value, char *out)
{
    if (value.type() == xmlrpc_c::value::TYPE_STRING)
    {
        out = strdup((std::string(xmlrpc_c::value_string(value))).c_str());
    }
    else if (value.type() == xmlrpc_c::value::TYPE_NIL)
    {
        throw SgXmlrpcValueIsNilError();
    }
    else
    {
        throw SgXmlrpcValueTypeError(value,
                                     xmlrpc_c::value::TYPE_STRING,
                                     value.type());
    }
}

// *****************************************************************************
void fromXmlrpcValue(const xmlrpc_c::value &value, std::string &out)
{
    if (value.type() == xmlrpc_c::value::TYPE_STRING)
    {
        out = std::string(xmlrpc_c::value_string(value));
    }
    else if (value.type() == xmlrpc_c::value::TYPE_NIL)
    {
        throw SgXmlrpcValueIsNilError();
    }
    else
    {
        throw SgXmlrpcValueTypeError(value,
                                     xmlrpc_c::value::TYPE_STRING,
                                     value.type());
    }
}

// *****************************************************************************
void fromXmlrpcValue(const xmlrpc_c::value &value, int &out)
{
    if (value.type() == xmlrpc_c::value::TYPE_INT)
    {
        out = int(xmlrpc_c::value_int(value));
    }
    else if (value.type() == xmlrpc_c::value::TYPE_NIL)
    {
        throw SgXmlrpcValueIsNilError();
    }
    else
    {
        throw SgXmlrpcValueTypeError(value,
                                     xmlrpc_c::value::TYPE_INT,
                                     value.type());
    }
}

// *****************************************************************************
void fromXmlrpcValue(const xmlrpc_c::value &value, double &out)
{
    if (value.type() == xmlrpc_c::value::TYPE_DOUBLE)
    {
        out = double(xmlrpc_c::value_double(value));
    }
    else if (value.type() == xmlrpc_c::value::TYPE_NIL)
    {
        throw SgXmlrpcValueIsNilError();
    }
    else
    {
        throw SgXmlrpcValueTypeError(value,
                                     xmlrpc_c::value::TYPE_DOUBLE,
                                     value.type());
    }
}

// *****************************************************************************
void fromXmlrpcValue(const xmlrpc_c::value &value, bool &out)
{
    if (value.type() == xmlrpc_c::value::TYPE_BOOLEAN)
    {
        out = bool(xmlrpc_c::value_boolean(value));
    }
    else if (value.type() == xmlrpc_c::value::TYPE_NIL)
    {
        throw SgXmlrpcValueIsNilError();
    }
    else
    {
        throw SgXmlrpcValueTypeError(value,
                                     xmlrpc_c::value::TYPE_BOOLEAN,
                                     value.type());
    }
}

// *****************************************************************************
void fromXmlrpcValue(const xmlrpc_c::value &value, time_t &out)
{
    if (value.type() == xmlrpc_c::value::TYPE_DATETIME)
    {
        out = time_t(xmlrpc_c::value_datetime(value));
    }
    else if (value.type() == xmlrpc_c::value::TYPE_NIL)
    {
        throw SgXmlrpcValueIsNilError();
    }
    else
    {
        throw SgXmlrpcValueTypeError(value,
                                     xmlrpc_c::value::TYPE_DATETIME,
                                     value.type());
    }
}

// *****************************************************************************
void fromXmlrpcValue(const xmlrpc_c::value &value, struct tm &out)
{
    if (value.type() == xmlrpc_c::value::TYPE_DATETIME)
    {
        time_t t = time_t(xmlrpc_c::value_datetime(value));

        // This converts the UTC time to local time
        out = *(localtime(&t));
    }
    else if (value.type() == xmlrpc_c::value::TYPE_NIL)
    {
        throw SgXmlrpcValueIsNilError();
    }
    else
    {
        throw SgXmlrpcValueTypeError(value,
                                     xmlrpc_c::value::TYPE_DATETIME,
                                     value.type());
    }
}

// *****************************************************************************
void fromXmlrpcValue(const xmlrpc_c::value &value, std::vector<xmlrpc_c::value> &out)
{
    if (value.type() == xmlrpc_c::value::TYPE_ARRAY)
    {
        out = (xmlrpc_c::value_array(value)).vectorValueValue();
    }
    else if (value.type() == xmlrpc_c::value::TYPE_NIL)
    {
        throw SgXmlrpcValueIsNilError();
    }
    else
    {
        throw SgXmlrpcValueTypeError(value,
                                     xmlrpc_c::value::TYPE_ARRAY,
                                     value.type());
    }
}

// *****************************************************************************
void fromXmlrpcValue(const xmlrpc_c::value &value, std::map<std::string, xmlrpc_c::value> &out)
{
    if (value.type() == xmlrpc_c::value::TYPE_STRUCT)
    {
        out = std::map<std::string, xmlrpc_c::value>(xmlrpc_c::value_struct(value));
    }
    else if (value.type() == xmlrpc_c::value::TYPE_NIL)
    {
        throw SgXmlrpcValueIsNilError();
    }
    else
    {
        throw SgXmlrpcValueTypeError(value,
                                     xmlrpc_c::value::TYPE_STRUCT,
                                     value.type());
    }
}

// *****************************************************************************
void fromXmlrpcValue(const xmlrpc_c::value &value, Strings &out)
{
    if (value.type() == xmlrpc_c::value::TYPE_ARRAY)
    {
        std::vector<xmlrpc_c::value> array = (xmlrpc_c::value_array(value)).vectorValueValue();
        
        Strings strings;
        for (size_t i = 0; i < array.size(); i++)
        {
            if (array[i].type() == xmlrpc_c::value::TYPE_STRING)
            {
                strings.push_back(std::string(xmlrpc_c::value_string(array[i])));
            }
            else
            {
                throw SgXmlrpcValueTypeError(array[i],
                                             xmlrpc_c::value::TYPE_STRING,
                                             array[i].type());
            }
        }

        out = strings;
    }
    else if (value.type() == xmlrpc_c::value::TYPE_NIL)
    {
        throw SgXmlrpcValueIsNilError();
    }
    else
    {
        throw SgXmlrpcValueTypeError(value,
                                     xmlrpc_c::value::TYPE_ARRAY,
                                     value.type());
    }
}

// *****************************************************************************
void fromXmlrpcValue(const xmlrpc_c::value &value, List &out)
{
    if (value.type() == xmlrpc_c::value::TYPE_ARRAY)
    {
        out = List((xmlrpc_c::value_array(value)).vectorValueValue());
    }
    else if (value.type() == xmlrpc_c::value::TYPE_NIL)
    {
        throw SgXmlrpcValueIsNilError();
    }
    else
    {
        throw SgXmlrpcValueTypeError(value,
                                     xmlrpc_c::value::TYPE_ARRAY,
                                     value.type());
    }
}

// *****************************************************************************
void fromXmlrpcValue(const xmlrpc_c::value &value, Dict &out)
{
    if (value.type() == xmlrpc_c::value::TYPE_STRUCT)
    {
        out = Dict(std::map<std::string, xmlrpc_c::value>(xmlrpc_c::value_struct(value)));
    }
    else if (value.type() == xmlrpc_c::value::TYPE_NIL)
    {
        throw SgXmlrpcValueIsNilError();
    }
    else
    {
        throw SgXmlrpcValueTypeError(value,
                                     xmlrpc_c::value::TYPE_STRUCT,
                                     value.type());
    }
}

// *****************************************************************************
void fromXmlrpcValue(const xmlrpc_c::value &value, FilterBy &out)
{
    if (value.type() == xmlrpc_c::value::TYPE_STRUCT)
    {
        out = FilterBy(Dict(std::map<std::string, xmlrpc_c::value>(xmlrpc_c::value_struct(value))));
    }
    else if (value.type() == xmlrpc_c::value::TYPE_NIL)
    {
        throw SgXmlrpcValueIsNilError();
    }
    else
    {
        throw SgXmlrpcValueTypeError(value,
                                     xmlrpc_c::value::TYPE_STRUCT,
                                     value.type());
    }
}

// *****************************************************************************
void fromXmlrpcValue(const xmlrpc_c::value &value, SortBy &out)
{
    if (value.type() == xmlrpc_c::value::TYPE_ARRAY)
    {
        out = SortBy(List((xmlrpc_c::value_array(value)).vectorValueValue()));
    }
    else if (value.type() == xmlrpc_c::value::TYPE_NIL)
    {
        throw SgXmlrpcValueIsNilError();
    }
    else
    {
        throw SgXmlrpcValueTypeError(value,
                                     xmlrpc_c::value::TYPE_ARRAY,
                                     value.type());
    }
}

// *****************************************************************************
void fromXmlrpcValue(const xmlrpc_c::value &value, Fields &out)
{
    if (value.type() == xmlrpc_c::value::TYPE_ARRAY)
    {
        out = Fields((xmlrpc_c::value_array(value)).vectorValueValue());
    }
    else if (value.type() == xmlrpc_c::value::TYPE_NIL)
    {
        throw SgXmlrpcValueIsNilError();
    }
    else
    {
        throw SgXmlrpcValueTypeError(value,
                                     xmlrpc_c::value::TYPE_ARRAY,
                                     value.type());
    }
}

// *****************************************************************************
void fromXmlrpcValue(const xmlrpc_c::value &value, xmlrpc_c::value &out)
{
    out = value;
}

// *****************************************************************************
std::string currDateStr()
{
    // Get the current time
    time_t rawTime;
    time(&rawTime);

    // Convert to a time string
    struct tm *timeInfo = localtime(&rawTime);
    char timeStr[10];
    strftime(timeStr, 20, "%Y-%m-%d", timeInfo);

    return std::string(timeStr);
}

} // End namespace SG

// *****************************************************************************
// *****************************************************************************
std::string toStdString(const int val)
{
    char str[1024];
    sprintf(str, "%d", val);
    return std::string(str);
}

std::string toStdString(const bool val)
{
    char str[1024];
    sprintf(str, "%d", val);
    return std::string(str);
}

std::string toStdString(const double val)
{
    char str[1024];
    sprintf(str, "%f", val);
    return std::string(str);
}

std::string toStdString(const time_t val)
{
    char str[1024];
    sprintf(str, "%u", val);
    return std::string(str);
}

std::string toStdString(const struct tm &val)
{
    return std::string(asctime(&val));
}

std::string toStdString(const xmlrpc_c::value &value)
{
    static int depth = 0;
    
    std::string output;
    char str[1024];

    if (value.type() == xmlrpc_c::value::TYPE_INT)
    { 
        sprintf(str, "%d", int(xmlrpc_c::value_int(value)));
        output = str;
    }
    else if (value.type() == xmlrpc_c::value::TYPE_BOOLEAN)
    {
        sprintf(str, "%d", bool(xmlrpc_c::value_boolean(value)));
        output = str;
    }
    else if (value.type() == xmlrpc_c::value::TYPE_DOUBLE)
    {
        sprintf(str, "%f", double(xmlrpc_c::value_double(value)));
        output = str;
    }
    else if (value.type() == xmlrpc_c::value::TYPE_DATETIME)
    {
        sprintf(str, "%u", time_t(xmlrpc_c::value_datetime(value)));
        output = str;
    }
    else if (value.type() == xmlrpc_c::value::TYPE_STRING)
    {
        output = "\"" + std::string(xmlrpc_c::value_string(value)) + "\"";
    }
    else if (value.type() == xmlrpc_c::value::TYPE_BYTESTRING)
    {
        sprintf(str, "%u", (xmlrpc_c::value_bytestring(value)).length());
        output = str;
    }
    else if (value.type() == xmlrpc_c::value::TYPE_ARRAY)
    {
        std::string whitespace = "    ";
        std::string indent;
        std::string indent2;
        for (size_t l = 0; l < depth; l++)
        {
            indent += whitespace;
        }
        indent2 = indent + whitespace;
        depth++;
        
        // This helps save the current recursive state
        size_t old_depth = depth;

        output = "\n" + indent + "[\n";

        std::vector<xmlrpc_c::value> array = xmlrpc_c::value_array(value).vectorValueValue();
        for (size_t i = 0; i < array.size(); i++)
        {
            if (i == (array.size() - 1))
            {
                output += indent2 + toStdString(array[i]) + "\n";
            }
            else
            {
                output += indent2 + toStdString(array[i]) + ", \n";
            }

            // This restores the current recursive state
            depth = old_depth;
        }

        depth = old_depth - 1;

        output += indent + "]";
    }
    else if (value.type() == xmlrpc_c::value::TYPE_STRUCT)
    {
        std::string whitespace = "    ";
        std::string indent;
        std::string indent2;
        for (size_t l = 0; l < depth; l++)
        {
            indent += whitespace;
        }
        indent2 = indent + whitespace;
        depth++;

        // This helps save the current recursive state
        size_t old_depth = depth;

        output = "\n" + indent + "{\n";

        std::map<std::string, xmlrpc_c::value> map = std::map<std::string, xmlrpc_c::value>(xmlrpc_c::value_struct(value));
        size_t count = 0;
        for (std::map<std::string, xmlrpc_c::value>::const_iterator mIter = map.begin(); mIter != map.end(); mIter++)
        {
            count++;
            if (count == map.size())
            {
                output += indent2 + "\"" + (*mIter).first + "\" : " + toStdString((*mIter).second) + "\n";
            }
            else
            {
                output += indent2 + "\"" + (*mIter).first + "\" : " + toStdString((*mIter).second) + ", \n";
            }

            // This restores the current recursive state
            depth = old_depth;
        }

        depth = old_depth - 1;

        output += indent + "}";
    }
    else if (value.type() == xmlrpc_c::value::TYPE_NIL)
    {
        output = "nil";
    }

    return output;
}

// *****************************************************************************
std::string toStdString(const std::map<std::string, xmlrpc_c::value> &map)
{
    return toStdString(xmlrpc_c::value_struct(map));
}

// *****************************************************************************
std::string toStdString(const std::vector<xmlrpc_c::value> &array)
{
    return toStdString(xmlrpc_c::value_array(array));
}

// *****************************************************************************
std::string toStdString(const SG::Strings &strs)
{
    std::string output = "[";

    if (strs.size() > 0)
    {
        for (size_t i = 0; i < strs.size()-1; i++)
        {
            output += "\"" + strs[i] + "\", ";
        }

        output += "\"" + strs[strs.size()-1] + "\"";
    }

    output += "]";

    return output;
}

// *****************************************************************************
std::string toStdString(const SG::MethodSignatures &sigs)
{
    std::string output = "[";

    if (sigs.size() > 0)
    {
        for (size_t i = 0; i < sigs.size()-1; i++)
        {
            output += toStdString(sigs[i]) + ", ";
        }

        output += toStdString(sigs[sigs.size()-1]);
    }

    output += "]";

    return output;
}

// *****************************************************************************
std::ostream &operator<<(std::ostream& output, const xmlrpc_c::value &value)
{
    output << toStdString(value);
    return output;
}

// *****************************************************************************
std::ostream &operator<<(std::ostream& output, const struct tm &time)
{
    output << toStdString(time);
    return output;
}

// *****************************************************************************
std::ostream &operator<<(std::ostream& output, const std::map<std::string, xmlrpc_c::value> &map)
{
    output << toStdString(map);
    return output;
}

// *****************************************************************************
std::ostream &operator<<(std::ostream& output, const std::vector<xmlrpc_c::value> &array)
{
    output << toStdString(array);
    return output;
}

// *****************************************************************************
std::ostream &operator<<(std::ostream& output, const SG::Strings &strs)
{
    output << toStdString(strs);
    return output;
}

// *****************************************************************************
std::ostream &operator<<(std::ostream& output, const SG::MethodSignatures &sigs)
{
    output << toStdString(sigs);
    return output;
}


