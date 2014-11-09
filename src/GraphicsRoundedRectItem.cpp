// Local
#include "GraphicsRoundedRectItem.h"

// Qt
#include <QPainter>


GraphicsRoundedRectItem::GraphicsRoundedRectItem(QGraphicsItem* parent)
  : QGraphicsRectItem(parent)
  , m_rx(0)
  , m_ry(0)
{}


GraphicsRoundedRectItem::GraphicsRoundedRectItem(const QRectF& rect, QGraphicsItem* parent)
  : QGraphicsRectItem(rect, parent)
  , m_rx(0)
  , m_ry(0)
{}


GraphicsRoundedRectItem::GraphicsRoundedRectItem(qreal x, qreal y, qreal w, qreal h, QGraphicsItem* parent)
  : QGraphicsRectItem(x, y, w, h, parent)
  , m_rx(0)
  , m_ry(0)
{}


qreal GraphicsRoundedRectItem::radiusX() const
{
  return m_rx;
}


void GraphicsRoundedRectItem::setRadiusX(qreal radiusX)
{
  m_rx = radiusX;
  update();
}


qreal GraphicsRoundedRectItem::radiusY() const
{
  return m_ry;
}


void GraphicsRoundedRectItem::setRadiusY(qreal radiusY)
{
  m_ry = radiusY;
  update();
}


void GraphicsRoundedRectItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  Q_UNUSED(option)
  Q_UNUSED(widget)

  painter->setBrush(brush());
  painter->setPen(pen());
  painter->drawRoundedRect(rect(), m_rx, m_ry);
}
