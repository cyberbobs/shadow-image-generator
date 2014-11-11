#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt
#include <QMainWindow>
class QGraphicsScene;
class QGraphicsRectItem;

// Ui
namespace Ui
{
  class MainWindow;
}

// Local
class GraphicsRoundedRectItem;


class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow();

  signals:
    void sourceOptionsChanged(int width, int height, int radius);
    void scaleChanged(qreal scale);

  private slots:
    void on_saveButton_clicked();

    void updateSource();
    void addShadow();

    void userSourceChanged();
    void userScaleChanged(int value);

  private:
    Ui::MainWindow* ui;
    QGraphicsScene* m_scene;

    GraphicsRoundedRectItem* m_baseItem;

    int m_width;
    int m_height;
    int m_radius;
    qreal m_scale;

    QRect filledRect(const QImage& image) const;
};

#endif // MAINWINDOW_H
