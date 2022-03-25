#ifndef MAPITEM_H
#define MAPITEM_H

#include <QPointF>

#include "treeitem.h"

class MapObj;
class LinkableMapObj;

/*! /brief MapItem is used to store information of MapObj and inherited
   classes.

    This is done even while no QGraphicsView is availabe. This is useful
    if e.g. on a small device like a cellphone the full map is not used,
    but just a treeview instead.
*/

class MapItem : public TreeItem {
  protected:
    QPointF pos;

  public:
    MapItem(TreeItem *parent = nullptr);

    void init();

    /*! Overloaded from TreeItem. Used to set parObj in LinkableMapObj */
    virtual void appendChild(TreeItem *item);

    /*! Used to save relative position while map is not in QGraphicsView */
    virtual void setRelPos(const QPointF &);

    /*! Used to save absolute position while map is not in QGraphicsView */
    virtual void setAbsPos(const QPointF &);

  protected:
    bool hideLinkUnselected;

  public:
    /*! Hide link if item is not selected */
    virtual void setHideLinkUnselected(bool);

    /*! Check if link is hidden for unselected items */
    virtual bool getHideLinkUnselected();

    virtual QString getMapAttr(); //! Get attributes for saving as XML

    virtual QRectF getBBoxURLFlag(); //! get bbox of url flag
    virtual void setRotation(const qreal &a);

  protected:
    MapObj *mo;
    qreal angle;

  public:
    /*! Returns pointer to related LinkableMapObj in QGraphicsView */
    virtual MapObj *getMO();
    virtual LinkableMapObj *getLMO();

    /*! Return path to highlight currently selected items in scene coordinates */
    virtual QPainterPath getSelectionPath();

    /*! Return position to edit headings in scene coordinates */
    virtual QPointF getEditPosition();
};

#endif
