#include "base_push_button.hpp"
#include <QPixmap>
#include <QBitmap>
#include <QCoreApplication>
namespace ReportDesign
{
base_push_button::base_push_button(QWidget *p): QPushButton(p)
{
    resize(110, 44);
}

gbase_push_button::gbase_push_button(QWidget *p): QPushButton(p)
{
    resize(110, 44);
}


base_Label::base_Label(QWidget *p /*= nullptr*/): QLabel(p)
{
}

base_Label::base_Label(QString text,  QWidget *p /*= nullptr*/): QLabel(text, p)
{
    setText(text);
}

void base_Label::setText(QString text)
{
    setToolTip(text);
    m_text = text;
    int labelWidth = width(); // 获取QLabel控件的宽度
    QFontMetrics fontMetrics(font()); // 使用QLabel的字体创建QFontMetrics对象
    int textWidth = fontMetrics.width(m_text); // 计算文本的宽度
    if (textWidth > labelWidth)
    {
        QString ellipsizedText = fontMetrics.elidedText(m_text, Qt::ElideRight, labelWidth); // 使用省略号替代超出部分
        QLabel::setText(ellipsizedText); // 更新QLabel的文本内容
    }
    else
    {
        QLabel::setText(m_text); // 更新QLabel的文本内容
    }
}

QString base_Label::text()
{
    return m_text;
}

image_button::image_button(QWidget *parent, QString Image, QString hoverUrl, QString Checked):
    QPushButton(parent)
{
    if (!Image.isEmpty())
    {
        SetImage(Image, hoverUrl, Checked);
    }
}

void image_button::SetImage(QString Image, QString hoverUrl, QString Checked)
{
    m_imageUrl = Image;
    m_hoverUrl = hoverUrl;
    m_cheakUrl =  Checked;
    if (hoverUrl.isEmpty())
    {
        hoverUrl = Image;
    }

    int Checkedfalg = 1;
    if (Checked.isEmpty())
    {
        Checked = Image;
        Checkedfalg = 0;
    }

    m_pixmap = QSharedPointer<QPixmap>(new QPixmap(Image));
    setFixedSize(m_pixmap->size());
    if (Checkedfalg)
    {
        setCheckable(true);
        m_StyleSheet = QString("QPushButton { border-radius: 50%; background-image: url(%1); background-color: transparent; } "
                               "QPushButton:checked  { border-radius: 50%; background-image: url(%2); background-color: transparent; } "
                              ).arg(Image).arg(Checked);
    }
    else
    {
        m_StyleSheet = QString("QPushButton { border-radius: 50%; background-image: url(%1); background-color: transparent; } "
                               "QPushButton:hover { border-radius: 50%; background-image: url(%3); background-color: transparent; }").arg(Image).arg(hoverUrl);

    }
    setStyleSheet(m_StyleSheet);
}

CircularButton::CircularButton(QWidget *parent, QString Image) : QPushButton(parent)
{
    if (!Image.isEmpty())
    {
        SetImage(Image);
    }
}

void CircularButton::paintEvent(QPaintEvent *event)
{
    //QPushButton::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255)); // 设置为白色背景

    painter.drawEllipse(rect());
    QRectF pixmapRect = m_pixmap->rect();
    pixmapRect.setTopLeft(QPointF(width() / 2 - pixmapRect.width() / 2, height() / 2 - pixmapRect.height() / 2));
    painter.drawPixmap(pixmapRect.x(), pixmapRect.y(), *m_pixmap);
}

void CircularButton::SetImage(QString Image)
{
    m_imageUrl = Image;
    m_pixmap =  QSharedPointer<QPixmap>(new QPixmap(Image));
    int max = m_pixmap->size().width() > m_pixmap->size().height() ? m_pixmap->size().width() : m_pixmap->size().height();
    if (max > width() || max > height())
    {
        resize(50, 50);
    }
}

switchButton::switchButton(QWidget *p/* = nullptr*/): image_button(p, ":/turnOff"),
    switchFlag(false)
{
    connect(this, &QPushButton::clicked, [this]
    {
        if (switchFlag)
        {
            setTurnOff();
        }
        else
        {
            setTurnOn();
        }
    });
}

void switchButton::setTurnOn()
{
    switchFlag = true;
    SetImage(":/turnOn");
    emit turnOn();
}

void switchButton::setTurnOff()
{
    switchFlag = false;
    SetImage(":/turnOff");
    emit turnOff();
}

bool switchButton::isTurnOn()
{
    return switchFlag;
}

module_file_button::module_file_button(QString ItemName, QWidget *parent): QWidget(parent),
    m_image(new QLabel(this)), m_fileName(new base_Label(this)), m_VLayout(new QVBoxLayout(this)),
    m_boldFlag(true)
{
    init();
    setFileName(ItemName);
}

void module_file_button::init()
{
    QPalette palette;
    //设置主窗口背景颜色
    palette.setColor(QPalette::Window, QColor(255, 255, 255));
    setPalette(palette);
    resize(140, 160);
    m_fileName->setWordWrap(true);
    m_VLayout->setContentsMargins(2, 2, 2, 2);
    m_VLayout->addWidget(m_image, 0, Qt::AlignHCenter);
    m_VLayout->addWidget(m_fileName, 0, Qt::AlignHCenter);
    setLayout(m_VLayout);
}

void module_file_button::showBorder(bool flag)
{
    m_boldFlag = flag;
    update();
}

void module_file_button::setImage(QString url)
{
    QPixmap  pixmap(url);
    m_image->resize(pixmap.size());
    m_image->setPixmap(pixmap);
}

void module_file_button::setFileName(QString fileName)
{
    m_fileNameStr = fileName;
    m_fileName->setText(m_fileNameStr);
}

QString module_file_button::filePath()
{
    return QString("%1/%2").arg(QCoreApplication::applicationDirPath() + DEFAULT_MODULE_FILE_PATH).arg(m_fileNameStr);
}

void module_file_button::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    if (m_boldFlag)
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing); // 抗锯齿
        painter.setPen(QPen(QColor(0xa8, 0xb2, 0xc2), 1)); // 设置边框颜色和宽度
        painter.setBrush(Qt::white); // 设置填充颜色

        int borderRadius = 4; // 圆角半径
        int borderWidth = 1; // 边框宽度
        QRect rect = contentsRect().adjusted(borderWidth, borderWidth, -borderWidth, -borderWidth); // 考虑边框宽度后的绘制区域
        painter.drawRoundedRect(rect, borderRadius, borderRadius);
    }
}

