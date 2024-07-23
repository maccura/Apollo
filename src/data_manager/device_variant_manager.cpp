#include "device_variant_manager.hpp"
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QCoreApplication>
#include <QMetaEnum>
#include <QObject>
#include <QTextStream>
#include <QTextCodec>
#include "global.hpp"
#include "ini_manager.hpp"
#define  IMAGE_BS_PATH              "/config/simulated_data/Image.jpeg"
#define  IMAGE_GZBS                 "/config/simulated_data/Image.jpeg.gz"

namespace ReportDesign
{
QStringList device_variant_manager::deviceList(QString filePath, ini_manager::Languages language)
{
    m_language = language;
    QStringList deviceList;
    if (m_deviceType_deviceFile_map.empty())
    {
        QDir Dir(filePath);
        Dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        // 获取目录下的所有文件
        QFileInfoList list = Dir.entryInfoList(); // 获取文件夹信息列表
        // 遍历文件和子目录
        for (const QFileInfo &info : list)
        {
            QString deviceType = getDeviceIDFromfile(info.filePath());
            if (!deviceType.isEmpty())
            {
                deviceList.append(deviceType);
            }
        }
    }
    else
    {
        for (auto it = m_deviceType_deviceFile_map.begin(); it != m_deviceType_deviceFile_map.end(); ++it)
        {
            deviceList.append(it.key());
        }
    }

    return deviceList;
}

bool device_variant_manager::isCurrentLanguage(QString filePath)
{
    QString flag;
    if (m_language == ini_manager::chinese)
    {
        flag = LANGUAGE_CHINESE_SUFFIX;
    }
    else if (m_language == ini_manager::english)
    {
        flag = LANGUAGE_ENGLISH_SUFFIX;
    }
    else if (m_language == ini_manager::russian)
    {
        flag = LANGUAGE_RUSSIAN_SUFFIX;
    }
    else if (m_language == ini_manager::japanese)
    {
        flag = LANGUAGE_JAPANESE_SUFFIX;
    }
    else if (m_language == ini_manager::french)
    {
        flag = LANGUAGE_FRENCH_SUFFIX;
    }
    else if (m_language == ini_manager::latin)
    {
        flag = LANGUAGE_LATIN_SUFFIX;
    }
    else if (m_language == ini_manager::german)
    {
        flag = LANGUAGE_GERMAN_SUFFIX;
    }
    else if (m_language == ini_manager::spanish)
    {
        flag = LANGUAGE_SPANISH_SUFFIX;
    }
    else if (m_language == ini_manager::italian)
    {
        flag = LANGUAGE_ITALIAN_SUFFIX;
    }

    int lastUnderscore = filePath.lastIndexOf("_"); // 获取最后一个"_"的位置
    QString result = filePath.mid(lastUnderscore + 1); // 获取"_"后面的内容
    int dotIndex = result.indexOf("."); // 获取"."的位置
    result = result.left(dotIndex); // 截取"."之前的内容

    if (flag == result)
    {
        return true;
    }

    return false;
}

QString device_variant_manager::getDeviceIDFromfile(QString Dir)
{
    QDir fileDir(Dir);
    // 获取目录下的所有文件
    QStringList fileList = fileDir.entryList(QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

    for (const QString &file : fileList)
    {
        //根据当前软件的语言拿到对应语言的变量配置文件
        if (isCurrentLanguage(file))
        {
            QString filefullPath =  fileDir.filePath(file);
            QString deviceType = device(filefullPath);
            if (!filefullPath.isEmpty() &&
                !deviceType.isEmpty())
            {
                m_deviceType_deviceFile_map.insert(deviceType, filefullPath);
            }
            return deviceType;
        }
    }
    return "";
}

QString device_variant_manager::device(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        return "";
    }

    QByteArray jsonData =  file.readAll();
    QJsonDocument document = QJsonDocument::fromJson(jsonData);
    if (!document.isObject())
    {
        return "";
    }

    QJsonObject object = document.object();
    if (!object.contains("device"))
    {
        return "";
    }

    return object.value("device").toString();
}

// 根据设备加载对应变量文件
bool device_variant_manager::loadDeviceJson(QString device)
{
    if (m_deviceType_deviceFile_map.contains(device))
    {
        QString filePath = m_deviceType_deviceFile_map.value(device);
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly))
        {
            return false;
        }

