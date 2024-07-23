#include "design_main_window.hpp"
#include "ini_manager.hpp"
#include "global.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //样式
    QFile qss(":/rc/QSS/style.qss");
    if (qss.open(QFile::ReadOnly))
    {
        a.setStyleSheet(qss.readAll());
    }

    // 语言
    QTranslator myLanguage;
    QString qmFile = ReportDesign::ini_manager::instance().curLangFile(QCoreApplication::applicationDirPath() + ReportDesign::LANGUAGE_FILE_PATH,
                                                                       ReportDesign::ini_manager::instance().language());
    if (myLanguage.load(qmFile))
    {
        a.installTranslator(&myLanguage);
    }

    ReportDesign::design_main_window p;
    p.show();
    return a.exec();
}
