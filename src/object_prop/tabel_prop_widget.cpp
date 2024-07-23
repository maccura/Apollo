#include "tabel_prop_widget.hpp"
#include "property_widget_factory.hpp"
#include "item/base_design_item.hpp"
#include "item/page_scene.hpp"
#include "item/table_item.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QButtonGroup>
#include <QMessageBox>
namespace
{
ReportDesign::base_prop_widget *createTabelPropWidget(QObject *object, QList<QObject *> *objects, QWidget *parent)
{
    if (dynamic_cast<ReportDesign::base_design_item *>(object) == nullptr)
    {
        return nullptr;
    }
    return new ReportDesign::tabel_prop_widget(object, objects, parent);
}

bool registred = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(TABLE_ROW_COUNT_PROPERTY), createTabelPropWidget);
bool registred2 = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(TABLE_ROW_HEIGHT_PROPERTY), createTabelPropWidget);
bool registred3 = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(TABLE_COLUMN_COUNT_PROPERTY), createTabelPropWidget);
bool registred4 = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(TABLE_COLUMN_WIDTH_PROPERTY), createTabelPropWidget);
bool registred5 = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(TABLE_SHOW_GRID_PROPERTY), createTabelPropWidget);
bool registred6 = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(TABLE_SHOW_HEADER_PROPERTY), createTabelPropWidget);
bool registred7 = ReportDesign::property_widget_factory::instance().registerPropertyWidget(PROPERTY_TO_STRING(TABLE_UNDER_LINE_PROPERTY), createTabelPropWidget);
}