        QByteArray jsonData = file.readAll();
        QJsonDocument document = QJsonDocument::fromJson(jsonData);
        if (!formatVerification(document))
        {
            return false;
        }

        m_jsonDoc = document;
        m_deviceID = device;
        return true;
    }
    return false;
}

QString device_variant_manager::DeviceJson()
{
    if (!m_jsonDoc.isEmpty())
    {
        return m_jsonDoc.toJson().toStdString().c_str();
    }
    return "";
}

bool device_variant_manager::formatVerification(QJsonDocument jsonDoc)
{
    if (!jsonDoc.isObject())
    {
        return false;
    }

    QJsonObject object = jsonDoc.object();
    if (!object.contains("print_module_ds_list") ||
        !object.contains("device"))
    {
        return false;
    }

    if (!object.value("print_module_ds_list").isArray())
    {
        return false;
    }

    return true;
}

// 返回当前设备文件中的模块列表
QList<variant_info> device_variant_manager::moduleList()
{
    QList<variant_info> variantList;
    if (m_jsonDoc.isEmpty())
    {
        return QList<variant_info>();
    }
    QJsonObject object = m_jsonDoc.object();

    if (!object.value("print_module_ds_list").isArray())
    {
        return QList<variant_info>();
    }

    QJsonArray module_ds_list = object.value("print_module_ds_list").toArray();
    for (const QJsonValue &it : module_ds_list)
    {
        if (it.isObject() && !it.toObject().keys().isEmpty())
        {
            QString module = it.toObject().keys()[0];
            QJsonValue moduleValue = it.toObject().value(module);
            if (moduleValue.isArray())
            {
                QJsonArray moduleArr = moduleValue.toArray();
                // 描述信息
                if (moduleArr.at(0).isString())
                {
                    variant_info variant;
                    variant.m_ID = module;
                    variant.m_name = moduleArr.at(0).toString();
                    variantList.push_back(variant);
                }
            }
        }
    }
    return variantList;
}

base_homogeneous_variant *device_variant_manager::HomogeneousVariant(QString module, VariantType variantType)
{
    QString moduleJson;
    if (m_jsonDoc.isEmpty())
    {
        return nullptr;
    }

    QJsonObject object = m_jsonDoc.object();
    if (!object.value("print_module_ds_list").isArray())
    {
        return nullptr;
    }

    QJsonArray module_ds_list = object.value("print_module_ds_list").toArray();
    for (const QJsonValue &it : module_ds_list)
    {
        if (it.isObject())
        {
            QStringList keyList = it.toObject().keys();
            if (!keyList.isEmpty() &&
                keyList[0] ==  module)
            {
                moduleJson = QJsonDocument(it.toObject()).toJson();
                break;
            }
        }
    }

    if (!moduleJson.isEmpty() &&
        m_homogeneous_variant_map.contains(variantType))
    {
        return  m_homogeneous_variant_map[variantType](moduleJson);
    }

    return nullptr;
}

bool  device_variant_manager::registerVariant(VariantType variantType, Creator  creator)
{
    if (creator == nullptr)
    {
        return false;
    }

    if (!m_homogeneous_variant_map.contains(variantType))
    {
        m_homogeneous_variant_map.insert(variantType, creator);
    }

    return true;
}

