
///////////////////////////////////////////////////////////////////////////////
//
//  BSD License
//  http://www.opensource.org/licenses/bsd-license.html
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions are met:
//
//  - Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//
//  - Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution. 
//
//  - Neither the name of the CAD Toolkit nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission. 
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbXmlVisitorMacros.h: Macros that are used in visitor classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_XML_LIBRARY_VISITOR_MACROS_H_
#define _CADKIT_DATABASE_XML_LIBRARY_VISITOR_MACROS_H_

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include "Standard/SlRefBase.h"
#endif


///////////////////////////////////////////////////////////////////////////////
//
// Put this in your abstract visitor's class definition.
//
///////////////////////////////////////////////////////////////////////////////

#define DB_XML_DECLARE_VISITOR(class_name, id) \
  SL_DECLARE_REFCOUNT_TYPE(class_name) \
  SL_DECLARE_CLASS(class_name, id)


///////////////////////////////////////////////////////////////////////////////
//
// Put this in your concrete visitor's class definition.
//
///////////////////////////////////////////////////////////////////////////////

#define DB_XML_DECLARE_DYNAMIC_VISITOR(class_name, id) \
  SL_DECLARE_REFCOUNT_TYPE(class_name) \
  SL_DECLARE_DYNAMIC_CLASS(class_name, id)


///////////////////////////////////////////////////////////////////////////////
//
// Put this in your abstract visitor's implementation file.
//
///////////////////////////////////////////////////////////////////////////////

#define DB_XML_IMPLEMENT_VISITOR(class_name, base_class_name) \
  SL_IMPLEMENT_CLASS(class_name, base_class_name)


///////////////////////////////////////////////////////////////////////////////
//
// Put this in your concrete visitor's implementation file.
//
///////////////////////////////////////////////////////////////////////////////

#define DB_XML_IMPLEMENT_DYNAMIC_VISITOR(class_name, base_class_name) \
  SL_IMPLEMENT_DYNAMIC_CLASS(class_name, base_class_name)


#endif // _CADKIT_DATABASE_XML_LIBRARY_VISITOR_MACROS_H_
