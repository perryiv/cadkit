
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Input dialog for files.
//
///////////////////////////////////////////////////////////////////////////////

#include "ReactionDialog.h"
#include "Usul/Strings/Format.h"
#include "Usul/Exceptions/Canceled.h"

#include <QtGui/QCheckBox>

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

ReactionDialog::ReactionDialog ( QWidget *parent ) : 
BaseClass ( parent ),
_reactions()
{
  //// Initialize code from Designer.
  this->setupUi ( this );  

  QStringList titles;

  titles.push_back( "Chemical" );
  titles.push_back( "Reaction Type" );
  titles.push_back( "1st-order rate constant [1/h]" );
  titles.push_back( "Oxygen Stequiometric ratio [g-oxygen/g-carbon]" );
  titles.push_back( "CO2 Stequiometric Ratio [g-CO2/g-carbon]" );

  _reactionTable->setHorizontalHeaderLabels( titles );

  // size the columns
  _reactionTable->resizeColumnsToContents();

  //initialize the table view
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

ReactionDialog::~ReactionDialog()
{
  
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize ( Public ).
//
///////////////////////////////////////////////////////////////////////////////

void ReactionDialog::initialize()
{
  this->_initialize();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize ( Private ).
//
///////////////////////////////////////////////////////////////////////////////

void ReactionDialog::_initialize()
{
  // first pass through Source values?
  bool firstPass( true );

  // the current number of rows
  unsigned int rowCount ( 0 );

  for( unsigned int i = 0; i < _reactions.size(); ++i )
  {
    // create a Source object
    Reaction rx ( _reactions.at( i ) );

    // add a row
    _reactionTable->insertRow( rowCount );

     // create an item widget for the first column
    QTableWidgetItem *item0 = new QTableWidgetItem;
    item0->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter ); 

    //// create a checkbox widget for the seventh column
    //QTableWidgetItem *item6 = new QTableWidgetItem;
    //item6->data( Qt::CheckStateRole );
    //item6->setCheckState( Qt::Unchecked );

    // create an item widget for the second column
    QTableWidgetItem *item1 = new QTableWidgetItem;
    item1->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    // create an item widget for the third column
    QTableWidgetItem *item2 = new QTableWidgetItem;
    item2->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    // create an item widget for the fourth column
    QTableWidgetItem *item3 = new QTableWidgetItem;
    item3->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    // create an item widget for the fifth column
    QTableWidgetItem *item4 = new QTableWidgetItem;
    item4->setTextAlignment( Qt::AlignLeft | Qt::AlignVCenter );

    if( true == firstPass )
    {
      // set the current item
      _reactionTable->setCurrentItem( item0 );

      // no longer the first pass
      firstPass = false;
    }

    // insert the columns
    _reactionTable->setItem( rowCount, 0, item0 );
    _reactionTable->setItem( rowCount, 1, item1 );
    _reactionTable->setItem( rowCount, 2, item2 );
    _reactionTable->setItem( rowCount, 3, item3 );
    _reactionTable->setItem( rowCount, 4, item4 );

    // set the values of the row
    _reactionTable->item( rowCount, 0 )->setText( rx.name.c_str()										);
    _reactionTable->item( rowCount, 1 )->setText( rx.type.c_str()										);
		_reactionTable->item( rowCount, 2 )->setText( rx.firstOrderRateConstant.c_str()	);
    _reactionTable->item( rowCount, 3 )->setText( rx.oxygenSRatio.c_str()						);
    _reactionTable->item( rowCount, 4 )->setText( rx.co2SRatio.c_str()							);

    ++rowCount;
  }

  // size the columns
  _reactionTable->resizeColumnsToContents();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clear out the reactions table
//
///////////////////////////////////////////////////////////////////////////////

void ReactionDialog::_clearTable()
{
  // remove all the rows
  for( int i = _reactionTable->rowCount() - 1; i >= 0 ; --i )
  {
    _reactionTable->removeRow( i );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the reactions
//
///////////////////////////////////////////////////////////////////////////////

ReactionDialog::Reactions ReactionDialog::reactions()
{
  return _reactions;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the reactions
//
///////////////////////////////////////////////////////////////////////////////

void ReactionDialog::reactions( Reactions c )
{
  _reactions = c;
}

