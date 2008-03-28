
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/WmsLayerQt/OptionsDialog.h"

#include "QtGui/QLineEdit"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OptionsDialog::OptionsDialog ( const Options& options, QWidget* parent ) : BaseClass ( parent ),
  _model ( new OptionsItemModel ( options ) ),
  _delegate ( new OptionsItemDelegate )
{
  this->setupUi( this );
  
  _optionsTreeWidget->setModel ( _model );
  _optionsTreeWidget->setItemDelegate ( _delegate );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

OptionsDialog::~OptionsDialog()
{
  delete _model;
  delete _delegate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the options.
//
///////////////////////////////////////////////////////////////////////////////

OptionsDialog::Options OptionsDialog::options() const
{
  Options options;
  
  if ( 0x0 != _model )
  {
    typedef OptionsItemModel::Values Values;
    Values values ( _model->values() );
  
    for ( Values::const_iterator iter = values.begin(); iter != values.end(); ++iter )
    {
      options.insert ( *iter );
    }
    
  }
  
  return options;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a row.
//
///////////////////////////////////////////////////////////////////////////////

void OptionsDialog::on_addRowButton_clicked()
{
  QModelIndex index;
  _model->insertRow ( _model->rowCount(), index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a row.
//
///////////////////////////////////////////////////////////////////////////////

void OptionsDialog::on_removeRowButton_clicked()
{
  QModelIndex index ( _optionsTreeWidget->selectionModel()->currentIndex() );
  _model->removeRow ( index.row(), index.parent() );
}



///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OptionsDialog::OptionsItemModel::OptionsItemModel ( const Options& options, QObject *parent ) : BaseClass ( parent ),
  _values()
{
  for ( Options::const_iterator iter = options.begin(); iter != options.end(); ++iter )
  {
    _values.push_back ( *iter );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

OptionsDialog::OptionsItemModel::~OptionsItemModel()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the row count.
//
///////////////////////////////////////////////////////////////////////////////

int OptionsDialog::OptionsItemModel::rowCount ( const QModelIndex &parent ) const
{
  return _values.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the column count.
//
///////////////////////////////////////////////////////////////////////////////

int OptionsDialog::OptionsItemModel::columnCount ( const QModelIndex &parent ) const
{
  return 2;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data.
//
///////////////////////////////////////////////////////////////////////////////

QVariant OptionsDialog::OptionsItemModel::data ( const QModelIndex& index, int role ) const
{
  if ( false == index.isValid() )
    return QVariant();
  
  if ( Qt::TextAlignmentRole == role )
    return static_cast<int> ( Qt::AlignRight | Qt::AlignVCenter );
  if ( Qt::DisplayRole == role )
  {
    if ( 0 == index.column() )
      return QString ( _values.at ( index.row() ).first.c_str() );
    if ( 1 == index.column() )
      return QString ( _values.at ( index.row() ).second.c_str() );
  }
  
  return QVariant();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool OptionsDialog::OptionsItemModel::setData ( const QModelIndex& index, const QVariant& value, int role )
{
  if ( index.isValid() && role == Qt::EditRole )
  {
    // Color.
    if ( 0 == index.column() )
    {
			std::string s ( value.toString().toStdString() );
      _values[index.row()].first = s;
      
			emit dataChanged ( index, index );
      
      return true;
    }
    
    // Value.
    else if ( 1 == index.column() )
    {
      std::string s ( value.toString().toStdString() );
      _values[index.row()].second = s;
      
      emit dataChanged ( index, index );
      
      return true;
    }
  }
  
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the header data.
//
///////////////////////////////////////////////////////////////////////////////

QVariant OptionsDialog::OptionsItemModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
  if ( Qt::DisplayRole == role )
  {
    if ( 0 == section )
      return "Key";
    if ( 1 == section )
      return "Value";
  }
  
  return QVariant();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the index.
//
///////////////////////////////////////////////////////////////////////////////

QModelIndex OptionsDialog::OptionsItemModel::index ( int row, int column, const QModelIndex & parent ) const
{
  return this->hasIndex ( row, column, parent ) ? this->createIndex ( row, column, 0 ) : QModelIndex();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the parent.
//
///////////////////////////////////////////////////////////////////////////////

QModelIndex OptionsDialog::OptionsItemModel::parent ( const QModelIndex & index ) const
{
  return QModelIndex();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the ModelIndex have any children.
//
///////////////////////////////////////////////////////////////////////////////

bool OptionsDialog::OptionsItemModel::hasChildren ( const QModelIndex & parent ) const
{
  return parent.isValid() ? false : ( this->rowCount() > 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the flags.
//
///////////////////////////////////////////////////////////////////////////////

Qt::ItemFlags OptionsDialog::OptionsItemModel::flags ( const QModelIndex &index ) const
{
  Qt::ItemFlags flags ( BaseClass::flags ( index ) );
  
  if ( index.isValid() )
    flags |= Qt::ItemIsEditable;
  
  return flags;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the row.
//
///////////////////////////////////////////////////////////////////////////////

bool OptionsDialog::OptionsItemModel::removeRow ( int row, const QModelIndex & parent )
{
  if ( row < static_cast<int> ( _values.size() ) )
  {
    this->beginRemoveRows ( parent, row, row );
    _values.erase ( _values.begin() + row );
    this->endRemoveRows();
    
    return true;
  }
  
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert a row.
//
///////////////////////////////////////////////////////////////////////////////

bool OptionsDialog::OptionsItemModel::insertRow ( int row, const QModelIndex & parent )
{
  if ( row <= static_cast<int> ( _values.size() ) )
  {
    this->beginInsertRows ( parent, row, row );
    _values.insert ( _values.begin() + row, Value ( "key", "value" ) );
    this->endInsertRows();
    
    return true;
  }
  
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

OptionsDialog::OptionsItemDelegate::OptionsItemDelegate( QObject *parent ) : BaseClass ( parent )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

OptionsDialog::OptionsItemDelegate::~OptionsItemDelegate()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create an editor.
//
///////////////////////////////////////////////////////////////////////////////

QWidget * OptionsDialog::OptionsItemDelegate::createEditor ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
  if ( 0 == index.column() || 1 == index.column() )
  {
		return new QLineEdit ( parent );
  }
  
  return BaseClass::createEditor ( parent, option, index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state of the editor.
//
///////////////////////////////////////////////////////////////////////////////

void OptionsDialog::OptionsItemDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const
{
  if ( 0 == index.column() || 1 == index.column() )
  {
    QString text ( index.model()->data ( index, Qt::DisplayRole ).toString() );
		QLineEdit* lineEdit ( static_cast<QLineEdit*> ( editor ) );
		lineEdit->setText ( text );
  }
  else
    BaseClass::setEditorData ( editor, index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state of the model.
//
///////////////////////////////////////////////////////////////////////////////

void OptionsDialog::OptionsItemDelegate::setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
	if ( 0 == index.column() || 1 == index.column() )
	{
		QLineEdit* lineEdit ( static_cast<QLineEdit*> ( editor ) );
		model->setData ( index, lineEdit->text(), Qt::EditRole );
	}
	else
		BaseClass::setModelData ( editor, model, index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Paint.
//
///////////////////////////////////////////////////////////////////////////////

void OptionsDialog::OptionsItemDelegate::paint ( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
  BaseClass::paint ( painter, option, index );
}
