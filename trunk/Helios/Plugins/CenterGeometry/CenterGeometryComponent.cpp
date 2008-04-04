
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#include "Helios/Plugins/CenterGeometry/CenterGeometryComponent.h"

#include "Usul/Adaptors/Bind.h"
#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Commands/GenericCommand.h"
#include "Usul/Components/Factory.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Interfaces/IModelsScene.h"

#include "MenuKit/Menu.h"
#include "MenuKit/Button.h"

#include "OsgTools/Group.h"
#include "OsgTools/Utilities/TranslateGeometry.h"

#include "osg/MatrixTransform"

#include <algorithm>

USUL_DECLARE_COMPONENT_FACTORY( CenterGeometryComponent );
USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( CenterGeometryComponent, CenterGeometryComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

CenterGeometryComponent::CenterGeometryComponent() : BaseClass()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

CenterGeometryComponent::~CenterGeometryComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *CenterGeometryComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin*>(this);
  case Usul::Interfaces::IMenuAdd::IID:
    return static_cast < Usul::Interfaces::IMenuAdd*>(this);
  default:
    return 0x0;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add to the menu.
//
///////////////////////////////////////////////////////////////////////////////

void CenterGeometryComponent::menuAdd ( MenuKit::Menu& m, Usul::Interfaces::IUnknown* caller )
{
	  // Typedefs.
  typedef MenuKit::Button Button;

  // Build the menu.
  MenuKit::Menu::RefPtr menu ( m.find ( "Tools", true ) );

	menu->append ( new Button ( Usul::Commands::genericCommand ( "Center Geometry", 
      Usul::Adaptors::memberFunction<void> ( this, &CenterGeometryComponent::centerGeometry ),
      Usul::Adaptors::memberFunction<bool> ( this, &CenterGeometryComponent::enableCenterGeometry ) ) ) );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Center the geometry.
//
///////////////////////////////////////////////////////////////////////////////

void CenterGeometryComponent::centerGeometry()
{
	Usul::Interfaces::IModelsScene::QueryPtr ms ( Usul::Documents::Manager::instance().activeView() );

	if ( false == ms.valid() )
		return;

	osg::ref_ptr<osg::Group> group ( ms->modelsScene() );

	if ( false == group.valid() )
		return;

	// Get the bounding sphere.
	osg::BoundingSphere bs ( group->getBound() );

	// Get the center.
	osg::Vec3d center ( bs.center() );

	// Visit the scene.
	osg::ref_ptr<OsgTools::Utilities::TranslateGeometry> visitor ( new OsgTools::Utilities::TranslateGeometry ( -center ) );
	group->accept ( *visitor );

	if ( osg::MatrixTransform *mt = dynamic_cast<osg::MatrixTransform*> ( group.get() ) )
	{
		osg::Matrixd matrix ( mt->getMatrix() );
		matrix.makeTranslate ( matrix.getTrans() + center );

		mt->setMatrix ( matrix );
	}
	else
	{
		osg::ref_ptr<osg::MatrixTransform> transform ( new osg::MatrixTransform );

		OsgTools::Group::addAllChildren ( group.get(), transform.get() );
		OsgTools::Group::removeAllChildren ( group.get() );

		group->addChild ( transform.get() );
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//  Enable the button?
//
///////////////////////////////////////////////////////////////////////////////

bool CenterGeometryComponent::enableCenterGeometry() const
{
	Usul::Interfaces::IModelsScene::QueryPtr ms ( Usul::Documents::Manager::instance().activeView() );
	return ms.valid();
}
