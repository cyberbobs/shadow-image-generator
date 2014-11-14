#ifndef SHADOWSETTINGS_H
#define SHADOWSETTINGS_H

// Qt
#include <QWidget>
class QGraphicsScene;
class QGraphicsRectItem;
class QGraphicsBlurEffect;

// Ui
namespace Ui
{
  class ShadowSettings;
}

// Local
class GraphicsRoundedRectItem;


class ShadowSettings : public QWidget
{
  Q_OBJECT

  public:
    explicit ShadowSettings(QGraphicsScene* scene, QWidget* parent = 0);
    ~ShadowSettings();

    int xOffset() const;
    void setXOffset(int);

    int yOffset() const;
    void setYOffset(int);

    int opacity() const;
    void setOpacity(int);

    double blur() const;
    void setBlur(double);

  public slots:
    void setSourceOptions(int width, int height, int radius);
    void setScale(qreal scale);

  private slots:
    void updateShadow();

    void on_removeButton_clicked();

  private:
    Ui::ShadowSettings* ui;

    QGraphicsScene* m_scene;
    GraphicsRoundedRectItem* m_item;
    QGraphicsBlurEffect* m_effect;
    QGraphicsRectItem* m_fakeEffectItem;

    int m_width;
    int m_height;
    int m_radius;
    qreal m_scale;
};

#endif // SHADOWSETTINGS_H
