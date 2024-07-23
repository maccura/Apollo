#include "prop_spinbox_widget.hpp"
#include <QHBoxLayout>
#include <QPainter>
#define PROP_SPINBOX_MIN_HEIGHT  28
namespace ReportDesign
{
prop_spinbox_widget::prop_spinbox_widget(QWidget *parent /*= nullptr*/, QString name): QWidget(parent),
    m_spinBox(new CustomSpinBox(this)), m_name(new QLabel(name, this)), m_editFlag(false)
{
    setMinimumHeight(PROP_SPINBOX_MIN_HEIGHT);
    m_spinBox->setAlignment(Qt::AlignCenter);
    QVBoxLayout *Vlayout = new QVBoxLayout(this);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(m_name);
    layout->addWidget(m_spinBox);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    Vlayout->addSpacing(0);
    Vlayout->addLayout(layout);
    Vlayout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(2, 2, 2, 2);

    connect(m_spinBox, SIGNAL(valueChanged(const QString &)), this, SIGNAL(valueChanged(const QString &)));
    connect(m_spinBox, &CustomSpinBox::editingFinished, [this]
    {
        m_editFlag = false;
        update();
    });

    connect(m_spinBox, &CustomSpinBox::beginEdit, [this]
    {
        m_editFlag = true;
        update();
    });
    setName(name);
}


void prop_spinbox_widget::setValue(int value)
{
    m_spinBox->setValue(value);
}

void prop_spinbox_widget::setName(QString name)
{
    m_name->setText(name);
    if (m_name->text().isEmpty())
    {
        m_name->hide();
    }
    else
    {
        m_name->show();
    }
}

void prop_spinbox_widget::setMinimumHeight(int height)
{
    m_spinBox->setMinimumHeight(height);
    m_name->setMinimumHeight(height);
}

void prop_spinbox_widget::setMinimumWidth(int width)
{
    QWidget::setMinimumWidth(width);
}

int prop_spinbox_widget::value()
{
    return m_spinBox->value();
}

void prop_spinbox_widget::setMaximum(int mun)
{
    m_spinBox->setMaximum(mun);
}

void prop_spinbox_widget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    if (m_editFlag)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing); // 设置抗锯齿
        // 绘制带圆角的矩形边框
        painter.setPen(QPen(QColor(0x57, 0xbd, 0x7c), 2));
        painter.drawRoundedRect(rect(), 4, 4); // 绘制圆角矩形边框
    }

}
}
