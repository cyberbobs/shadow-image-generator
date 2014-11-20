// Local
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "ShadowSettings.h"

#include "GraphicsRoundedRectItem.h"

// Qt
#include <QGraphicsRectItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QMenu>
#include <QInputDialog>
#include <QDebug>


MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_settings(new QSettings)
  , m_width(40)
  , m_height(40)
  , m_radius(0)
  , m_scale(1)
{
  ui->setupUi(this);

  m_scene = new QGraphicsScene(this);

  m_baseItem = new GraphicsRoundedRectItem();
  m_baseItem->setBrush(QBrush(Qt::white));
  m_baseItem->setPen(QPen(Qt::NoBrush, 0, Qt::NoPen));
  m_baseItem->setZValue(1000);

  m_scene->addItem(m_baseItem);
  ui->graphicsView->setScene(m_scene);
  addShadow();

  connect(ui->scaleSlider, SIGNAL(valueChanged(int)), SLOT(userScaleChanged(int)));

  connect(ui->widthBox, SIGNAL(valueChanged(int)), SLOT(userSourceChanged()));
  connect(ui->heightBox, SIGNAL(valueChanged(int)), SLOT(userSourceChanged()));
  connect(ui->radiusBox, SIGNAL(valueChanged(int)), SLOT(userSourceChanged()));

  connect(ui->addShadowButton, SIGNAL(clicked()), SLOT(addShadow()));

  connect(this, SIGNAL(sourceOptionsChanged(int,int,int)), SLOT(updateSource()));
  connect(this, SIGNAL(scaleChanged(qreal)), SLOT(updateSource()));

  // Hack: position view to the center of layout
  //
  // This is the only way I've found to ensure the item will be placed in the center of view: we simply make the view
  // rectangle definitely larger than the view itself, turn off the scroll bars display (in the UI file) and forcing
  // positioning on the center of the base item. If the scene rect is smaller than the view itself, QGraphicsView
  // control simply ignores QGraphicsView::centerOn calls
  ui->graphicsView->setSceneRect(-1000, -1000, 2000, 2000);

  updateSource();
  updatePresets();
}


MainWindow::~MainWindow()
{
  delete ui;
}


void MainWindow::on_saveButton_clicked()
{
  // File name
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save rendered image as..."), QString(),
                                                  "PNG image (*.png)");
  if (fileName.isEmpty())
    return;
  if (!fileName.endsWith(".png", Qt::CaseInsensitive))
    fileName.append(".png");

  // Source scene rect to render
  // We must use the aligned rectangle to not mess with the non-integer items coordinates
  QRectF sourceRect = QRectF(m_scene->itemsBoundingRect().toAlignedRect());

  QImage target(sourceRect.width(), sourceRect.height(), QImage::Format_ARGB32_Premultiplied);
  target.fill(Qt::transparent);

  QPainter p(&target);
  p.setRenderHint(QPainter::Antialiasing);
  p.setRenderHint(QPainter::SmoothPixmapTransform);

  if (!ui->renderSourceCheckBox->isChecked())
    m_baseItem->setVisible(false);

  m_scene->render(&p, target.rect(), sourceRect);

  m_baseItem->setVisible(true);
  p.end();

  // Determine the image rectangle filled by shadow and crop borders
  QRect filled = filledRect(target);
  target = target.copy(filled);

  // Save file
  bool saveResult = target.save(fileName);
  if (!saveResult)
  {
    QMessageBox::critical(this, tr("Error saving file"), tr("Could not save file %1").arg(fileName));
    return;
  }

  // Generate QML
  if (ui->generateQmlCheckBox->isChecked())
  {
    QRectF baseItemRect = m_baseItem->boundingRect().translated(-sourceRect.topLeft()).translated(-filled.topLeft());

    QFileInfo fileInfo(fileName);
    QFile qmlFile(fileInfo.absolutePath() + "/" + fileInfo.completeBaseName() + ".qml");
    if (!qmlFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      QMessageBox::critical(this, tr("Error saving file"), tr("Could not save file %1").arg(qmlFile.fileName()));
      return;
    }

    QTextStream stream(&qmlFile);

    stream << "import QtQuick 2.0" << endl;
    stream << endl;
    stream << "Item {" << endl;

    stream << "  BorderImage {" << endl;
    stream << "    source: \"" << fileInfo.fileName() << "\"" << endl;
    stream << endl;
    stream << "    anchors {" << endl;
    stream << "      fill: parent" << endl;
    stream << "      leftMargin: -border.left; topMargin: -border.top" << endl;
    stream << "      rightMargin: -border.right; bottomMargin: -border.bottom" << endl;
    stream << "    }" << endl;
    stream << endl;
    stream << "    border.left: " << baseItemRect.left() << "; border.top: " << baseItemRect.top() << endl;
    stream << "    border.right: " << target.width() - baseItemRect.right()
           <<   "; border.bottom: " << target.height() - baseItemRect.bottom() << endl;
    stream << "  }" << endl;

    stream << "}" << endl;

    qmlFile.close();
  }
}


