#include "message_info_dialog.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
namespace ReportDesign
{

message_info_dialog::message_info_dialog(QWidget *parent):
    base_mask_dialog(parent), m_cancel(new base_push_button(this)),
    m_no(new base_push_button(this)), m_yes(new gbase_push_button(this)),
    m_messageLabel(new QLabel(this)), m_ret(cancle)
{
    init();
}

void message_info_dialog::init()
{
    resize(650, 300);
    SetTitle(tr("Tips"));
    m_messageLabel->setMinimumSize(600, 150);
    QVBoxLayout *Vlayout = VLayout();
    QHBoxLayout *HLayout = new QHBoxLayout(this);
    HLayout->addSpacing(20);
    HLayout->addWidget(m_messageLabel);
    Vlayout->addLayout(HLayout);
    Vlayout->addStretch();
    QHBoxLayout *HLayout2 = new QHBoxLayout(this);
    HLayout2->addSpacing(260);
    HLayout2->addWidget(m_cancel);
    m_cancel->setMinimumSize(110, 44);
    m_cancel->setText(tr("cancle"));
    connect(m_cancel, &QPushButton::clicked, [this] { m_ret = cancle; close();});
    HLayout2->addSpacing(20);
    HLayout2->addWidget(m_no);
    m_no->setMinimumSize(110, 44);
    m_no->setText(tr("no"));
    connect(m_no, &QPushButton::clicked, [this] {  m_ret = no;  close();});
    HLayout2->addSpacing(20);
    HLayout2->addWidget(m_yes);
    m_yes->setMinimumSize(110, 44);
    m_yes->setText(tr("yes"));
    connect(m_yes, &QPushButton::clicked, [this] { m_ret = yes;  close();});
    HLayout2->addSpacing(20);
    Vlayout->addLayout(HLayout2);
}

void message_info_dialog::setMessage(QString message)
{
    m_messageLabel->setText(message);
}

message_info_dialog::DlgResult message_info_dialog::message(QWidget *parent, QString title, QString message)
{
    message_info_dialog *dlg = new message_info_dialog(parent);
    dlg->SetTitle(title);
    dlg->setMessage(message);
    dlg->exec();
    message_info_dialog::DlgResult ret = dlg->dlgResult();
    delete dlg;
    return ret;
}

}
