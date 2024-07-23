#include "module_type_dialog.hpp"
#include "ui_module_type_dialog.h"
#include <QDir>
#include<QAbstractItemView>

#include "global.hpp"
#define TABLEWIDGETCOLUMNCOUNT   5
namespace ReportDesign
{
module_type_dialog::module_type_dialog(QWidget *parent) :
    base_mask_dialog(parent), m_cancel(new base_push_button(this)), m_ok(new gbase_push_button(this)),
    ui(new Ui::module_type_dialog)
{
    ui->setupUi(this);
    init();
    LoadDefaultModule();
}

void module_type_dialog::init()
{
    resize(780, 584);
    SetTitle(tr("new report"));

    // label设置
    ui->label->move(20, 71);
    ui->label->setText(tr("file name"));
    ui->label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 设置大小策略为自动扩展
    // 调整大小以适应内容
    ui->label->adjustSize();

    ui->label_2->move(320, 71);
    ui->label_2->setText(tr("module"));
    ui->label_2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 设置大小策略为自动扩展
    // 调整大小以适应内容
    ui->label_2->adjustSize();

    ui->label_3->move(20, 146);
    ui->label_3->resize(70, 15);
    ui->label_3->setText(tr("template"));
    // 编辑栏
    ui->lineEdit->move(20, 94);
    ui->lineEdit->resize(250, 32);

    // 下拉选择框
    ui->comboBox->move(320, 94);
    ui->comboBox->resize(250, 32);

    // 按钮
    m_cancel->move(520, 520);
    m_cancel->setMaximumWidth(150);
    m_cancel->setText(tr("cancle"));
    connect(m_cancel, &QPushButton::clicked, [this]
    {
        close();
    });

    m_ok->move(650, 520);
    m_ok->setMaximumWidth(150);
    m_ok->setText(tr("ok"));
    connect(m_ok, &QPushButton::clicked, this, &module_type_dialog::slotOk);
    connect(ui->tableWidget, &QTableWidget::cellPressed, this, &module_type_dialog::slotCellClicked);

    // Table
    ui->tableWidget->move(20, 170);
    ui->tableWidget->resize(140 + 600, 160 + 170);
    // 禁用水平表头
    ui->tableWidget->horizontalHeader()->setVisible(false);
    ui->tableWidget->setShowGrid(false);
    // 禁用垂直表头
    ui->tableWidget->verticalHeader()->setVisible(false);
    // 单选
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection); // 设置选择模式为单选
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems); // 设置选择行为为选中单元格

    QList<variant_info> moduleList = device_variant_manager::instance().moduleList();
    for (auto it : moduleList)
    {
        QVariant variant;
        variant.setValue(it);
        ui->comboBox->addItem(it.m_name, variant);
    }
}

void module_type_dialog::LoadDefaultModule()
{
    QDir dir(QCoreApplication::applicationDirPath() + DEFAULT_MODULE_FILE_PATH);
    // 获取目录下的所有文件
    QStringList fileList;
    fileList.append(tr("empty file"));
    fileList.append(dir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name));
    ui->tableWidget->setRowCount(fileList.count() / TABLEWIDGETCOLUMNCOUNT + static_cast<int>(fileList.count() % TABLEWIDGETCOLUMNCOUNT != 0));
    int columnCount = TABLEWIDGETCOLUMNCOUNT;
    if (fileList.count() / columnCount < 1)
    {
        columnCount = fileList.count();
    }
    ui->tableWidget->setColumnCount(columnCount);
    QSize buttonSize;
    for (int it = 0; it < fileList.count(); ++it)
    {
        QString fileName = fileList.at(it);
        module_file_button *fileButton = new module_file_button(fileName, ui->tableWidget);
        if (fileName == tr("empty file"))
        {
            fileButton->setImage(":/emptyFileName");
            fileButton->setObjectName("emptyFileButton");
            buttonSize = fileButton->size();
            fileButton->showBorder(false);
        }
        else
        {
            fileButton->setImage(":/moduleFileImage");
        }
        ui->tableWidget->setCellWidget(it / 5, it % 5, fileButton);
        fileButton->setToolTip(fileButton->fileName());
    }

    // 设置空文档 默认选中
    ui->tableWidget->setCurrentCell(0, 0);

    for (int i = 0; i < ui->tableWidget->rowCount() ; ++i)
    {
        ui->tableWidget->setRowHeight(i, buttonSize.height());
    }

    for (int i = 0; i < ui->tableWidget->columnCount() ; ++i)
    {
        ui->tableWidget->setColumnWidth(i, buttonSize.width());
    }
}

void module_type_dialog::slotCellClicked(int row, int col)
{
    for (int i = 0; i < ui->tableWidget->rowCount() ; ++i)
    {
        for (int j = 0; j < ui->tableWidget->colorCount(); ++j)
        {
            // 获取选中单元格的窗口部件并转换为自定义窗口类
            module_file_button *selectedCustomWidget = qobject_cast<module_file_button *>(ui->tableWidget->cellWidget(i, j));
            if (selectedCustomWidget != nullptr)
            {
                if (i == row && j == col)
                {
                    selectedCustomWidget->showBorder(false);
                }
                else
                {
                    selectedCustomWidget->showBorder(true);
                }

            }
        }
    }
    update();

}

void module_type_dialog::slotOk()
{
    QString defaultModuleFileName;

    int row = -1, column = -1;
    row = ui->tableWidget->currentRow();
    column =  ui->tableWidget->currentColumn();

    if (row != -1 && column != -1)
    {
        // 获取选中单元格的窗口部件并转换为自定义窗口类
        module_file_button *selectedCustomWidget = qobject_cast<module_file_button *>(ui->tableWidget->cellWidget(row, column));
        if (selectedCustomWidget != nullptr)
        {
            // 执行自定义窗口类的操作
            if (selectedCustomWidget->fileName() != tr("empty file"))
            {
                defaultModuleFileName = selectedCustomWidget->filePath();
            }

        }
    }
    emit moduleType(ui->comboBox->currentData().value<variant_info>(), ui->lineEdit->text(), defaultModuleFileName);
    close();
}

module_type_dialog::~module_type_dialog()
{
    delete ui;
}

}

