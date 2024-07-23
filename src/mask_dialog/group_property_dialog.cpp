#include "group_property_dialog.hpp"
#include <QLabel>
#include <QMessageBox>
#include "data_manager/device_variant_manager.hpp"
namespace ReportDesign
{
group_property_dialog::group_property_dialog(page_scene *pageScene, QWidget *parent):
    base_mask_dialog(parent, true, false), m_variantCombox(new prop_combox(this)), m_pageScene(pageScene),
    m_repeatCheckBox(new QCheckBox(this)), m_groupPageBreakBox(new QCheckBox(this)), m_cancelButton(new base_push_button(this)),
    m_okButton(new gbase_push_button(this))
{
    SetTitle(tr("group property"));
    resize(650, 300);
    m_variantCombox->resize(250, 32);
    m_variantCombox->setEnabled(false);

    m_repeatCheckBox->move(20, 71);
    m_repeatCheckBox->resize(20, 20);
    connect(m_repeatCheckBox, &QCheckBox::clicked, this, &group_property_dialog::slotRepeatCheck);
    m_groupPageBreakBox->move(m_repeatCheckBox->x(), m_repeatCheckBox->pos().y() + m_repeatCheckBox->height() + 20);

    QLabel *label =  new QLabel(this);
    label->setText(tr("repeat"));
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 设置大小策略为自动扩展
    label->setAlignment(Qt::AlignLeft);
    // 调整大小以适应内容
    label->adjustSize();
    label->move(m_repeatCheckBox->pos().x() + m_repeatCheckBox->width(), m_repeatCheckBox->y());

    QLabel *label2 =  new QLabel(this);
    label2->setText(tr("groupPageBreak"));
    label2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 设置大小策略为自动扩展
    label2->setAlignment(Qt::AlignLeft);
    // 调整大小以适应内容
    label2->adjustSize();
    label2->move(m_groupPageBreakBox->pos().x() + 20, m_groupPageBreakBox->pos().y() + 5);

    m_variantCombox->move(label->x() + label->width() + 10, 71);
    m_cancelButton->move(390, 236);
    m_cancelButton->setText(tr("cancle"));
    connect(m_cancelButton, &QPushButton::clicked, [this]
    {
        close();
    });

    m_okButton->move(520, 236);
    m_okButton->setText(tr("ok"));
    connect(m_okButton, &QPushButton::clicked, [this]
    {
        slotOkButton();
    });

    base_homogeneous_variant *Variant = nullptr;
    if (m_pageScene)
    {
        // 获取组变量
        Variant = device_variant_manager::instance()
                  .HomogeneousVariant(m_pageScene->pageItem()->module(), VariantType::Group);
    }

    if (Variant != nullptr)
    {
        // GROUP_VARIANT_TYPE
        QVector<variant_info> groupVariantList = Variant->variant();
        QVariant empty;
        empty.setValue<variant_info>(variant_info());
        m_variantCombox->addItem("", empty);
        for (auto item : groupVariantList)
        {
            QVariant data;
            data.setValue<variant_info>(item);
            m_variantCombox->addItem(item.m_name, data);
        }
    }

    // 获取当前选中的item 的组属性
    QMap<int, QList<base_design_item *>>  itemList =  m_pageScene->selectGroupItem();
    if (itemList.count() > 0)
    {
        base_design_item *item = (*itemList.begin()).back();
        if (item)
        {
            m_groupPageBreakBox->setChecked(item->groupPageBreak());
            variant_info groupVariant = item->groupVariant();
            if (groupVariant != variant_info())
            {
                m_repeatCheckBox->setChecked(true);
                m_variantCombox->setEnabled(true);
                setComBoxToID(groupVariant.m_ID);
            }
        }
    }
}

void group_property_dialog::setComBoxToID(QString ID)
{
    for (int i = 0; i < m_variantCombox->count(); ++i)
    {
        if (m_variantCombox->itemData(i).value<variant_info>().m_ID == ID)
        {
            m_variantCombox->setCurrentIndex(i);
            return;
        }
    }
}

void group_property_dialog::setComBoxToEmpty()
{
    m_variantCombox->setCurrentIndex(0);
}

void group_property_dialog::slotRepeatCheck(bool cheaked)
{
    // 选择组变量
    if (cheaked)
    {
        // 保证选中的item只有一个组;
        QMap<int, QList<base_design_item *>> groupItems = m_pageScene->selectGroupItem();
        if (groupItems.count() > 1)
        {
            QMessageBox::information(nullptr, "info", "You can set properties only for a single group"); // 您只能为单个组设置属性
            return;
        }

        // 判断所选item 是否variant 为空。非空不允许选择组变量
        QList<base_design_item *> groupItemList = *groupItems.begin();
        for (base_design_item *item : groupItemList)
        {
            if (!item->variantIsEmpty())
            {
                QMessageBox::information(nullptr, "info", "A component with a variable set exists in the group"); //组中存在已设置了变量的元件
                return;
            }
        }

        // 设置变量combox 可选
        m_variantCombox->setEnabled(true);
        return;
    }

    // 取消组变量
    m_variantCombox->setEnabled(false);
    setComBoxToEmpty();
}

// 完成组属性设置编辑
void group_property_dialog::slotOkButton()
{
    variant_info Variant;
    if (m_repeatCheckBox->isChecked())
    {
        Variant = m_variantCombox->currentData().value<variant_info>();
        if (Variant == variant_info())
        {
            QMessageBox::information(nullptr, "info",
                                     "The repeat property does not select a variable, please select a variable"); //repeat属性未选择变量，请选择变量
            return;
        }
    }
    emit GroupProperty(m_groupPageBreakBox->isChecked(), Variant);
    close();
}
}
