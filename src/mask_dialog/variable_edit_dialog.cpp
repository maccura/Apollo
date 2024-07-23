#include "mask_dialog/variable_edit_dialog.hpp"
#include "ui_variable_edit_dialog.h"
#include "global.hpp"
#include<QFont>
#define VARIANT_ID_COL      0
#define VARIANT_NAME_COL    1
#define VARIANT_CHEACK_COL  2

namespace ReportDesign
{
variable_edit_dialog::variable_edit_dialog(bool inTable, base_homogeneous_variant *dataSource,
                                           variant_info variable, variant_info tableVariableID) :
    base_mask_dialog(QApplication::activeWindow()), m_inTable(inTable), m_dataSource(dataSource),
    m_ok(new gbase_push_button(this)), m_cancel(new base_push_button(this)),
    ui(new Ui::variable_edit_dialog)
{
    ui->setupUi(this);
    init();
    initVaraintList();
    updateCheckStatus(variable, tableVariableID);
    ui->textEdit->setFocus();
    ui->textEdit->selectAll();

    QKeyEvent *keyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Super_R, Qt::NoModifier, "");
    QApplication::sendEvent(ui->textEdit, keyEvent);
}

void variable_edit_dialog::init()
{
    resize(800, 600);
    SetTitle(tr("text editing"));
    // textEdit
    ui->textEdit->resize(385, 455);
    ui->textEdit->move(10, 61);

    //    ui->label_2->resize(58, 16);
    //    ui->label_2->move(404, 70);
    ui->label_2->hide();
    ui->comboBox->resize(200, 32);
    ui->comboBox->move(405, 61);

    // 表格
    ui->tableWidget->resize(385, 413);
    ui->tableWidget->move(405, 103);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 不可編輯s
    if (!m_inTable)
    {
        ui->comboBox->hide();
        ui->label_2->hide();
        QSize newSize(ui->tableWidget->width(), ui->textEdit->height());
        ui->tableWidget->move(QPoint(ui->tableWidget->x(), ui->textEdit->y()));
        ui->tableWidget->resize(newSize);
    }
    else
    {
        connect(ui->comboBox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(slotLoadVaraint()));
    }

    // 设置水平表头的背景色
    ui->tableWidget->setColumnCount(3);
    QTableWidgetItem *headerItem1 = new QTableWidgetItem("ID");
    headerItem1->setBackground(QBrush(Qt::transparent)); // 设置背景色为透明
    QTableWidgetItem *headerItem2 = new QTableWidgetItem("Type");
    headerItem2->setBackground(QBrush(Qt::transparent)); // 设置背景色为透明
    QTableWidgetItem *headerItem3 = new QTableWidgetItem("Check");
    headerItem3->setBackground(QBrush(Qt::transparent)); // 设置背景色为透明
    ui->tableWidget->setHorizontalHeaderItem(VARIANT_ID_COL, headerItem1);
    ui->tableWidget->setColumnHidden(VARIANT_ID_COL, true);
    ui->tableWidget->setHorizontalHeaderItem(VARIANT_NAME_COL, headerItem2);
    ui->tableWidget->setHorizontalHeaderItem(VARIANT_CHEACK_COL, headerItem3);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->horizontalHeader()->resize(385, 40);
    ui->tableWidget->setShowGrid(false);

    // 设置水平表头可拖动
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    // 变量Name列宽度自适应
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(VARIANT_NAME_COL, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(VARIANT_CHEACK_COL, QHeaderView::ResizeToContents);

    // 设置表格行选
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(ui->tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(slotSelectVariant(int, int)));

    // 按钮
    m_cancel->move(540, 536);
    m_cancel->setText(tr("cancle"));
    connect(m_cancel, &QPushButton::clicked, [this]
    {
        close();
    });

    m_ok->move(670, 536);
    m_ok->setText(tr("ok"));
    connect(m_ok, &QPushButton::clicked, this, &variable_edit_dialog::slotOk);
}

variable_edit_dialog::~variable_edit_dialog()
{
    if (!m_dataSource)
    {
        delete m_dataSource;
    }
    delete ui;
}

void variable_edit_dialog::slotSelectVariant(int row, int col)
{
    col = VARIANT_CHEACK_COL;
    if (col == VARIANT_CHEACK_COL)
    {
        QTableWidgetItem *idItem = ui->tableWidget->item(row, col);
        // 勾选
        if (idItem->icon().isNull())
        {
            //清空该列的check图标
            for (int i = 0; i < ui->tableWidget->rowCount() ; ++i)
            {
                QTableWidgetItem *Item = ui->tableWidget->item(i, VARIANT_CHEACK_COL);
                if (Item)
                {
                    Item->setIcon(QIcon());
                }
            }

            //添加check图标
            idItem->setIcon(QIcon(":/variantCheck"));

            // 更新编辑区内容
            QTableWidgetItem *nameItem = ui->tableWidget->item(row, VARIANT_NAME_COL);
            if (nameItem)
            {
                ui->textEdit->setText(nameItem->text());
            }
        }
        else //取消勾选
        {
            idItem->setIcon(QIcon());
            ui->textEdit->clear();
        }
    }

}

int variable_edit_dialog::currentVariable()
{
    for (int i = 0; i < ui->tableWidget->rowCount() ; ++i)
    {
        QTableWidgetItem *Item = ui->tableWidget->item(i, VARIANT_CHEACK_COL);
        if (!Item->icon().isNull())
        {
            return i;
        }
    }

    return -1;
}

void variable_edit_dialog::updateCheckStatus(variant_info variable, variant_info tableVariableID)
{
    if (m_inTable)
    {
        for (int i = 0; i < ui->comboBox->count(); ++i)
        {
            if (tableVariableID.m_ID == ui->comboBox->itemData(i).value<variant_info>().m_ID)
            {
                ui->comboBox->setCurrentIndex(i);
                slotLoadVaraint();
                break;
            }
        }
    }

    for (int i = 0; i < ui->tableWidget->rowCount() ; ++i)
    {
        QTableWidgetItem *Item = ui->tableWidget->item(i, VARIANT_ID_COL);
        if (Item->text() == variable.m_ID)
        {
            QTableWidgetItem *checkItem = ui->tableWidget->item(i, VARIANT_CHEACK_COL);
            checkItem->setIcon(QIcon(":/variantCheck"));
            ui->textEdit->setText(variable.m_name);
            return;
        }
    }

    // 静态文本
    ui->textEdit->setText(variable.m_name);
}

void variable_edit_dialog::slotLoadVaraint()
{
    if (!m_dataSource)
    {
        return;
    }

    QVector<variant_info> Variant;
    if (m_inTable)
    {
        Variant = m_dataSource->childVariants(ui->comboBox->currentData().value<variant_info>().m_ID);
    }
    else
    {
        Variant = m_dataSource->variant();
    }

    ui->tableWidget->setRowCount(Variant.count());
    for (int i = 0; i < Variant.count(); ++i)
    {
        QTableWidgetItem *IDItem = new QTableWidgetItem(Variant[i].m_ID);
        QFont font("HarmonyOS Sans SC Medium", 12); // 创建一个Arial字体，大小为12
        IDItem->setFont(font); // 设置字体
        ui->tableWidget->setItem(i, VARIANT_ID_COL, IDItem);
        QTableWidgetItem *nameItem = new QTableWidgetItem(Variant[i].m_name);
        nameItem->setFont(font); // 设置字体
        ui->tableWidget->setItem(i, VARIANT_NAME_COL, nameItem);
        QTableWidgetItem *checkItem = new QTableWidgetItem();
        checkItem->setSizeHint(QSize(QIcon(":/variantCheck").pixmap(ui->tableWidget->iconSize()).size()));
        checkItem->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(i, VARIANT_CHEACK_COL, checkItem);
    }
}

void variable_edit_dialog::initVaraintList()
{
    if (!m_dataSource)
    {
        return;
    }

    QVector<variant_info> vecTable = m_dataSource->variant();
    // 表格控件
    if (m_inTable)
    {
        //
        for (auto item : vecTable)
        {
            QVariant variant;
            variant.setValue(item);
            ui->comboBox->addItem(item.m_name, variant);
        }
    }
    slotLoadVaraint();
}

void variable_edit_dialog::slotOk()
{
    if (!m_dataSource)
    {
        close();
        return;
    }

    int row = currentVariable();
    QString editString = ui->textEdit->toPlainText();

    // 既没有选择变量，也没有输入文本
    if (editString.isEmpty() && row == -1)
    {
        emit variable(variant_info());
        if (m_inTable)
        {
            emit tableVariable(variant_info(), variant_info());
        }
        close();
        return ;
    }

    // 没选择变量，只输入了静态文本
    variant_info variant;
    if (row == -1)
    {
        variant.m_name = editString;
        emit variable(variant);
        close();
        return;
    }

    // 选择了变量，允许用户修改值
    QTableWidgetItem *item = ui->tableWidget->item(row, VARIANT_ID_COL);
    if (item)
    {
        if (m_inTable)
        {
            variant = m_dataSource->childVariants(ui->comboBox->currentData().value<variant_info>().m_ID, item->text());
            if (variant.m_name != editString)
            {
                variant.m_nameChange = true;
            }
            variant.m_name = editString;
            emit tableVariable(variant, ui->comboBox->currentData().value<variant_info>());
        }
        else
        {
            variant = m_dataSource->variantInfo(item->text());
            if (variant.m_name != editString)
            {
                variant.m_nameChange = true;
            }
            variant.m_name = editString;
        }
    }
    emit variable(variant);
    close();
}
}

