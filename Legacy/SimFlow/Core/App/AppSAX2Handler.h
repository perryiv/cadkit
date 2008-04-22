
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// Handle for SAX2. 
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __APP_APP_SAX2_HANDLER_H__
#define __APP_APP_SAX2_HANDLER_H__

#include <ostream>
#include <vector>
#include <string>

#include "SimFlow/Core/Export.h"

#include "SimFlow/Core/Parser/ISAX2Handler.h"
#include "SimFlow/Core/Scene/SceneManager.h"

#include "osg/ref_ptr"
#include "osg/Referenced"

namespace osg{ class Geode; }
namespace SimFlow
{ 
	namespace Layer
	{ 
		class PointTimeLayer;  
		class PolygonLayer;
	} 
}

namespace SimFlow
{
	namespace App
	{		
		class SIM_FLOW_EXPORT AppSAX2Handler : public SimFlow::XMLParser::ISAX2Handler
		{
		public:
      ///////////////////////////////////////////////////////////////////
			//
			// Typeefs.
			//
			///////////////////////////////////////////////////////////////////
      typedef std::vector< std::string >  FileNames;
			typedef osg::ref_ptr< Scene::SceneManager > SceneManagerPtr;

      ///////////////////////////////////////////////////////////////////
			//
			// Constructor. 
			//
			///////////////////////////////////////////////////////////////////
			AppSAX2Handler() : 
			ISAX2Handler(),
			mSceneStartTag  ( false ),
			mSceneEndTag		( false ),
			mGroupStartTag	( false ),
			mGroupEndTag		( false ),
			mSourceStartTag	( false ),
			mSourceEndTag		( false ), 
			mLayerType			( "" ), 
			mPrimitiveType	( 1  ), 
			mPrimitiveSize	( 1.0 )
			{
			}

			///////////////////////////////////////////////////////////////////
			//
			// Destructor.
			//
			///////////////////////////////////////////////////////////////////
			~AppSAX2Handler()
			{
			}

			///////////////////////////////////////////////////////////////////
			//
			// Pointer to the SceneManager.
			//
			///////////////////////////////////////////////////////////////////
			void sceneManager( Scene::SceneManager* sm )
			{
				mSceneManager = sm;
			}

			///////////////////////////////////////////////////////////////////
			//
			//  Handlers for the SAX ContentHandler interface
			//
			///////////////////////////////////////////////////////////////////
			virtual void          startElement( const XMLCh* const uri, 
									                        const XMLCh* const localname, 
									                        const XMLCh* const qname, 
									                        const Attributes& attrs );

			virtual void          endElement  ( const XMLCh* const uri, 
									                        const XMLCh* const localname, 
									                        const XMLCh* const qname );

			virtual void          characters  ( const XMLCh* const chars, 
									                        const unsigned int length );			
			
			virtual void          addPointTimeData( SimFlow::Layer::PointTimeLayer* );			

			virtual void          addDataAsPolygonData( SimFlow::Layer::PolygonLayer*, unsigned int );

			//virtual void extractData( Scene::Sequence* seq, Scene::Model* model );

	private:			
			///////////////////////////////////////////////////////////////////
			//
			// Flags that determine the element active at a given time.
			//
			///////////////////////////////////////////////////////////////////			
			bool								  mSceneStartTag;
			bool								  mSceneEndTag;			

			bool								  mGroupStartTag;
			bool								  mGroupEndTag;

			bool								  mSourceStartTag;
			bool								  mSourceEndTag;

			string								mLayerType;
			unsigned int					mPrimitiveType;
			float								  mPrimitiveSize;


			///////////////////////////////////////////////////////////////////
			//
			// 
			//
			///////////////////////////////////////////////////////////////////
			SceneManagerPtr       mSceneManager;

			///////////////////////////////////////////////////////////////////
			//
			//
			//
			///////////////////////////////////////////////////////////////////
			FileNames			        mFileNames;
		};
	}
}

#endif // __APP_APP_SAX2_HANDLER_H__

