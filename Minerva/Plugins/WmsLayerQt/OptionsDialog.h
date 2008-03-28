
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WMS_OPTIONS_DIALOG_H__
#define __WMS_OPTIONS_DIALOG_H__

#include "Minerva/Plugins/WmsLayerQt/CompileGuard.h"
#include "ui_OptionsDialog.h"

#include "QtGui/QDialog"
#include "QtGui/QItemDelegate"

#include <map>
#include <string>

class OptionsDialog : public QDialog,
                      private Ui::OptionsDialog
{
  Q_OBJECT;
public:
  typedef QDialog BaseClass;
  typedef std::map<std::string, std::string> Options;
  
  OptionsDialog ( const Options& options, QWidget *parent = 0x0 );
  virtual ~OptionsDialog();
  
  Options options() const;
  
protected slots:
  
  void on_addRowButton_clicked();
  void on_removeRowButton_clicked();
  
private:
  
  // Custom item model.
  class OptionsItemModel : public QAbstractItemModel
  {
  public:
    typedef QAbstractItemModel BaseClass;
    typedef std::pair<std::string,std::string> Value;
    typedef std::vector<Value> Values;
    
    OptionsItemModel ( const Options& alphas, QObject *parent = 0x0 );
    virtual ~OptionsItemModel();
    
    Values values() const { return _values; }
    
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
    Values _values;
  };
  
  // Custom item delegate.
  class OptionsItemDelegate : public QItemDelegate
  {
  public:
    typedef QItemDelegate BaseClass;
    
    OptionsItemDelegate( QObject *parent = 0x0 );
    virtual ~OptionsItemDelegate();
    
    virtual QWidget *createEditor  ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    virtual void     setEditorData ( QWidget *editor, const QModelIndex &index ) const;
    virtual void     setModelData  ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const;
    virtual void     paint ( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  };
  
  OptionsItemModel *_model;
  QAbstractItemDelegate *_delegate;
};

#endif // __WMS_OPTIONS_DIALOG_H__