QString device_variant_manager::genSimulatedData(QString module)
{
    if (module.isEmpty())
    {
        return "";
    }

    QJsonObject jsonObject;
    for (int i = 0; i <= Group; ++i)
    {
        VariantType variantType = static_cast<VariantType>(i);
        base_homogeneous_variant *homogeneous_variant = HomogeneousVariant(module, variantType);
        if (!homogeneous_variant)
        {
            continue;
        }
        QVector<variant_info> vecVariant = homogeneous_variant->variant();

        // 处理 Text 类型
        if (variantType == Text)
        {
            for (const variant_info &it : vecVariant)
            {
                if (it.m_type == TEXT_VARIANT_TYPE)
                {
                    jsonObject[it.m_ID] = QObject::tr("Text data");
                }
                else if (it.m_type == NM_VARIANT_TYPE)
                {
                    jsonObject[it.m_ID] = 66;
                }
            }
        }
        // 处理 Image 类型
        else if (variantType == Image)
        {
            for (const variant_info &it : vecVariant)
            {
                // 二进制转base64
                if (it.m_type == IMAGE_DATA_VARIANT_TYPE)
                {
                    jsonObject[it.m_ID] = imageSimulatedBa64File(QCoreApplication::applicationDirPath() + IMAGE_BS_PATH);
                }
                // 图片路径
                if (it.m_type == IMAGE_PATH_VARIANT_TYPE)
                {
                    jsonObject[it.m_ID] = QCoreApplication::applicationDirPath() + IMAGE_BS_PATH;
                }
                // 图片压缩包
                if (it.m_type == IMAGE_GZBS_VARIANT_TYPE)
                {
                    jsonObject[it.m_ID] = imageSimulatedBa64File(QCoreApplication::applicationDirPath() + IMAGE_GZBS);
                }
            }
        }
        // 处理 Table 类型
        else if (variantType == Table)
        {
            // 多少个表
            for (const variant_info &it : vecVariant)
            {
                QJsonObject tableJson;
                QJsonArray tableHeader;
                QJsonArray tableData;
                QVector<variant_info> TableColVariant = homogeneous_variant->childVariants(it.m_ID);
                // 单个表中有多少个变量（表头）
                for (const variant_info &ColVariant : TableColVariant)
                {
                    tableHeader.push_back(ColVariant.m_ID);
                }

                // 表格5行数据
                for (int i = 0; i < 50; ++i)
                {
                    //QString str = "161_sd LDL-C、307_IgM、255_CO2、002_ALT、202_α-HBDH、022_CG、353_LPS、054_Crea Gen.2、103_HbAlc、318_κ(U)、309_C4、009_ALB、066_UA Gen.2、251_Ca、052_UA、317_κ、106_GA、319_λ、012_TBA、900_S.IND、303_CRP、453_HAPT、110_APN、204_CK-MB、016_ChE、104_FMN、304_hs-CRP、107_LAC、320_λ(U)、354_P-AMY、101_Glu(HK)、020_LAP、258_Cu、151_TG、024_GLDH、056_β2-MG、055_mALB、316_RF Gen.2、023_mAST、259_Zn、257_IRON、006_γ-GT、058_Cys C、205_LDH1、312_SAA、005_ALP、008_TP、019_MAO、060_α1-MG、256_CO2(C)、067_uTRF、452_UIBC、108_β-HB、301_ASO、253_P、062_NGAL、206_H-FABP、017_PA、063_RBP、162_HCY(C）、313_IgE、065_NAG、203_CK、163_Lp-PLA2、010_TBil、004_AST、501_PGⅠ、057_β2-MG(U)、021_AMM、154_LDL-C、310_anti-CCP、260_Ca Gen.2、059_Cys C(C)、053_Crea、051_Urea、011_DBil、155_ApoA1、454_TRF、156_ApoB、152_TC、061_CSF/U-TP、158_Hcy、153_HDL-C、305_IgG、306_IgA、064_RBP(U)、015_ADA、252_Mg、201_LD、451_Ferritin、160_ApoE、502_PGⅡ、157_Lp(a)、014_AFU、018_5'-NT、302_RF、315_ASO Gen.2、308_C3、351_AMY、111_SA、102_Glu(GOD)、314_MMP-3、105_GA-ALB、159_NEFA";
                    //str = "Table data";
                    QJsonArray tableDataItem;
                    for (int j = 0; j < tableHeader.count(); ++j)
                    {
                        tableDataItem.push_back(QObject::tr("Table data") + QString::number(i));
                        //tableDataItem.push_back(QString::number(i) + "|  "+str );
                    }
                    tableData.push_back(tableDataItem);
                }
                tableJson["header"] = tableHeader;
                tableJson["items"] = tableData;
                jsonObject[it.m_ID] = tableJson;
            }
        }
        else if (variantType == Group)
        {
            int blockDataNum = 5;
            // 多少个组
            for (const variant_info &it : vecVariant)
            {
                // 目前一个数组中只会有一个json对象
                QJsonArray groupArr;
                QJsonObject groupJsonObject;
                for (int j = 0; j <= Table; ++j)
                {
                    VariantType groupVariantType = static_cast<VariantType>(j);
                    base_homogeneous_variant *group_variant = homogeneous_variant->typeChildVariants(it.m_ID, groupVariantType);
                    if (group_variant)
                    {
                        QVector<variant_info> gourpVecVariant = group_variant->variant();
                        if (groupVariantType == Text)
                        {
                            for (const variant_info &it : gourpVecVariant)
                            {
                                QJsonArray gourpItemArr;
                                for (int n = 0; n < blockDataNum; n++)
                                {
                                    if (it.m_type == TEXT_VARIANT_TYPE)
                                    {
                                        QString str(QObject::tr("Text data"));
                                        str += QString("(%1 %2)").arg("group").arg(n);
                                        gourpItemArr.push_back(str);
                                    }
                                    else if (it.m_type == NM_VARIANT_TYPE)
                                    {
                                        gourpItemArr.push_back(66);
                                    }
                                }
                                groupJsonObject[it.m_ID] = gourpItemArr;
                            }
                        }
                        else if (groupVariantType == Image)
                        {
                            for (const variant_info &it : gourpVecVariant)
                            {
                                QJsonArray gourpItemArr;
                                for (int n = 0; n < blockDataNum; n++)
                                {
                                    // 二进制转base64
                                    if (it.m_type == IMAGE_DATA_VARIANT_TYPE)
                                    {
                                        gourpItemArr.push_back(imageSimulatedBa64File(QCoreApplication::applicationDirPath() + IMAGE_BS_PATH));
                                    }
                                    // 图片路径
                                    if (it.m_type == IMAGE_PATH_VARIANT_TYPE)
                                    {
                                        gourpItemArr.push_back(QCoreApplication::applicationDirPath() + IMAGE_BS_PATH);
                                    }
                                    // 图片压缩包
                                    if (it.m_type == IMAGE_GZBS_VARIANT_TYPE)
                                    {
                                        gourpItemArr.push_back(imageSimulatedBa64File(QCoreApplication::applicationDirPath() + IMAGE_GZBS));
                                    }
                                }
                                groupJsonObject[it.m_ID] = gourpItemArr;
                            }
                        }
                        else if (groupVariantType == Table)
                        {
                            // 多少个组
                            for (const variant_info &it : gourpVecVariant)
                            {
                                QJsonObject tableJson;
                                QJsonArray tableHeader;
                                QJsonArray tableData;
                                QVector<variant_info> TableColVariant = group_variant->childVariants(it.m_ID);
                                // 单个表中有多少个变量（表头）
                                for (const variant_info &ColVariant : TableColVariant)
                                {
                                    tableHeader.push_back(ColVariant.m_ID);
                                }

                                // 一个表格中有多少块数据，对应元件
                                for (int n = 0; n < blockDataNum; n++)
                                {
                                    // 一个表格中 20行的数据
                                    QJsonArray gorpTableItem;
                                    for (int i = 0; i < 20; ++i)
                                    {
                                        // 一行的数据
                                        QJsonArray tableDataItem;
                                        for (int j = 0; j < tableHeader.count(); ++j)
                                        {
                                            QString str(QObject::tr("Table data"));
                                            str += QString("%1(%2 %3)").arg(i).arg("group").arg(n);
                                            tableDataItem.push_back(str);
                                        }
                                        gorpTableItem.push_back(tableDataItem);
                                    }
                                    tableData.push_back(gorpTableItem);
                                }
                                tableJson["header"] = tableHeader;
                                tableJson["items"] = tableData;
                                groupJsonObject[it.m_ID] = tableJson;
                            }
                        }
                    }
                }
                groupArr.push_back(groupJsonObject);
                jsonObject[it.m_ID] = groupArr;
            }
        }
    }
    return QString::fromUtf8(QJsonDocument(jsonObject).toJson());
}

QString  device_variant_manager::imageSimulatedBa64File(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
    {
        return QString();
    }

    QByteArray imageData = file.readAll();
    file.close();

    QByteArray base64Data = imageData.toBase64();
    QString base64String(base64Data);

    return base64String;
}

}
