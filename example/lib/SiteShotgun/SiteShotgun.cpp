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

#include <SiteShotgun/SiteShotgun.h>
#include <SiteShotgun/SiteProject.h>
#include <SiteShotgun/SiteReference.h>

namespace SiteSG {

// *****************************************************************************
SiteShotgun::SiteShotgun(const std::string &serverURL,
                         const std::string &authName,
                         const std::string &authKey)
    : SG::Shotgun(serverURL, authName, authKey)
{
    // ------------------------------------------------------------------------
    // Register the site-specific classes. This either creates a new registry
    // entry or overrides an existing one that has the same shotgun entity type.
    //
    // For example, "SiteReference" entity is actually of "CustomEntity02" type
    // inside Shotgun. The re-registration of "CustomEntity02" entity will have 
    // the correct factory function return a "SiteReference" class object instead 
    // of a "CustomEntity02" class object.
    // ------------------------------------------------------------------------

    registerClass("Project",          &SiteProject::factory,      &SiteProject::defaultReturnFields);
    registerClass("CustomEntity02",   &SiteReference::factory,    &SiteReference::defaultReturnFields);
}

// *****************************************************************************
SiteShotgun::~SiteShotgun()
{
    // Nothing
}

} // End namespace SiteSG
