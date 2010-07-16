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

#include <Shotgun/Method.h>
#include <Shotgun/Entity.h>
#include <Shotgun/Shotgun.h>
#include <Shotgun/ReviewItem.h>
#include <Shotgun/Show.h>

namespace Shotgun {

// *****************************************************************************
ReviewItem::ReviewItem(Shotgun *sg, const xmlrpc_c::value &attrs)
    : Entity(sg)
{
    m_type = "ReviewItem";
    m_attrs = new xmlrpc_c::value(attrs);
}

// *****************************************************************************
ReviewItem::ReviewItem()
    : Entity(NULL)
{
    m_type = "ReviewItem";
    m_attrs = NULL;
}

// *****************************************************************************
ReviewItem::ReviewItem(const ReviewItem &ref)
    : Entity(ref.m_sg)
{
    m_type = "ReviewItem";
    m_attrs = new xmlrpc_c::value(*ref.m_attrs);
}

// *****************************************************************************
ReviewItem::~ReviewItem()
{
    // Nothing
}

// *****************************************************************************
ReviewItem ReviewItem::create(Shotgun *sg, 
                              const std::string &showCode,
                              //const std::string &reviewItemName, // read-only
                              const SgMap &reviewItemShotLink,
                              const SgMap &reviewItemDailyLink,
                              const SgMap &reviewItemReviewLink,
                              const std::string &reviewItemPurpose,
                              const int reviewItemOrder)
{
    // Check if the reviewItem already exists
    try
    {
        std::string reviewItemName = getAttrValueAsString("name", reviewItemDailyLink);
        ReviewItem reviewItem = sg->findReviewItemByName(showCode, reviewItemName);

        std::string err = "ReviewItem \"" + reviewItemName + "\" already exists.";
        throw SgEntityCreateError(err);
    }
    catch (SgEntityNotFoundError)
    {
        Show show = sg->findShowByCode(showCode);

        SgMap attrsMap;
        attrsMap["project"] = toXmlrpcValue(show.asLink());
        // Attribute "code" is read-only, I think it's filled in automatically 
        // based on the "sg_version" link.
        //attrsMap["code"] = toXmlrpcValue(reviewItemName); 
        attrsMap["sg_link"] = toXmlrpcValue(reviewItemShotLink);
        attrsMap["sg_version"] = toXmlrpcValue(reviewItemDailyLink);
        attrsMap["sg_review"] = toXmlrpcValue(reviewItemReviewLink);
        attrsMap["sg_purpose"] = toXmlrpcValue(reviewItemPurpose);

        if (reviewItemOrder != TIPSHOTGUN_INVALID_ORDER_NUM)
        {
            attrsMap["sg_order"] = toXmlrpcValue(reviewItemOrder);
        }

        // Call the base class function to create an entity
        return ReviewItem(sg, createEntity(sg, "ReviewItem", attrsMap));
    }
}

// *****************************************************************************
ReviewItems ReviewItem::find(Shotgun *sg, SgMap &findMap)
{
    // Find the entities that match the findMap and create a ReviewItem for each of them
    ReviewItems reviewItems;

    SgArray result = Entity::findEntities(sg, findMap);
    if (result.size() > 0)
    {
        for (size_t i = 0; i < result.size(); i++)
        {
            reviewItems.push_back(ReviewItem(sg, result[i]));
        }
    }

    return reviewItems;
}

} // End namespace Shotgun