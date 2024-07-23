#include "language_type_dialog.hpp"
#include <QDir>
#include <QAbstractItemView>
#include <QMetaEnum>
#include <QCoreApplication>
#include "global.hpp"
#include "ini_manager.hpp"
namespace ReportDesign
{
language_type_dialog::language_type_dialog(QWidget *parent) :
    base_mask_dialog(parent), m_cancel(new base_push_button(this)), m_ok(new gbase_push_button(this)),
    m_language(new QComboBox(this))
{
    tr("chinese");
    tr("english");
    tr("russian");
    tr("japanese");
    tr("french");
    tr("latin");
    tr("german");
    tr("spanish");
    tr("italian");
    init();
}

language_type_dialog::~language_type_dialog()
{

}

int language_type_dialog::language()
{
    int cur_langIndex = -1;
    QMetaEnum m = QMetaEnum::fromType<ini_manager::Languages>();
    ini_manager::Languages curLang = ini_manager::instance().language();
    for (int i = 0; i < m.keyCount(); ++i)
    {
        QString qmFile = ReportDesign::ini_manager::instance().curLangFile(QCoreApplication::applicationDirPath() + LANGUAGE_FILE_PATH,
                                                                           static_cast<ini_manager::Languages>(m.value(i)));
        if (!qmFile.isEmpty())
        {
            m_language->insertItem(i, QIcon(), tr(m.key(i)), m.value(i));
        }

        if (curLang == m.value(i))
        {
            cur_langIndex = m_language->findData(m.value(i));
        }
    }

    return cur_langIndex;
}

void language_type_dialog::init()
{
    resize(650, 300);
    SetTitle(tr("language"));
    m_language->move(98, 71);
    m_language->resize(250, 32);

    QLabel *label = new QLabel(this);
    label->move(20, 80);
    label->setText(tr("language"));
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 设置大小策略为自动扩展
    // 调整大小以适应内容
    label->adjustSize();

    m_language->move(20 + label->width() + 20, 75);

    m_cancel->move(390, 236);
    m_cancel->setText(tr("cancle"));
    connect(m_cancel, &QPushButton::clicked, [this]
    {
        close();
    });

    m_ok->move(520, 236);
    m_ok->setText(tr("ok"));
    connect(m_ok, &QPushButton::clicked, [this]
    {
        slotOk();
        close();
    });

    // 当前选择语言
    m_language->setCurrentIndex(language());
}

QString language_type_dialog::valueToKey(int index)
{
    return QMetaEnum::fromType<ini_manager::Languages>().valueToKey(index);
}

int language_type_dialog::keysToValue(QString name)
{
    return  QMetaEnum::fromType<ini_manager::Languages>().keysToValue(name.toUtf8().constData());
}

void language_type_dialog::slotOk()
{
    int value = m_language->currentData().toInt();
    if (value == -1)
    {
        return;
    }
    ini_manager::instance().setLanguage(value);
}
}
