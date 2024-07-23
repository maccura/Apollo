#include "ini_manager.hpp"
#include "global.hpp"
#include <QCoreApplication>
#include <QDir>
namespace ReportDesign
{
ini_manager::ini_manager(QObject *p): QObject(p), m_configIni(nullptr)
{
    init();
}

ini_manager::~ini_manager()
{
    delete m_configIni;
}

void ini_manager::init()
{
    QString  filePath;
    filePath = QCoreApplication::applicationDirPath() + "/config/config.ini";
    m_configIni = new QSettings(filePath, QSettings::IniFormat);//初始化读取Ini文件对象
}

ini_manager::Languages ini_manager::language()
{
    return static_cast<ini_manager::Languages>(m_configIni->value("language/current").toInt());
}

void ini_manager::ini_manager::setLanguage(int index)
{
    m_configIni->setValue("language/current", QString::number(index));
}

bool ini_manager::isCurrentLanguage(QString file, ini_manager::Languages language)
{
    QString flag;
    if (language == ini_manager::chinese)
    {
        flag = LANGUAGE_CHINESE_SUFFIX;
    }
    else if (language == ini_manager::english)
    {
        flag = LANGUAGE_ENGLISH_SUFFIX;
    }
    else if (language == ini_manager::russian)
    {
        flag = LANGUAGE_RUSSIAN_SUFFIX;
    }
    else if (language == ini_manager::japanese)
    {
        flag = LANGUAGE_JAPANESE_SUFFIX;
    }
    else if (language == ini_manager::french)
    {
        flag = LANGUAGE_FRENCH_SUFFIX;
    }
    else if (language == ini_manager::latin)
    {
        flag = LANGUAGE_LATIN_SUFFIX;
    }
    else if (language == ini_manager::german)
    {
        flag = LANGUAGE_GERMAN_SUFFIX;
    }
    else if (language == ini_manager::spanish)
    {
        flag = LANGUAGE_SPANISH_SUFFIX;
    }
    else if (language == ini_manager::italian)
    {
        flag = LANGUAGE_ITALIAN_SUFFIX;
    }

    int lastUnderscore = file.lastIndexOf("_"); // 获取最后一个"_"的位置
    QString result = file.mid(lastUnderscore + 1); // 获取"_"后面的内容
    int dotIndex = result.indexOf("."); // 获取"."的位置
    result = result.left(dotIndex); // 截取"."之前的内容

    if (flag == result)
    {
        return true;
    }

    return false;
}

QString ini_manager::curLangFile(QString filePath, ini_manager::Languages lang)
{
    QDir fileDir(filePath);
    // 获取目录下的所有文件
    QStringList fileList = fileDir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

    for (const QString &file : fileList)
    {
        //根据当前软件的语言拿到对应语言的变量配置文件
        if (isCurrentLanguage(file, lang))
        {
            return fileDir.filePath(file);
        }
    }
    return "";
}

}
