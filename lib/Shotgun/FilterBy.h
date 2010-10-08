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

#ifndef __FILTERBY_H__
#define __FILTERBY_H__

#include <string>

#include <Shotgun/types.h>
#include <Shotgun/List.h>
#include <Shotgun/Dict.h>

namespace Shotgun {

// *****************************************************************************
class FilterBy
{
public:
    FilterBy();
    FilterBy(const Dict &filters);

    template <typename T>
    FilterBy(const std::string &path,
             const std::string &relation,
             const T &value);

    // -------------------------------------------------------------------
    // logic "and"
    template <typename T>
    FilterBy &And(const std::string &path,
                  const std::string &relation,
                  const T &value);

    FilterBy &And(const FilterBy &that) { return op("and", that); }

    // -------------------------------------------------------------------
    // logic "or"
    template <typename T>
    FilterBy &Or(const std::string &path,
                 const std::string &relation,
                 const T &value);
    
    FilterBy &Or(const FilterBy &that) { return op("or", that); }


    const Dict &filters() const { return m_filters; }
    const bool empty() const { return m_filters.empty(); }
    const int size() const { return m_filters.size(); }
    const std::string str() const { return m_filters.str(); }
    void clear() { m_filters.clear(); }

    FilterBy &operator=(const FilterBy &that)
    {
        if (this != &that)
        {
            m_filters = that.m_filters;
        }

        return *this;
    }

    friend std::ostream& operator<<(std::ostream &output, const FilterBy &filterBy)
    {
        output << filterBy.str();
        return output;
    }

protected:
    FilterBy &op(const std::string &logicOperator,
                 const std::string &path,
                 const std::string &relation,
                 const xmlrpc_c::value &value);
    FilterBy &op(const std::string &logicOperator,
                 const FilterBy &that);

    Dict m_filters;
};

// *****************************************************************************
template <typename T>
FilterBy::FilterBy(const std::string &path,
                   const std::string &relation,
                   const T &value)
{
     op("and", path, relation, toXmlrpcValue(value));      
}

// *****************************************************************************
template <typename T>
FilterBy &FilterBy::And(const std::string &path,
                        const std::string &relation,
                        const T &value)
{
    return op("and", path, relation, toXmlrpcValue(value));
}

// *****************************************************************************
template <typename T>
FilterBy &FilterBy::Or(const std::string &path,
                       const std::string &relation,
                       const T &value)
{
    return op("or", path, relation, toXmlrpcValue(value));
}

} // End namespace Shotgun

#endif    // End #ifdef __FILTERBY_H__
