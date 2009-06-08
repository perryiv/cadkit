
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __XERCES_ERROR_HANDLER_H__
#define __XERCES_ERROR_HANDLER_H__

#include "XmlTree/XercesString.h"

#include "xercesc/sax2/DefaultHandler.hpp"

#include <stdexcept>

namespace XmlTree
{
  class XercesErrorHandler : public xercesc::DefaultHandler
  {
  public:
    typedef xercesc::DefaultHandler BaseClass;

    XercesErrorHandler () : BaseClass()
    {
    }

    virtual ~XercesErrorHandler ()
    {
    }

    void warning ( const xercesc::SAXParseException &e )
    {
      // Nothing
    }

    void error ( const xercesc::SAXParseException &e )
    {
      throw std::runtime_error ( XmlTree::toNative( e.getMessage() ) );
    }

    void fatalError ( const xercesc::SAXParseException &e )
    {
      this->error ( e );
    }
  };
}

#endif // __XERCES_ERROR_HANDLER_H__
