#include "prop_line_edit.hpp"
#include <QHBoxLayout>
#include <QPainter>
#define PROP_LINE_MIN_HEIGHT  28
namespace ReportDesign
{
prop_line_edit::prop_line_edit(QString name, QWidget *parent, bool bodeFlag): QWidget(parent),
    m_name(new QLabel(name, this)), m_edit(new CustomLineEdit(this)), m_editLineFlag(false), m_bodeFlag(bodeFlag)
{

    setMinimumHeight(PROP_LINE_MIN_HEIGHT);
    // 居中显示
    m_edit->setAlignment(Qt::AlignCenter);

    QVBoxLayout *Vlayout = new QVBoxLayout(this);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(m_name);
    layout->addWidget(m_edit);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    Vlayout->addSpacing(0);
    Vlayout->addLayout(layout);
    Vlayout->setContentsMargins(0, 0, 0, 0);

    setContentsMargins(1, 1, 1, 1);
    connect(m_edit, &CustomLineEdit::editingFinished, [this]
    {
        emit  contentEditFinished(m_edit->text());
        m_editLineFlag = false;
        update();
    });

    connect(m_edit, &CustomLineEdit::beginEdit, [this]
    {
        m_editLineFlag = true;
        emit beginEdit();
        update();
    });
}

QString prop_line_edit::name()
{
    return m_name->text();
}

void prop_line_edit::setName(QString name)
{
    m_name->setText(name);
}

QString prop_line_edit::content()
{
    return m_edit->text();
}

void prop_line_edit::setContent(QString content)
{
    m_edit->setText(content);
}

void prop_line_edit::setMinimumHeight(int height)
{
    m_name->setMinimumHeight(height);
    m_edit->setMinimumHeight(height);
}

void prop_line_edit::setReadOnly(bool flag)
{
    m_edit->setReadOnly(flag);
}

void prop_line_edit::paintEditLine(QPaintEvent *event)
{

}

void prop_line_edit::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    if (m_editLineFlag && m_bodeFlag)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing); // 设置抗锯齿
        // 绘制带圆角的矩形边框
        painter.setPen(QPen(QColor(0x57, 0xbd, 0x7c), 2));
        painter.drawRoundedRect(rect(), 4, 4); // 绘制圆角矩形边框
    }
}

void prop_line_edit::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
}

}
