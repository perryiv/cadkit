
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CORE_SEQUENCE_H__
#define __CORE_SEQUENCE_H__

#include <osg/Group>
#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>

#include <cassert>

#include "SimFlow/Core/Export.h"

namespace core
{
	class SIM_FLOW_EXPORT Sequence : public osg::Group
	{
		public:		
			typedef std::vector	< osg::Vec3f >									PointPositions;				
			typedef std::vector	< PointPositions >								PointPositionsSet;
			typedef std::vector	< osg::ref_ptr< osg::MatrixTransform > >		MatrixTransforms;

			enum Mode
			{
				PLAY	= 0, 
				PAUSE	= 1, 
				STOP	= 2
			};

			Sequence() : Group(), 
				mSpeed			( 0.05 ),
				mNoOfRepeats	( -1.0 ), 
				mMode			( PLAY ),
				mMinPoints		( 0    ),
				mMaxPoints		( 0	   ),
				mTimeStep		( 0	   )
			{	
				try
				{
					osg::ref_ptr< osg::Sphere > sphere = new osg::Sphere();
					sphere->setRadius( 0.004 );			
					sphere->setCenter( osg::Vec3f( 0.0, 0.0, 0.0 ) );
					osg::ref_ptr< osg::ShapeDrawable > shapeData = new osg::ShapeDrawable( sphere.get() );	
					shapeData->setColor( osg::Vec4( 1.00, 0.80, 0.20, 1.0 ) );
					mDrawableObject = new osg::Geode();
					mDrawableObject->addDrawable( shapeData.get() );
				}
				catch( char* error )
				{
					std::cerr << "[Sequence()] Exception raised: " << error << std::endl;
				}
			}

			Sequence( const Sequence& seq ) : Group( seq ), 
				mSpeed		( 0.05 ), 
				mNoOfRepeats( -1.0 ), 
				mMinPoints	( 0    ),
				mMaxPoints	( 0	   ),
				mMode		( PLAY )
			{
			}

			virtual void setDrawable( osg::Drawable* drawable );

			virtual osg::Geode* getRenderObject();

			virtual void addPositions( std::vector< osg::Vec3f > pointPos )
			{				
				mPointPositionsSet.push_back( pointPos );
			}

			virtual osg::MatrixTransform* getMatrixTransform( const int& index )
			{
				assert( index != mMatrixTransforms.size() );
				return mMatrixTransforms[index].get();
			}

			virtual osg::Group* getRoot()
			{				
				return this;
			}

			virtual void buildScene()
			{
				// Making an assumption that all the time steps contains same number of points. 
				mMaxPoints = getMaxSize( mPointPositionsSet );
				mMinPoints = getMinSize( mPointPositionsSet );

				for( size_t i=0; i < mMaxPoints; ++i )
				{						
					osg::ref_ptr< osg::MatrixTransform > matTransform = new osg::MatrixTransform();
					matTransform->addChild( mDrawableObject.get() );
					mMatrixTransforms.push_back( matTransform.get() );					
					this->addChild( mMatrixTransforms[i].get() );
				}

				for( size_t i=0; i < mPointPositionsSet[0].size(); ++i )
				{
					osg::Vec3f pos = mPointPositionsSet[0][i];
					mMatrixTransforms[i]->setMatrix( osg::Matrixf::translate(  pos ) );
				}				
			}

			// Move on to the next time step. 
			virtual void next()
			{
				++mTimeStep;
				if( mTimeStep >= mPointPositionsSet.size() )
				{
					mTimeStep = 0;
				}
			}

			virtual void run()
			{		
				next();				

				for( size_t i=0; i < mMaxPoints; ++i )
				{
					if( i < mPointPositionsSet[mTimeStep].size() )
					{
						mMatrixTransforms[i]->setNodeMask( 0xff );
						mMatrixTransforms[i]->setMatrix( osg::Matrix::translate( mPointPositionsSet[mTimeStep][i] ) );					
					}
					else
					{
						mMatrixTransforms[i]->setNodeMask( 0x00 );
					}
				}				
			}	

			virtual void play()
			{	
				mMode = PLAY;
			}

			virtual void pause()
			{
				mMode = PAUSE;
			}

			void traverse( osg::NodeVisitor &nv )
			{
				setNumChildrenRequiringUpdateTraversal( 1 );
				if( nv.getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR )
				{
					if( mMode == PLAY )
					{
						run();
					}
				}
				
				osg::Group::traverse( nv );
			}

			unsigned int getMaxSize( PointPositionsSet pps )
			{
				unsigned int max = 0;

				for( size_t i=0; i < mPointPositionsSet.size(); ++i )
				{
					if( mPointPositionsSet[i].size() > max )
					{
						max = mPointPositionsSet[i].size();
					}
				}

				return max;
			}

			unsigned int getMinSize( PointPositionsSet pps )
			{
				unsigned int min = 0;

				for( size_t i=0; i < mPointPositionsSet.size(); ++i )
				{
					if( mPointPositionsSet[i].size() < min )
					{
						min = mPointPositionsSet[i].size();
					}
				}

				return min;
			}

			unsigned int getCurrTimeStep()
			{
				return mTimeStep;	
			}

			unsigned int getCurrSize()
			{
				return mPointPositionsSet[mTimeStep].size();
			}


		protected:
			
			virtual ~Sequence()
			{
			}

		protected:
			float							mSpeed;
			int								mNoOfRepeats;			

			unsigned int					mMinPoints;
			unsigned int					mMaxPoints;			
			unsigned int					mTimeStep; 

			Mode							mMode;

			PointPositionsSet				mPointPositionsSet;
			MatrixTransforms				mMatrixTransforms;						

			osg::ref_ptr< osg::Geode >		mDrawableObject;			
	};
}

#endif // __CORE_SEQUENCE_H__