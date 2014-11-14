// Local
#include "ShadowSettings.h"
#include "ui_ShadowSettings.h"

#include "GraphicsRoundedRectItem.h"

// Qt
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsBlurEffect>
#include <QDebug>


ShadowSettings::ShadowSettings(QGraphicsScene* scene, QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::ShadowSettings)
  , m_scene(scene)
  , m_width(0)
  , m_height(0)
  , m_radius(0)
  , m_scale(1)
{
  ui->setupUi(this);

  m_item = new GraphicsRoundedRectItem;
  m_item->setBrush(QBrush(Qt::black));
  m_item->setPen(QPen(Qt::NoBrush, 0, Qt::NoPen));

  m_effect = new QGraphicsBlurEffect;
  m_effect->setBlurHints(QGraphicsBlurEffect::QualityHint);
  m_item->setGraphicsEffect(m_effect);

  m_fakeEffectItem = new QGraphicsRectItem;
  m_fakeEffectItem->setPen(QPen(Qt::NoBrush, 0, Qt::NoPen));
  m_fakeEffectItem->setOpacity(0);

  scene->addItem(m_item);
  scene->addItem(m_fakeEffectItem);

  connect(ui->xOffsetBox, SIGNAL(valueChanged(int)), SLOT(updateShadow()));
  connect(ui->yOffsetBox, SIGNAL(valueChanged(int)), SLOT(updateShadow()));
  connect(ui->blurBox, SIGNAL(valueChanged(double)), SLOT(updateShadow()));
  connect(ui->opacityBox, SIGNAL(valueChanged(int)), SLOT(updateShadow()));
}


ShadowSettings::~ShadowSettings()
{
  m_scene->removeItem(m_fakeEffectItem);
  m_scene->removeItem(m_item);
  delete m_fakeEffectItem;
  delete m_item;
  delete ui;
}


int ShadowSettings::xOffset() const
{
  return ui->xOffsetBox->value();
}


void ShadowSettings::setXOffset(int xOffset)
{
  ui->xOffsetBox->setValue(xOffset);
}


int ShadowSettings::yOffset() const
{
  return ui->yOffsetBox->value();
}


void ShadowSettings::setYOffset(int yOffset)
{
  ui->yOffsetBox->setValue(yOffset);
}


int ShadowSettings::opacity() const
{
  return ui->opacityBox->value();
}


void ShadowSettings::setOpacity(int opacity)
{
  ui->opacityBox->setValue(opacity);
}


double ShadowSettings::blur() const
{
  return ui->blurBox->value();
}


void ShadowSettings::setBlur(double blur)
{
  ui->blurBox->setValue(blur);
}


void ShadowSettings::setSourceOptions(int width, int height, int radius)
{
  m_width = width;
  m_height = height;
  m_radius = radius;

  updateShadow();
}


void ShadowSettings::setScale(qreal scale)
{
  m_scale = scale;

  updateShadow();
}


void ShadowSettings::updateShadow()
{
  m_item->setRect(0, 0, m_width * m_scale, m_height * m_scale);
  m_item->setRadiusX(m_radius * m_scale);
  m_item->setRadiusY(m_radius * m_scale);

  // Offset
  m_item->setX(ui->xOffsetBox->value() * m_scale);
  m_item->setY(ui->yOffsetBox->value() * m_scale);

  // Opacity
  // Since QGraphicsBlurEffect produces more opaque result than we need, we amplify it by the fixed coefficient
  m_item->setBrush(QBrush(QColor(0, 0, 0, qreal(255) * (qreal(ui->opacityBox->value()) / 100) * 1.414)));

  // Blur
  m_effect->setBlurRadius(ui->blurBox->value() * m_scale * 1.414 * 2);
  m_effect->update();

  // Fake item coordinates
  // QGraphicsScene::boundingRect function ignores the graphics effect bounding rectangle and doesn't include it in the
  // scene size. We're bypassing it by creating fake transparent item, filling the shadow effect rectangle.
  QRectF effectRect = m_effect->boundingRect();
  m_fakeEffectItem->setX(ui->xOffsetBox->value() * m_scale - (effectRect.width() - m_width * m_scale) / 2);
  m_fakeEffectItem->setY(ui->yOffsetBox->value() * m_scale - (effectRect.height() - m_width * m_scale) / 2);
  m_fakeEffectItem->setRect(QRectF(0, 0, effectRect.width(), effectRect.height()));
}


void ShadowSettings::on_removeButton_clicked()
{
  deleteLater();
}
