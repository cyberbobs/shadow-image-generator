#ifndef GRAPHICSROUNDEDRECTITEM_H
#define GRAPHICSROUNDEDRECTITEM_H

// Qt
#include <QGraphicsRectItem>


class GraphicsRoundedRectItem : public QGraphicsRectItem
{
  public:
    GraphicsRoundedRectItem(QGraphicsItem* parent = 0);
    GraphicsRoundedRectItem(const QRectF& rect, QGraphicsItem* parent = 0);
    GraphicsRoundedRectItem(qreal x, qreal y, qreal w, qreal h, QGraphicsItem* parent = 0);

    qreal radiusX() const;
    void setRadiusX(qreal radiusX);

    qreal radiusY() const;
    void setRadiusY(qreal radiusY);

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);

  private:
    qreal m_rx;
    qreal m_ry;
};

#endif // GRAPHICSROUNDEDRECTITEM_H
