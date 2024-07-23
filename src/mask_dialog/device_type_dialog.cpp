#include "device_type_dialog.hpp"
#include "ui_device_type_dialog.h"
#include "data_manager/device_variant_manager.hpp"
#include "ini_manager.hpp"
#include "global.hpp"
#include <QGraphicsOpacityEffect>
namespace ReportDesign
{
device_type_dialog::device_type_dialog(QWidget *parent) :
    base_mask_dialog(parent, true, false), ui(new Ui::device_type_dialog), m_cancelButton(new base_push_button(this)),
    m_okButton(new gbase_push_button(this))
{
    ui->setupUi(this);
    init();
}

device_type_dialog::~device_type_dialog()
{
    delete ui;
}

void device_type_dialog::init()
{
    resize(650, 300);
    SetTitle(tr("Instrument selection"));//仪器选择

    ui->comboBox->resize(250, 32);

    ui->label->setText(tr("device"));
    ui->label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 设置大小策略为自动扩展
    ui->label->setAlignment(Qt::AlignRight);
    // 调整大小以适应内容
    ui->label->adjustSize();
    ui->label->move(20, 76);

    ui->comboBox->move(20 + ui->label->width() + 20, 71);

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
        close();
    });

    // 仪器列表
    QStringList deviceList = device_variant_manager::instance().deviceList(QCoreApplication::applicationDirPath() + DEVICE_JSON_PATH, ini_manager::instance().language());
    ui->comboBox->addItems(deviceList);
}

void device_type_dialog::slotOkButton()
{
    QString deviceType = ui->comboBox->currentText();
    if (device_variant_manager::instance().loadDeviceJson(deviceType))
    {
        emit DeviceType(deviceType);
        return;
    }
    else
    {
        // 加载失败提醒
        QPalette palette = ui->label_2->palette();
        palette.setColor(QPalette::Text, QColor("red"));
        ui->label_2->setPalette(palette);
        ui->label_2->setText(tr(QString("%1 文件加载失败！软件不可用！请联系工程师！").arg(deviceType).toUtf8().constData()));
    }
    emit DeviceType("");
}
}