namespace ReportDesign
{
tabel_prop_widget::tabel_prop_widget(QObject *object, QList<QObject *> *objects, QWidget *parent):
    base_prop_widget(object, objects, parent), m_isTable(false)
{
    init();
    QVBoxLayout *vlayout = new QVBoxLayout(this);
    QHBoxLayout *hlayout = new QHBoxLayout();
    vlayout->addWidget(new QLabel(tr("table"), this));

    hlayout->addWidget(m_colCount, 1);
    hlayout->addWidget(m_colWidth, 1);
    hlayout->addItem(new QSpacerItem(40, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));
    m_colWidth->setMinimumWidth(87);
    vlayout->addLayout(hlayout);

    hlayout = new QHBoxLayout();
    hlayout->addWidget(m_rowCount, 1);
    hlayout->addWidget(m_rowHeight, 1);
    hlayout->addItem(new QSpacerItem(40, 0, QSizePolicy::Fixed, QSizePolicy::Minimum));
    m_rowHeight->setMinimumWidth(87);
    vlayout->addLayout(hlayout);

    hlayout = new QHBoxLayout();
    hlayout->addWidget(m_showGrid);
    hlayout->addWidget(new QLabel(tr("show grid"), this), 1);
    hlayout->addStretch();
    hlayout->addWidget(m_showHeader);
    hlayout->addWidget(new QLabel(tr("show header"), this), 1);
    hlayout->addStretch();
    vlayout->addLayout(hlayout);
    if (m_isTable)
    {
        hlayout = new QHBoxLayout();
        hlayout->addWidget(m_underLine);
        hlayout->addWidget(new QLabel(tr("underline"), this), 1);
        hlayout->addStretch();
        hlayout->addWidget(connect_lock);
        hlayout->addWidget(new QLabel(tr("table lock"), this), 1);
        hlayout->addStretch();
        vlayout->addLayout(hlayout);

        hlayout = new QHBoxLayout();
        hlayout->addWidget(m_autoFill);
        hlayout->addWidget(new QLabel(tr("autoFill"), this), 1);
        hlayout->addStretch();
        vlayout->addLayout(hlayout);
    }
    vlayout->setContentsMargins(PROP_WIDGET_MARGINS);
}

void tabel_prop_widget::init()
{
    m_colCount = new prop_line_edit(tr("col count"), this);
    m_colWidth = new prop_spinbox_widget(this, tr("width"));
    m_rowCount = new prop_line_edit(tr("row count"), this);
    m_rowHeight = new prop_spinbox_widget(this, tr("height"));
    m_showHeader = new QCheckBox(this);
    m_showGrid = new QCheckBox(this);

    m_colWidth->setMaximum(std::numeric_limits<int>::max());
    m_rowHeight->setMaximum(std::numeric_limits<int>::max());


    if (object())
    {
        table_item *item = dynamic_cast<table_item *>(object());
        if (item)
        {
            m_isTable = true;
            m_underLine = new QCheckBox(this);
            connect_lock = new QCheckBox(this);
            m_autoFill = new QCheckBox(this);
        }
    }

    QObject *pobject = object();
    if (pobject)
    {
        // 列数
        updateColCount(pobject->property(PROPERTY_TO_STRING(TABLE_COLUMN_COUNT_PROPERTY)).toInt());
        // 列宽
        updateColWidth(pobject->property(PROPERTY_TO_STRING(TABLE_COLUMN_WIDTH_PROPERTY)).toInt());
        //
        updateRowCount(pobject->property(PROPERTY_TO_STRING(TABLE_ROW_COUNT_PROPERTY)).toInt());
        updateRowHeight(pobject->property(PROPERTY_TO_STRING(TABLE_ROW_HEIGHT_PROPERTY)).toInt());
        updateShowHeader(pobject->property(PROPERTY_TO_STRING(TABLE_SHOW_HEADER_PROPERTY)).toBool());
        updateShowGrid(pobject->property(PROPERTY_TO_STRING(TABLE_SHOW_GRID_PROPERTY)).toBool());

        if (m_isTable)
        {
            updateUnderLine(pobject->property(PROPERTY_TO_STRING(TABLE_UNDER_LINE_PROPERTY)).toBool());
            updateLock(pobject->property(PROPERTY_TO_STRING(TABLE_TABLEITEM_ID_PROPERTY)).toInt(),
                       pobject->property(PROPERTY_TO_STRING(TABLE_CONNECTTABLEITEMID_PROPERTY)).toInt());
            updateAutoFill(pobject->property(PROPERTY_TO_STRING(TABLE_AUTO_FILL_PROPERTY)).toBool());
        }
    }

    connectRegister();
}

void tabel_prop_widget::updateColCount(int colCount)
{
    if (object())
    {
        m_colCount->setContent(QString::number(colCount));
    }
}

void tabel_prop_widget::setObjectProperty(QObject *object, QString propertyName, QVariant value)
{
    if (!object)
    {
        return;
    }
    object->setProperty(propertyName.toStdString().c_str(), value);
}

void tabel_prop_widget::setObjectsProperty(QList<QObject *> objects, QString propertyName, QVariant value)
{
    for (QObject *it : objects)
    {
        setObjectProperty(it, propertyName, value);
    }
}

void tabel_prop_widget::connectRegister()
{
    connect(m_colCount, &prop_line_edit::contentEditFinished, [this]
    {
        bool ret = false;
        int colCount = m_colCount->content().toInt(&ret);
        if (ret)
        {
            setObjectProperty(object(), PROPERTY_TO_STRING(TABLE_COLUMN_COUNT_PROPERTY), colCount);
            setObjectsProperty(objects(), PROPERTY_TO_STRING(TABLE_COLUMN_COUNT_PROPERTY), colCount);
        }
    });

    connect(m_colWidth, &prop_spinbox_widget::valueChanged, [this]
    {
        setObjectProperty(object(), PROPERTY_TO_STRING(TABLE_COLUMN_WIDTH_PROPERTY), m_colWidth->value());
        setObjectsProperty(objects(), PROPERTY_TO_STRING(TABLE_COLUMN_WIDTH_PROPERTY), m_colWidth->value());
    });

    connect(m_rowCount, &prop_line_edit::contentEditFinished, [this]
    {
        bool ret = false;
        int rowCount = m_rowCount->content().toInt(&ret);
        if (ret)
        {
            setObjectProperty(object(), PROPERTY_TO_STRING(TABLE_ROW_COUNT_PROPERTY), rowCount);
            setObjectsProperty(objects(), PROPERTY_TO_STRING(TABLE_ROW_COUNT_PROPERTY), rowCount);
        }
    });

    connect(m_rowHeight, &prop_spinbox_widget::valueChanged, [this]
    {
        setObjectProperty(object(), PROPERTY_TO_STRING(TABLE_ROW_HEIGHT_PROPERTY),  m_rowHeight->value());
        setObjectsProperty(objects(), PROPERTY_TO_STRING(TABLE_ROW_HEIGHT_PROPERTY),  m_rowHeight->value());
    });

    connect(m_showHeader, &QCheckBox::stateChanged, [this]
    {
        setObjectProperty(object(), PROPERTY_TO_STRING(TABLE_SHOW_HEADER_PROPERTY),  m_showHeader->isChecked());
        setObjectsProperty(objects(), PROPERTY_TO_STRING(TABLE_SHOW_HEADER_PROPERTY),  m_showHeader->isChecked());
    });

    connect(m_showGrid,  &QCheckBox::stateChanged, [this]
    {
        setObjectProperty(object(), PROPERTY_TO_STRING(TABLE_SHOW_GRID_PROPERTY),  m_showGrid->isChecked());
        setObjectsProperty(objects(), PROPERTY_TO_STRING(TABLE_SHOW_GRID_PROPERTY),  m_showGrid->isChecked());
    });


    if (m_isTable)
    {
        connect(m_underLine,  &QCheckBox::stateChanged, [this]
        {
            setObjectProperty(object(), PROPERTY_TO_STRING(TABLE_UNDER_LINE_PROPERTY),  m_underLine->isChecked());
            setObjectsProperty(objects(), PROPERTY_TO_STRING(TABLE_UNDER_LINE_PROPERTY),  m_underLine->isChecked());
        });

        connect(m_autoFill,  &QCheckBox::stateChanged, [this]
        {
            setObjectProperty(object(), PROPERTY_TO_STRING(TABLE_AUTO_FILL_PROPERTY),  m_autoFill->isChecked());
            setObjectsProperty(objects(), PROPERTY_TO_STRING(TABLE_AUTO_FILL_PROPERTY),  m_autoFill->isChecked());
            if (m_autoFill->isChecked())
            {
                m_rowCount->setReadOnly(true);
            }
            else
            {
                m_rowCount->setReadOnly(false);
            }
        });


        connect(connect_lock,  &QCheckBox::stateChanged, [this]
        {
            base_design_item *item =  dynamic_cast<base_design_item *>(object());
            if (item)
            {
                if (item->page())
                {
                    if (item->page()->isVariantLock())
                    {
                        item->page()->itemVariantUnLock();
                    }
                    else
                    {
                        if (!item->page()->itemVariantLock())
                        {
                            QMessageBox::warning(nullptr, tr("tip"), tr("Only a single table!"));
                        }
                    }
                }
            }
        });
    }
}

void tabel_prop_widget::updateColWidth(int colWidth)
{
    if (object())
    {
        m_colWidth->setValue(colWidth);
    }
}

void tabel_prop_widget::updateRowCount(int rowCount)
{
    if (object())
    {
        m_rowCount->setContent(QString::number(rowCount));
    }
}

void tabel_prop_widget::updateRowHeight(int rowHeight)
{
    if (object())
    {
        m_rowHeight->setValue(rowHeight);
    }
}

void tabel_prop_widget::updateShowHeader(bool flag)
{
    if (object())
    {
        m_showHeader->setChecked(flag);
    }
}

void tabel_prop_widget::updateShowGrid(bool flag)
{
    if (object())
    {
        m_showGrid->setChecked(flag);
    }
}

void tabel_prop_widget::updateUnderLine(bool flag)
{
    if (object())
    {
        m_underLine->setChecked(flag);
    }
}

void tabel_prop_widget::updateAutoFill(bool flag)
{
    if (object())
    {
        m_autoFill->setChecked(flag);
        if (m_autoFill->isChecked())
        {
            m_rowCount->setReadOnly(true);
        }
        else
        {
            m_rowCount->setReadOnly(false);
        }
    }
}

void tabel_prop_widget::updateLock(int itemID, int connectID)
{
    if (object())
    {
        // 锁定表格
        if (connectID != -1)
        {
            connect_lock->setChecked(true);
            if (itemID != connectID)
            {
                connect_lock->setCheckable(false);
            }
        }
        else
        {
            connect_lock->setChecked(false);
            connect_lock->setCheckable(true);
        }
    }
}

void tabel_prop_widget::update(QString PropertyName)
{
    if (PropertyName == PROPERTY_TO_STRING(TABLE_COLUMN_COUNT_PROPERTY))
    {
        updateColCount(object()->property(PROPERTY_TO_STRING(TABLE_COLUMN_COUNT_PROPERTY)).toInt());
    }
    else if (PropertyName == PROPERTY_TO_STRING(TABLE_COLUMN_WIDTH_PROPERTY))
    {
        updateColWidth(object()->property(PROPERTY_TO_STRING(TABLE_COLUMN_WIDTH_PROPERTY)).toInt());
    }
    else if (PropertyName == PROPERTY_TO_STRING(TABLE_ROW_COUNT_PROPERTY))
    {
        updateRowCount(object()->property(PROPERTY_TO_STRING(TABLE_ROW_COUNT_PROPERTY)).toInt());
    }
    else if (PropertyName == PROPERTY_TO_STRING(TABLE_ROW_HEIGHT_PROPERTY))
    {
        updateRowHeight(object()->property(PROPERTY_TO_STRING(TABLE_ROW_HEIGHT_PROPERTY)).toInt());
    }
    else if (PropertyName == PROPERTY_TO_STRING(TABLE_SHOW_HEADER_PROPERTY))
    {
        updateShowHeader(object()->property(PROPERTY_TO_STRING(TABLE_SHOW_HEADER_PROPERTY)).toBool());
    }
    else if (PropertyName == PROPERTY_TO_STRING(TABLE_SHOW_GRID_PROPERTY))
    {
        updateShowGrid(object()->property(PROPERTY_TO_STRING(TABLE_SHOW_GRID_PROPERTY)).toBool());
    }
    else if (PropertyName == PROPERTY_TO_STRING(TABLE_UNDER_LINE_PROPERTY))
    {
        updateUnderLine(object()->property(PROPERTY_TO_STRING(TABLE_UNDER_LINE_PROPERTY)).toBool());
    }
    else if (PropertyName == PROPERTY_TO_STRING(TABLE_AUTO_FILL_PROPERTY))
    {
        updateAutoFill(object()->property(PROPERTY_TO_STRING(TABLE_AUTO_FILL_PROPERTY)).toBool());
    }
    else if (PropertyName == PROPERTY_TO_STRING(TABLE_CONNECTTABLEITEMID_PROPERTY))
    {
        updateLock(object()->property(PROPERTY_TO_STRING(TABLE_TABLEITEM_ID_PROPERTY)).toInt(),
                   object()->property(PROPERTY_TO_STRING(TABLE_CONNECTTABLEITEMID_PROPERTY)).toInt());
    }
}
}
