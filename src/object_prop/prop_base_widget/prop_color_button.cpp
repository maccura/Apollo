#include "prop_color_button.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#define PROP_COLOR_BUTTON_MIN_HEIGHT  28
namespace ReportDesign
{
prop_color_button::prop_color_button(QWidget *parent /*= nullptr*/): QWidget(parent),
    m_colorButton(new color_button(this)), m_colorLineEdit(new prop_line_edit("", this, false)),
    m_bodeFlag(false)
{
    setMinimumHeight(PROP_COLOR_BUTTON_MIN_HEIGHT);
    QHBoxLayout *layout = new QHBoxLayout(this);
    m_colorButton->setFixedSize(PROP_COLOR_BUTTON_MIN_HEIGHT - 6,
                                PROP_COLOR_BUTTON_MIN_HEIGHT - 6);
    layout->addSpacing(10);
    layout->addWidget(m_colorButton);
    layout->addWidget(m_colorLineEdit);
    layout->setContentsMargins(0, 0, 0, 0);
    setContentsMargins(0, 0, 0, 0);

    connect(m_colorLineEdit, SIGNAL(contentEditFinished(QString)), this, SLOT(slotEditFinish()));
    connect(m_colorButton, SIGNAL(clicked(bool)), this, SLOT(slotColorButtonClicked()));
    connect(m_colorLineEdit, &prop_line_edit::beginEdit, [this]
    {
        m_bodeFlag = true;
        update();
    });
}

void prop_color_button::setMinimumHeight(int minHeight)
{
    m_colorLineEdit->setMinimumHeight(minHeight);
    m_colorButton->setMinimumHeight(minHeight);
    QWidget::setMinimumHeight(minHeight);
}

void prop_color_button::slotEditFinish()
{
    QString input = m_colorLineEdit->content(); // 获取输入框中的文本
    QRegExp regExp("^([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})([0-9A-Fa-f]{2})$"); // 定义正则表达式
    if (regExp.exactMatch(input))
    {
        // 判断输入是否符合正则表达式
        // 如果符合，将RGB值分别提取出来
        int r = regExp.cap(1).toInt(nullptr, 16);
        int g = regExp.cap(2).toInt(nullptr, 16);
        int b = regExp.cap(3).toInt(nullptr, 16);
        // 判断RGB值是否在0-255之间
        if (r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255)
        {
            QColor fontColor = QColor(r, g, b);
            m_colorButton->setColor(fontColor);
            emit colorChange(fontColor);
        }
    }

    m_bodeFlag = false;
    update();
}

void prop_color_button::slotColorButtonClicked()
{
    // Qt::white为默认颜色，this为父窗口，tr("选择颜色")为对话框标题
    QColor color = QColorDialog::getColor(Qt::black, nullptr, tr("color"));
    if (color.isValid())
    {
        // 判断用户是否选择了颜色
        // 用户选择了颜色，可以使用color对象获取用户选择的颜色
        if (m_colorButton->color() != color)
        {
            setColor(color);
            emit colorChange(color);
        }
    }
}

void prop_color_button::setColor(QColor color)
{
    m_colorButton->setColor(color);
    m_colorLineEdit->setContent(QString("%1%2%3").arg(color.red(), 2, 16, QLatin1Char('0')).
                                arg(color.green(), 2, 16, QLatin1Char('0')).arg(color.blue(), 2, 16, QLatin1Char('0')));
}

// 重写paintEvent函数，在按钮上绘制矩形
void prop_color_button::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(m_colorLineEdit->palette().color(QPalette::Background));
    painter.drawRoundedRect(rect(), 4, 4); // 绘制圆角矩形边框
    if (m_bodeFlag)
    {
        // 綠色边框
        painter.setRenderHint(QPainter::Antialiasing); // 设置抗锯齿
        // 绘制带圆角的矩形边框
        painter.setPen(QPen(QColor(0x57, 0xbd, 0x7c), 1));
        painter.drawRoundedRect(rect(), 4, 4); // 绘制圆角矩形边框
    }
    QWidget::paintEvent(event);
}
}