void module_file_button::focusInEvent(QFocusEvent *event)
{
    // 处理选中状态的逻辑
    // ...
    QWidget::focusInEvent(event);

}

void module_file_button::focusOutEvent(QFocusEvent *event)
{
    // 处理未选中状态的逻辑
    // ...
    QWidget::focusOutEvent(event);
}


Item_button::Item_button(QString ItemName, QWidget *parent): QLabel(parent),
    m_image(new QLabel(this)), m_itemName(new base_Label(this)), m_VLayout(new QVBoxLayout(this))
{
    init();
    setItemName(ItemName);
}

void Item_button::init()
{
    QPalette palette;
    //设置主窗口背景颜色
    palette.setColor(QPalette::Window, QColor(255, 255, 255));
    setPalette(palette);
    setFixedSize(100, 80);

    m_itemName->resize(width(), 25);
    m_itemName->setWordWrap(true);
    m_itemName->setAlignment(Qt::AlignHCenter);
    m_itemName->setObjectName("itemName");
    m_image->setObjectName("image");
    m_VLayout->setContentsMargins(2, 10, 2, 16);
    m_VLayout->setSpacing(5);
    m_VLayout->addWidget(m_image, 0, Qt::AlignHCenter);
    m_VLayout->addSpacing(10);
    m_VLayout->addWidget(m_itemName, 0, Qt::AlignHCenter);
}

void Item_button::setImage(QString url)
{
    QPixmap  pixmap(url);
    m_image->resize(pixmap.size());
    m_image->setPixmap(pixmap);
    m_imageUrl = url;
}

void Item_button::setClickGray(bool status)
{
    if (status)
    {
        m_mask =  new QLabel(this);
        m_mask->setObjectName("mask");
        m_mask->setFixedSize(size());
        m_mask->show();
    }
    else
    {
        delete m_mask;
    }
}

QImage Item_button::dropImage()
{
    // 获取按钮的大小和位置
    QRect rect = this->geometry();

    // 创建一个QPixmap对象，大小与按钮相同
    QPixmap pixmap(rect.width(), rect.height());

    // 将QPixmap对象填充为透明
    pixmap.fill(Qt::transparent);

    // 创建一个QPainter对象，用于在QPixmap上绘制图形
    QPainter painter(&pixmap);

    // 将按钮绘制到QPixmap上
    this->render(&painter);

    // 将QPixmap转换为QImage对象
    QImage image = pixmap.toImage();

    // 将QImage对象设置为半透明
    for (int y = 0; y < image.height(); y++)
    {
        for (int x = 0; x < image.width(); x++)
        {
            QRgb pixel = image.pixel(x, y);
            int alpha = qAlpha(pixel);
            alpha /= 1.5;
            pixel = qRgba(qRed(pixel), qGreen(pixel), qBlue(pixel), alpha);
            image.setPixel(x, y, pixel);
        }
    }

    // 返回半透明的QImage对象
    return image;
}

void Item_button::setItemName(QString ItemName)
{
    m_itemName->setText(ItemName);
    setToolTip(ItemName);
}

void Item_button::mousePressEvent(QMouseEvent *event)
{
    emit pressed();
    QLabel::mousePressEvent(event);
}

}
