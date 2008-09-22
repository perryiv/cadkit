
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WMS_ALPHAS_DIALOG_H__
#define __WMS_ALPHAS_DIALOG_H__

#include "Minerva/Qt/Widgets/Export.h"

#include "ui_AlphasDialog.h"

#include "Usul/Interfaces/IRasterAlphas.h"

#include "QtGui/QDialog"

#include "QtCore/QAbstractItemModel"
#include "QtGui/QItemDelegate"


namespace Minerva {
namespace Widgets {
      
      
class MINERVA_QT_WIDGETS_EXPORT AlphasDialog : public QDialog,
                                               public Ui::AlphasDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef Usul::Interfaces::IRasterAlphas IRasterAlphas;
  typedef IRasterAlphas::Alphas Alphas;
  
  AlphasDialog ( const Alphas& alphas, QWidget *parent = 0x0 );
  virtual ~AlphasDialog();
  
  /// Get the alphas.
  Alphas alphas() const;
  
protected slots:
  
  void on_addRowButton_clicked();
  void on_removeRowButton_clicked();
  
private:
  
  // Custom item model.
  class AlphasItemModel : public QAbstractItemModel
  {
  public:
    typedef QAbstractItemModel BaseClass;
    typedef std::vector<QColor> Colors;
    typedef std::vector<unsigned short> Opacities;
    
    AlphasItemModel ( const Alphas& alphas, QObject *parent = 0x0 );
    virtual ~AlphasItemModel();
    
    Colors colors() const { return _colors; }
    Opacities opacities() const { return _opacities; }
    
    virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    
    virtual Qt::ItemFlags flags ( const QModelIndex &index ) const;
    
    virtual int rowCount ( const QModelIndex &parent = QModelIndex() ) const;
    virtual int columnCount ( const QModelIndex &parent = QModelIndex() ) const;
    
    virtual QVariant data ( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    virtual bool     setData ( const QModelIndex& index, const QVariant& value, int role );
    
    virtual bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    
    virtual QModelIndex parent ( const QModelIndex & index ) const;
    
    virtual bool insertRow ( int row, const QModelIndex & parent = QModelIndex() );
    virtual bool removeRow ( int row, const QModelIndex & parent = QModelIndex() );
    
  private:
    Colors _colors;
    Opacities _opacities;
  };
  
  // Custom item delegate.
  class AlphasItemDelegate : public QItemDelegate
  {
  public:
    typedef QItemDelegate BaseClass;
    
    AlphasItemDelegate( QObject *parent = 0x0 );
    virtual ~AlphasItemDelegate();
    
    virtual QWidget *createEditor  ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    virtual void     setEditorData ( QWidget *editor, const QModelIndex &index ) const;
    virtual void     setModelData  ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const;
    virtual void     paint ( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  };
  
  Alphas _alphas;
  AlphasItemModel *_model;
  AlphasItemDelegate *_delegate;
};

  
}
}

#endif // __WMS_ALPHAS_DIALOG_H__