void MainWindow::updateSource()
{
  m_baseItem->setRect(0, 0, m_width * m_scale, m_height * m_scale);
  m_baseItem->setRadiusX(m_radius * m_scale);
  m_baseItem->setRadiusY(m_radius * m_scale);

  ui->graphicsView->centerOn(m_baseItem);
}


void MainWindow::addShadow(int xOffset, int yOffset, int opacity, double blur)
{
  QBoxLayout* shadowLayout = static_cast<QBoxLayout*>(ui->shadowsBox->layout());

  ShadowSettings* settings = new ShadowSettings(m_scene);
  settings->setSourceOptions(m_width, m_height, m_radius);
  settings->setScale(m_scale);

  connect(this, SIGNAL(sourceOptionsChanged(int,int,int)), settings, SLOT(setSourceOptions(int,int,int)));
  connect(this, SIGNAL(scaleChanged(qreal)), settings, SLOT(setScale(qreal)));
  connect(settings, &QObject::destroyed, [this](QObject* obj) {
            this->m_shadows.removeOne(static_cast<ShadowSettings*>(obj));
          });

  settings->setXOffset(xOffset);
  settings->setYOffset(yOffset);
  settings->setOpacity(opacity);
  settings->setBlur(blur);

  shadowLayout->insertWidget(shadowLayout->count() - 1, settings);

  m_shadows.append(settings);
  ui->graphicsView->centerOn(m_baseItem);
}


void MainWindow::userSourceChanged()
{
  m_width = ui->widthBox->value();
  m_height = ui->heightBox->value();
  m_radius = ui->radiusBox->value();

  ui->radiusBox->setMaximum(qMin(m_width, m_height) / 2);

  emit sourceOptionsChanged(m_width, m_height, m_radius);
}


void MainWindow::userScaleChanged(int value)
{
  // Calculate and display scale value
  m_scale = qreal(value) * 0.25;
  ui->scaleValueLabel->setText(tr("%1%").arg(m_scale * 100));

  emit scaleChanged(m_scale);
}


void MainWindow::loadShadowPreset()
{
  QAction* a = qobject_cast<QAction*>(sender());
  if (!a)
  {
    qWarning("Unknown action source");
    return;
  }

  QVariantList o = a->data().toList();
  if (o.size() % 4)
  {
    qWarning() << "Incorrect preset data for preset" << a->text();
    return;
  }

  qDeleteAll(m_shadows);
  m_shadows.clear();

  for (int i = 0; i < o.size() / 4; ++i)
    addShadow(o.at(i * 4).toInt(), o.at(i * 4 + 1).toInt(), o.at(i * 4 + 2).toInt(), o.at(i * 4 + 3).toDouble());
}


