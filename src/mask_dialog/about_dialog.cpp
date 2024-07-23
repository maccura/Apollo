#include "about_dialog.hpp"
#include "global.hpp"
#include <QVBoxLayout>
#include <QFile>
#include <QTextStream>
namespace ReportDesign
{
about_dialog::about_dialog(QWidget *parent /*= 0*/):
    QDialog(parent), m_infoTable(new QTabWidget(this)),
    m_licenseTab(new QWidget(m_infoTable))
{
    init();
}

about_dialog::~about_dialog()
{

}

void about_dialog::init()
{
    // 关闭自销毁
    setAttribute(Qt::WA_DeleteOnClose, true);
    m_infoTable->setFixedSize(640, 480);

    QVBoxLayout *verticalLayout = new QVBoxLayout(m_licenseTab);
    m_licenseText = new QTextEdit(m_licenseTab);
    m_licenseText->setReadOnly(true);

    m_QtVerText = new QTextEdit(m_licenseTab);
    m_QtVerText->setReadOnly(true);
    verticalLayout->addWidget(m_licenseText);
    verticalLayout->addWidget(m_QtVerText);
    m_infoTable->addTab(m_licenseTab, QString("软件版权声明"));
    m_infoTable->addTab(m_QtVerText, QString("Qt版权声明"));

    loadSoftDes();
    loadQtDes();
}

void about_dialog::loadSoftDes()
{
    m_licenseText->append(readDes(":/SOFTDES"));
    m_licenseText->append("https://github.com/maccura/Apollo");
    m_licenseText->append("VERSION 1");
}

void about_dialog::loadQtDes()
{
    m_QtVerText->append(readDes(":/LICENSE"));
    m_QtVerText->append("https://www.gnu.org/licenses/gpl-3.0.txt");
    m_QtVerText->append("VERSION 3");
}

QString about_dialog::readDes(QString path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 处理错误，比如抛出异常或返回空字符串
        qWarning("Failed to open file: %s", qPrintable(file.errorString()));
    }

    QTextStream in(&file);
    return in.readAll(); // 读取所有内容到一个字符串中
}

}
