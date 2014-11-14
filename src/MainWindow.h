#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Qt
#include <QMainWindow>
class QGraphicsScene;
class QSettings;

// Ui
namespace Ui
{
  class MainWindow;
}

// Local
class GraphicsRoundedRectItem;
class ShadowSettings;


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
    void addShadow(int xOffset = 5, int yOffset = 5, int opacity = 50, double blur = 10.);

    void userSourceChanged();
    void userScaleChanged(int value);

    void loadShadowPreset();

    void on_addPresetButton_clicked();

  private:
    Ui::MainWindow* ui;
    QGraphicsScene* m_scene;

    QSettings* m_settings;

    GraphicsRoundedRectItem* m_baseItem;
    QList<ShadowSettings*> m_shadows;

    int m_width;
    int m_height;
    int m_radius;
    qreal m_scale;

    static QRect filledRect(const QImage& image);
    void updatePresets();
};

#endif // MAINWINDOW_H