QRect MainWindow::filledRect(const QImage& image)
{
  QRect result = image.rect();

  // Left border
  while (result.left() < result.right())
  {
    bool hasFilledPixel = false;
    for (int y = result.top(); y <= result.bottom(); y++)
    {
      if (qAlpha(image.pixel(result.left(), y)) > 0)
      {
        hasFilledPixel = true;
        break;
      }
    }

    if (hasFilledPixel)
      break;

    result.adjust(1, 0, 0, 0);
  }

  // Right border
  while (result.right() > result.left())
  {
    bool hasFilledPixel = false;
    for (int y = result.top(); y <= result.bottom(); y++)
    {
      if (qAlpha(image.pixel(result.right(), y)) > 0)
      {
        hasFilledPixel = true;
        break;
      }
    }

    if (hasFilledPixel)
      break;

    result.adjust(0, 0, -1, 0);
  }

  // Top border
  while (result.top() < result.bottom())
  {
    bool hasFilledPixel = false;
    for (int x = result.left(); x <= result.right(); x++)
    {
      if (qAlpha(image.pixel(x, result.top())) > 0)
      {
        hasFilledPixel = true;
        break;
      }
    }

    if (hasFilledPixel)
      break;

    result.adjust(0, 1, 0, 0);
  }

  // Top border
  while (result.bottom() > result.top())
  {
    bool hasFilledPixel = false;
    for (int x = result.left(); x <= result.right(); x++)
    {
      if (qAlpha(image.pixel(x, result.bottom())) > 0)
      {
        hasFilledPixel = true;
        break;
      }
    }

    if (hasFilledPixel)
      break;

    result.adjust(0, 0, 0, -1);
  }

  return result;
}


void MainWindow::updatePresets()
{
  if (!ui->loadPresetButton->menu())
    ui->loadPresetButton->setMenu(new QMenu);

  QMenu* menu = ui->loadPresetButton->menu();
  menu->clear();

  // Default presets
  QVariantList presets;
  if (!m_settings->contains("ShadowPresets"))
  {
    QVariantMap d1;
    d1[QStringLiteral("name")] = tr("Material design Z=1");
    d1[QStringLiteral("options")] = QVariantList() << 0 << 1 << 12 << 1.5
                                                   << 0 << 1 << 24 << 1.;

    QVariantMap d2;
    d2[QStringLiteral("name")] = tr("Material design Z=2");
    d2[QStringLiteral("options")] = QVariantList() << 0 << 3 << 16 << 3.
                                                   << 0 << 3 << 23 << 3.;

    QVariantMap d3;
    d3[QStringLiteral("name")] = tr("Material design Z=3");
    d3[QStringLiteral("options")] = QVariantList() << 0 << 10 << 19 << 10.
                                                   << 0 << 6  << 23 << 3.;

    QVariantMap d4;
    d4[QStringLiteral("name")] = tr("Material design Z=4");
    d4[QStringLiteral("options")] = QVariantList() << 0 << 14 << 25 << 14.
                                                   << 0 << 10 << 22 << 5.;

    QVariantMap d5;
    d5[QStringLiteral("name")] = tr("Material design Z=5");
    d5[QStringLiteral("options")] = QVariantList() << 0 << 19 << 30 << 19.
                                                   << 0 << 15 << 22 << 6.;

    presets << d1 << d2 << d3 << d4 << d5;
    m_settings->setValue(QStringLiteral("ShadowPresets"), presets);
  }
  else
  {
    presets = m_settings->value(QStringLiteral("ShadowPresets")).toList();
  }

  // Fill in the menu
  for (auto p : presets)
  {
    if (!p.isValid())
    {
      qWarning() << "Failed to load preset";
      continue;
    }

    QVariantMap preset = p.toMap();
    auto action = menu->addAction(preset[QStringLiteral("name")].toString(), this, SLOT(loadShadowPreset()));
    action->setData(preset[QStringLiteral("options")]);
  }
}


void MainWindow::on_addPresetButton_clicked()
{
  QString name = QInputDialog::getText(this, tr("Choose preset name"), tr("Preset name:"));
  if (!name.isEmpty())
  {
    QVariantMap preset;
    preset[QStringLiteral("name")] = name;
    QVariantList options;
    for (auto s : m_shadows)
      options << s->xOffset() << s->yOffset() << s->opacity() << s->blur();

    preset[QStringLiteral("options")] = options;

    auto presets = m_settings->value(QStringLiteral("ShadowPresets")).toList();
    presets.append(preset);
    m_settings->setValue(QStringLiteral("ShadowPresets"), presets);
    updatePresets();
  }
}
