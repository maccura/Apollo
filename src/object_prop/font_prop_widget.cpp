#include "font_prop_widget.hpp"
#include "property_widget_factory.hpp"
#include "item/base_design_item.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QAbstractItemView>
#include <QButtonGroup>
namespace
{
ReportDesign::base_prop_widget *createFontPropWidget(QObject *object, QList<QObject *> *objects, QWidget *parent)
{
    if (dynamic_cast<ReportDesign::base_design_item *>(object) == nullptr)
    {
        return nullptr;
    }
    return new ReportDesign::font_prop_widget(object, objects, parent);
}
bool registred = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(FONT_COLOR_PROPERTY), createFontPropWidget);
bool registred2 = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(FONT_PROPERTY), createFontPropWidget);
bool registred3 = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(ALIGNMENT_PROPERTY), createFontPropWidget);
}

namespace ReportDesign
{
font_prop_widget::font_prop_widget(QObject *object, QList<QObject *> *objects, QWidget *parent):
    base_prop_widget(object, objects, parent)
{
    init();
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QHBoxLayout *hlayout = new QHBoxLayout();
    vlayout->addWidget(new QLabel(tr("font"), this));
    hlayout->addWidget(m_familyCombox);
    hlayout->addSpacing(40);
    vlayout->addLayout(hlayout);

    hlayout = new QHBoxLayout();
    hlayout->addWidget(m_boldCombox);
    hlayout->addWidget(m_pointSizeSpinbox);
    hlayout->addSpacing(40);
    vlayout->addLayout(hlayout);

    hlayout = new QHBoxLayout();
    hlayout->addWidget(m_fontColorButton, 1);
    hlayout->addWidget(m_opacityCombox, 1);
    hlayout->addItem(new QSpacerItem(40, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));
    m_opacityCombox->setMinimumWidth(87);
    vlayout->addLayout(hlayout);

    hlayout = new QHBoxLayout();
    hlayout->addWidget(m_HAlignLeft);
    hlayout->addWidget(m_HAlignCenter);
    hlayout->addWidget(m_HAlignRight);
    hlayout->addWidget(m_VAlignTop);
    hlayout->addWidget(m_VAlignCenter);
    hlayout->addWidget(m_VAlignBottom);
    vlayout->addLayout(hlayout);
    vlayout->setContentsMargins(PROP_WIDGET_MARGINS);
}

void font_prop_widget::init()
{
    m_familyCombox = new QFontComboBox(this);
    m_familyCombox->setEditable(false);
    m_pointSizeSpinbox = new prop_spinbox_widget(this);
    m_boldCombox = new prop_combox(this);
    m_familyCombox->setMinimumHeight(m_boldCombox->minimumHeight());
    m_opacityCombox = new prop_combox(this);
    m_fontColorButton = new prop_color_button(this);

    // 字体对齐
    m_HAlignLeft = new image_Radio_button(this, ":/fontAlignToLeft", ":/fontAlignToLeft", ":/fontAlignToLeft_c");
    m_HAlignCenter = new image_Radio_button(this, ":/fontAlignToHCenter", ":/fontAlignToHCenter", ":/fontAlignToHCenter_c");
    m_HAlignRight = new image_Radio_button(this, ":/fontAlignToRight", ":/fontAlignToRight", ":/fontAlignToRight_c");
    m_VAlignTop = new image_Radio_button(this, ":/fontAlignToTop", ":/fontAlignToTop", ":/fontAlignToTop_c");
    m_VAlignCenter = new image_Radio_button(this, ":/fontAlignToVCenter", ":/fontAlignToVCenter", ":/fontAlignToVCenter_c");
    m_VAlignBottom = new image_Radio_button(this, ":/fontAlignToBottom", ":/fontAlignToBottom", ":/fontAlignToBottom_c");

    // 创建一个组，将radiobutton添加到组中
    m_hGroup = new QButtonGroup(this);
    m_hGroup->addButton(m_HAlignLeft);
    m_hGroup->addButton(m_HAlignCenter);
    m_hGroup->addButton(m_HAlignRight);

    m_vGroup = new QButtonGroup(this);
    m_vGroup->addButton(m_VAlignTop);
    m_vGroup->addButton(m_VAlignCenter);
    m_vGroup->addButton(m_VAlignBottom);

    // 是否加粗
    m_boldCombox->addItem(tr("unbold"), false);
    m_boldCombox->addItem(tr("bold"), true);

    for (int i = 0; i <= 10 ; i++)
    {
        m_opacityCombox->addItem(QString::number(i / 10.0, 'f', 1));
    }

    //
    QObject *pobject = object();
    if (pobject)
    {
        // 字体
        UpdateFont(pobject->property(PROPERTY_TO_STRING(FONT_PROPERTY)).value<QFont>());
        // 字体颜色透明度
        UpdateFontColor(pobject->property(PROPERTY_TO_STRING(FONT_COLOR_PROPERTY)).value<QColor>());
        // 字体对齐,第一次读是错误值，这句代码先不要删。不然读取有问题。
        pobject->property(PROPERTY_TO_STRING(ALIGNMENT_PROPERTY)).value<Qt::Alignment>();
        UpdateAlignment(pobject->property(PROPERTY_TO_STRING(ALIGNMENT_PROPERTY)).value<Qt::Alignment>());
    }

    connectRegister();
}

void font_prop_widget::slotFontColorProperty(QObject *object, QColor newColor)
{
    if (object)
    {
        QColor fontColor = object->property(PROPERTY_TO_STRING(FONT_COLOR_PROPERTY)).value<QColor>();
        fontColor.setRgb(newColor.rgb());
        object->setProperty(PROPERTY_TO_STRING(FONT_COLOR_PROPERTY), fontColor);
    }
}

void font_prop_widget::slotFontColorProperty(const QList<QObject *> &objects, QColor newColor)
{
    for (QObject *it : objects)
    {
        slotFontColorProperty(it, newColor);
    }
}

void font_prop_widget::slotColorOpacityProperty(QObject *object, QString opt)
{
    if (object)
    {
        QColor fontColor = object->property(PROPERTY_TO_STRING(FONT_COLOR_PROPERTY)).value<QColor>();
        fontColor.setAlphaF(opt.toDouble());
        object->setProperty(PROPERTY_TO_STRING(FONT_COLOR_PROPERTY), fontColor);
    }
}

void font_prop_widget::slotColorOpacityProperty(const QList<QObject *> &objects,  QString opt)
{
    for (QObject *it : objects)
    {
        slotColorOpacityProperty(it, opt);
    }
}

void font_prop_widget::slotFontFamilyProperty(QObject *object, QFont font)
{
    if (object)
    {
        QFont itemFont = object->property(PROPERTY_TO_STRING(FONT_PROPERTY)).value<QFont>();
        itemFont.setFamily(font.family());
        object->setProperty(PROPERTY_TO_STRING(FONT_PROPERTY), itemFont);
    }
}

void font_prop_widget::slotFontFamilyProperty(const QList<QObject *> &objects, QFont font)
{
    for (QObject *it : objects)
    {
        slotFontFamilyProperty(it, font);
    }
}

void font_prop_widget::slotFontBoldProperty(QObject *object, bool bold)
{
    if (object)
    {
        QFont itemFont = object->property(PROPERTY_TO_STRING(FONT_PROPERTY)).value<QFont>();
        itemFont.setBold(bold);
        object->setProperty(PROPERTY_TO_STRING(FONT_PROPERTY), itemFont);
    }
}

void font_prop_widget::slotFontBoldProperty(const QList<QObject *> &objects, bool bold)
{
    for (QObject *it : objects)
    {
        slotFontBoldProperty(it, bold);
    }
}

void font_prop_widget::UpdateAlignment(Qt::Alignment align)
{
    if (align & Qt::AlignLeft)
    {
        m_HAlignLeft->setChecked(true);
    }
    else if (align & Qt::AlignRight)
    {
        m_HAlignRight->setChecked(true);
    }
    else if (align & Qt::AlignHCenter)
    {
        m_HAlignCenter->setChecked(true);
    }

    if (align & Qt::AlignTop)
    {
        m_VAlignTop->setChecked(true);
    }
    else if (align & Qt::AlignBottom)
    {
        m_VAlignBottom->setChecked(true);
    }
    else if (align & Qt::AlignVCenter)
    {
        m_VAlignCenter->setChecked(true);
    }
}

void font_prop_widget::UpdateFontColor(QColor fontColor)
{
    QString tem = QString("%1").arg(QString::number(fontColor.alphaF(), 'f', 1));
    for (int i = 0; i < m_opacityCombox->count(); ++i)
    {
        if (m_opacityCombox->itemText(i) == tem)
        {
            m_opacityCombox->setCurrentIndex(i);
            break;
        }
    }
    m_fontColorButton->setColor(fontColor);
}

void font_prop_widget::slot_pointSizeSpinbox(const QString &text)
{
    slotPointSizeSpinbox(object(), text);
    slotPointSizeSpinbox(objects(), text);
}

void font_prop_widget::slotPointSizeSpinbox(QObject *object, const QString &text)
{
    if (object)
    {
        QFont itemFont = object->property(PROPERTY_TO_STRING(FONT_PROPERTY)).value<QFont>();
        itemFont.setPointSize(text.toInt());
        object->setProperty(PROPERTY_TO_STRING(FONT_PROPERTY), itemFont);
    }
}

void font_prop_widget::slotPointSizeSpinbox(const QList<QObject *> &objects, const QString &text)
{
    for (QObject *it : objects)
    {
        slotPointSizeSpinbox(it, text);
    }
}

void font_prop_widget::slotAlignButtonCheack(QObject *object, QAbstractButton *button)
{
    if (!object)
    {
        return;
    }

    Qt::Alignment align = object->property(PROPERTY_TO_STRING(ALIGNMENT_PROPERTY)).value<Qt::Alignment>();
    if (button == m_HAlignLeft)
    {
        align &= 0xfff0;
        align |= Qt::AlignLeft;
    }
    else if (button == m_HAlignCenter)
    {
        align &= 0xfff0;
        align |= Qt::AlignHCenter;
    }
    else if (button == m_HAlignRight)
    {
        align &= 0xfff0;
        align |= Qt::AlignRight;
    }

    if (button == m_VAlignTop)
    {
        align &= 0xff0f;
        align |= Qt::AlignTop;
    }
    else if (button == m_VAlignCenter)
    {
        align &= 0xff0f;
        align |= Qt::AlignVCenter;
    }
    else if (button == m_VAlignBottom)
    {
        align &= 0xff0f;
        align |= Qt::AlignBottom;
    }
    object->setProperty(PROPERTY_TO_STRING(ALIGNMENT_PROPERTY), static_cast<int>(align));
}

void font_prop_widget::slotAlignButtonCheack(const QList<QObject *> &objects, QAbstractButton *button)
{
    for (QObject *it : objects)
    {
        slotAlignButtonCheack(it, button);
    }
}

void font_prop_widget::UpdateFont(QFont font)
{
    QString newf = font.family();
    QString cf = m_familyCombox->currentFont().family();
    if (!compareFonts(font, m_familyCombox->currentFont()))
    {
        m_familyCombox->setCurrentFont(font);
        m_boldCombox->setCurrentIndex(font.bold());
        m_pointSizeSpinbox->setValue(font.pointSize());
    }
}

void font_prop_widget::connectRegister()
{
    connect(m_familyCombox, &QFontComboBox::currentFontChanged, [this](const QFont & font)
    {
        slotFontFamilyProperty(object(), font);
        slotFontFamilyProperty(objects(), font);
    });

    connect(m_pointSizeSpinbox, SIGNAL(valueChanged(const QString &)), this, SLOT(slot_pointSizeSpinbox(const QString &)));
    connect(m_boldCombox, &prop_combox::currentTextChanged, [this](const QString & text)
    {
        slotFontBoldProperty(object(), m_boldCombox->currentData().toBool());
        slotFontBoldProperty(objects(), m_boldCombox->currentData().toBool());
    });

    connect(m_opacityCombox, &prop_combox::currentTextChanged, [this](const QString & text)
    {
        slotColorOpacityProperty(object(), text);
        slotColorOpacityProperty(objects(), text);
    });

    connect(m_fontColorButton, &prop_color_button::colorChange, [this](QColor color)
    {
        slotFontColorProperty(object(), color);
        slotFontColorProperty(objects(), color);
    });

    // 连接信号和槽函数
    //    QObject::connect(m_hGroup, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(slot_alignButtonCheack(QAbstractButton *)));
    //    QObject::connect(m_vGroup, SIGNAL(buttonClicked(QAbstractButton *)), this, SLOT(slot_alignButtonCheack(QAbstractButton *)));

    QObject::connect(m_HAlignLeft, &image_Radio_button::clicked, [this]
    {
        slotAlignButtonCheack(object(), m_HAlignLeft);
        slotAlignButtonCheack(objects(), m_HAlignLeft);
    });

    QObject::connect(m_HAlignCenter, &image_Radio_button::clicked, [this]
    {
        slotAlignButtonCheack(object(), m_HAlignCenter);
        slotAlignButtonCheack(objects(), m_HAlignCenter);
    });

    QObject::connect(m_HAlignRight, &image_Radio_button::clicked, [this]
    {
        slotAlignButtonCheack(object(), m_HAlignRight);
        slotAlignButtonCheack(objects(), m_HAlignRight);
    });

    QObject::connect(m_VAlignTop, &image_Radio_button::clicked, [this]
    {
        slotAlignButtonCheack(object(), m_VAlignTop);
        slotAlignButtonCheack(objects(), m_VAlignTop);
    });

    QObject::connect(m_VAlignCenter, &image_Radio_button::clicked, [this]
    {
        slotAlignButtonCheack(object(), m_VAlignCenter);
        slotAlignButtonCheack(objects(), m_VAlignCenter);
    });

    QObject::connect(m_VAlignBottom, &image_Radio_button::clicked, [this]
    {
        slotAlignButtonCheack(object(), m_VAlignBottom);
        slotAlignButtonCheack(objects(), m_VAlignBottom);
    });
}

void font_prop_widget::update(QString PropertyName)
{
    if (!object())
    {
        return;
    }

    if (PropertyName == PROPERTY_TO_STRING(FONT_COLOR_PROPERTY))
    {
        UpdateFontColor(object()->property(PROPERTY_TO_STRING(FONT_COLOR_PROPERTY)).value<QColor>());
    }
    else if (PropertyName == PROPERTY_TO_STRING(FONT_PROPERTY))
    {
        UpdateFont(object()->property(PROPERTY_TO_STRING(FONT_PROPERTY)).value<QFont>());
    }
    else if (PropertyName == PROPERTY_TO_STRING(ALIGNMENT_PROPERTY))
    {
        UpdateAlignment(object()->property(PROPERTY_TO_STRING(ALIGNMENT_PROPERTY)).value<Qt::Alignment>());
    }
}

}
