// Local
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "ShadowSettings.h"

#include "GraphicsRoundedRectItem.h"

// Qt
#include <QGraphicsRectItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>


MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
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
  updateSource();

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
    QRectF baseItemRect = m_baseItem->boundingRect().translated(-sourceRect.topLeft());
    qDebug() << "Base item rectangle:" << baseItemRect;

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
    stream << "    border.right: " << sourceRect.width() - baseItemRect.right()
           <<   "; border.bottom: " << sourceRect.height() - baseItemRect.bottom() << endl;
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
}


void MainWindow::addShadow()
{
  QBoxLayout* shadowLayout = static_cast<QBoxLayout*>(ui->shadowsBox->layout());

  ShadowSettings* settings = new ShadowSettings(m_scene);
  settings->setSourceOptions(m_width, m_height, m_radius);
  settings->setScale(m_scale);

  connect(this, SIGNAL(sourceOptionsChanged(int,int,int)), settings, SLOT(setSourceOptions(int,int,int)));
  connect(this, SIGNAL(scaleChanged(qreal)), settings, SLOT(setScale(qreal)));

  shadowLayout->insertWidget(shadowLayout->count() - 1, settings);
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
