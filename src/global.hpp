/***************************************************************************
*   This file is part of the Andromedae project                           *
*   Copyright (C) 2024 by Mike Medical Electronics Co., Ltd               *
*   Andromedae@maccura.com                                                *
*                                                                         *
**                   GNU General Public License Usage                    **
*                                                                         *
*   This library is free software: you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation, either version 3 of the License, or     *
*   (at your option) any later version.                                   *
*   You should have received a copy of the GNU General Public License     *
*   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
*                                                                         *
**                  GNU Lesser General Public License                    **
*                                                                         *
*   This library is free software: you can redistribute it and/or modify  *
*   it under the terms of the GNU Lesser General Public License as        *
*   published by the Free Software Foundation, either version 3 of the    *
*   License, or (at your option) any later version.                       *
*   You should have received a copy of the GNU Lesser General Public      *
*   License along with this library.                                      *
*   If not, see <http://www.gnu.org/licenses/>.                           *
*                                                                         *
*   This library is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
****************************************************************************/
#ifndef GLOBAL_H
#define GLOBAL_H
#include <QString>
#include <QColor>
#include <QSize>
#include <QMargins>
#include <QFont>
#include <QGraphicsItem>
#include <QElapsedTimer>
namespace ReportDesign
{
// 属性名称
#define PROPERTY_TO_STR(PROPERTY)           #PROPERTY
#define PROPERTY_TO_STRING(PROPERTY)        PROPERTY_TO_STR(PROPERTY)
#define PAGE_SIZE_PROPERTY                  pageSize
#define PAGE_ORIENTATION_PROPERTY           pageOrientation
#define ITEM_ALIGNMENT_PROPERTY             itemAlignment
#define GEOMETRY_PROPERTY                   geometry
#define GEOMETRY_LOCKED_PROPERTY            geometryLocked
#define FONT_COLOR_PROPERTY                 fontColor
#define FONT_PROPERTY                       font
#define ALIGNMENT_PROPERTY                  alignment
#define BORDERLINESIZE_PROPERTY             borderLineSize
#define BORDERCOLOR_PROPERTY                borderColor
#define BORDER_PROPERTY                     borders
#define SHAPE_PROPERTY                      shape
#define MASTERMASK_PROPERTY                 masterMask
#define MASTERMASK_ITEM_ID_PROPERTY         masterMaskItemID

//表格
#define TABLE_ROW_COUNT_PROPERTY            dataRowCount
#define TABLE_ROW_HEIGHT_PROPERTY           rowHeight
#define TABLE_COLUMN_COUNT_PROPERTY         dataColumnCount
#define TABLE_COLUMN_WIDTH_PROPERTY         columnWidth
#define TABLE_SHOW_GRID_PROPERTY            showGrid
#define TABLE_SHOW_HEADER_PROPERTY          showHeader
#define TABLE_UNDER_LINE_PROPERTY           underLine
#define TABLE_AUTO_FILL_PROPERTY            autoFill

// 这两个表格属性决定了，是否被锁定
#define TABLE_TABLEITEM_ID_PROPERTY         tableItemID
#define TABLE_CONNECTTABLEITEMID_PROPERTY   connectTableItemID

// 画板背景
#define ARTBOARD_BACKGROUND_PROPERTY        artboardBackground


const int  DEFAULT_GRID_STEP = 1;
const int  RESIZE_HANDLE_SIZE = 5;
const int  SELECTION_PEN_SIZE = 1;
const int  MINIMUM_ITEM_WIDTH = 2 * RESIZE_HANDLE_SIZE;
const int  MINIMUM_ITEM_HEIGHT = 2 * RESIZE_HANDLE_SIZE;
const double  RESIZE_ZONE_OPACITY = 0.5;
const double  SELECTED_RESIZE_ZONE_OPACITY = 0.6;
const QColor  MC_BLUE_COLOR = QColor(0, 150, 255);
const QColor  PAGE_BORDER_COLOR = QColor(244, 195, 195);
const QColor  PAGE_GRID_COLOR = Qt::darkGray;

const Qt::GlobalColor  RESIZE_ZONE_COLOR = Qt::green;
// 选择框颜色
const QColor  SELECTION_COLOR = QColor(131, 203, 158);
const QColor  HOVER_RECT_COLOR = QColor(255, 118, 49);
const Qt::GlobalColor  JOIN_COLOR = Qt::blue;
const QColor VIEW_BG_COLOR =  QColor(235, 236, 237);
const QColor BUTTON_HOVER_COLOR =  QColor(114, 201, 146, 0.7);


const double  SELECTION_COLOR_OPACITY = 0.6;


const qreal fontFACTOR = 3.5;
const int mmFACTOR = 10;
const int itemPaleteIconSize = 24;
const qreal minSpaceBorder = 10;
const QString bandTAG = "band";
const Qt::GlobalColor BAND_NAME_LABEL_COLOR = Qt::yellow;
const Qt::GlobalColor BAND_NAME_BORDER_COLOR = Qt::darkYellow;
const qreal BAND_MARKER_OPACITY = 1;
const qreal LAYOUT_MARKER_OPACITY = 0.3;
const qreal BAND_NAME_AREA_OPACITY = 0.3;
const qreal BAND_NAME_TEXT_OPACITY = 0.6;
const qreal SELECTION_OPACITY = 0.3;
const QString FIELD_RX = "\\$D\\s*\\{\\s*([^{}]*)\\s*\\}";
const QString VARIABLE_RX = "\\$V\\s*\\{\\s*(?:([^\\{\\},]*)|(?:([^\\{\\}]*)\\s*,\\s*([^\\{\\}]*)))\\s*\\}";
const QString NAMED_VARIABLE_RX = "\\$V\\s*\\{\\s*(?:(%1)|(?:(%1)\\s*,\\s*([^\\{\\}]*)))\\s*\\}";
const QString SCRIPT_RX = "\\$S\\s*\\{(.*)\\}";
const QString GROUP_FUNCTION_PARAM_RX = "\\(\\s*((?:(?:\\\")|(?:))(?:(?:\\$(?:(?:D\\{\\s*\\w*..*\\})|(?:V\\{\\s*\\w*\\s*\\})|(?:S\\{.+\\})))|(?:\\w*))(?:(?:\\\")|(?:)))(?:(?:\\s*,\\s*(?:\\\"(\\w*)\\\"))|(?:))(?:(?:\\s*,\\s*(?:(\\w*)))|(?:))\\)";
const int DATASOURCE_INDEX = 3;
const int VALUE_INDEX = 2;
const int EXPRESSION_ARGUMENT_INDEX = 1;

const QString GROUP_FUNCTION_RX = "(%1\\s*" + GROUP_FUNCTION_PARAM_RX + ")";
const QString GROUP_FUNCTION_NAME_RX = "%1\\s*\\((.*[^\\)])\\)";
const int SCENE_MARGIN = 50;
const QString FUNCTION_MANAGER_NAME = "ReportDesign";
const QString DATAFUNCTIONS_MANAGER_NAME = "DatasourceFunctions";
const QString EOW("~!@#$%^&*()+{}|:\"<>?,/;'[]\\-=");
const int DEFAULT_TAB_INDENTION = 4;
const int DOCKWIDGET_MARGINS = 4;

const char SCRIPT_SIGN = 'S';
const char FIELD_SIGN = 'D';
const char VARIABLE_SIGN = 'V';

// 在元件工具栏拖动元件进行创建时，作为在工厂中查找元件构造器的索引
const QString TEXT_ITEM_TYPE = "TextItem";
const QString GROUP_TEXT_ITEM_TYPES = "GroupTextItem";
const QString SHAPE_ITEM_TYPE = "ShapeItem";
const QString TABLE_ITEM_TYPE = "TableItem";
const QString COL_HEADER_TABLE_ITEM_TYPE = "ColHeaderTableItem";
const QString SVG_ITEM_TYPE = "SVGItem";
const QString IMAGE_ITEM_TYPE = "ImageItem";
const QString PAGENUMBER_ITEM_TYPE = "PageNumber";
const QString WATERMARK_ITEM_TYPE = "WatermarkItem";
const QSize ITEM_SIZE = QSize(200, 50);

// 页眉页脚元件颜色
const int PAGE_TOP_ID = 1;
const int PAGE_BOTTOM_ID = 2;
const QColor PAGE_TOP_COLOR = QColor(131, 203, 158);
const QColor PAGE_BOTTOM_COLOR = QColor(255, 118, 49);
const int PAGE_MARGIN = 5;

// 复制元件时，副本元件位置的偏移量
const int COPY_POS_OFFSET = 20;

// 视图缩放比例
const qreal ZOOM_IN_UNIT = 1.2;
const qreal ZOOM_OUT_UNIT = 1 / 1.2;

const qreal MINSCALE  =  0.15;
const qreal MAXSCALE  = 1.4;

// 元件变量相关
const QString TEXT_VARIANT_TYPE = "";
const QString NM_VARIANT_TYPE = "";
const QString TABLE_VARIANT_TYPE = "";
const QString IMAGE_DATA_VARIANT_TYPE = "";
const QString IMAGE_PATH_VARIANT_TYPE = "";
const QString IMAGE_GZBS_VARIANT_TYPE = "";
const QString GROUP_VARIANT_TYPE = "";
const QString DEVICE_JSON_PATH =  "";
const QString DEFAULT_MODULE_FILE_PATH = "";
const QString LANGUAGE_FILE_PATH = "";
const QString TABELITEMUUSTR = "";

enum RenderPass {FirstPass = 1, SecondPass = 2};

QString extractClassName(QString className);

// 产生随机浅色系颜色
QColor generateRandomLightColor();

class  ReportSettings
{
public:
    ReportSettings(): m_suppressAbsentFieldsAndVarsWarnings(false) {}
    void setDefaultValues()
    {
        m_suppressAbsentFieldsAndVarsWarnings = false;
    }
    bool suppressAbsentFieldsAndVarsWarnings() const;
    void setSuppressAbsentFieldsAndVarsWarnings(bool suppressAbsentFieldsAndVarsWarnings);
private:
    bool m_suppressAbsentFieldsAndVarsWarnings;
};

class  ReportError : public std::runtime_error
{
public:
    ReportError(const QString &message);
};

bool isColorDark(QColor color);
bool compareFonts(const QFont &font1, const QFont &font2);
bool compareItems(QGraphicsItem *item1, QGraphicsItem *item2);
void timerStart(QElapsedTimer &timer);
void timerStop(QString Fun, QElapsedTimer &timer);

// 函数耗时测试
#define  TIMER_START                QElapsedTimer timer; \
    timerStart(timer);

#define  TIMER_STOP(FUN_NAME)       timerStop(FUN_NAME, timer);

const QMargins PROP_WIDGET_MARGINS = QMargins(10, 17, 5, 0);
const QString  LANGUAGE_CHINESE_SUFFIX  =  "zh";
const QString  LANGUAGE_ENGLISH_SUFFIX  =  "en";
const QString  LANGUAGE_JAPANESE_SUFFIX =  "ja";
const QString  LANGUAGE_RUSSIAN_SUFFIX  =  "ru";
const QString  LANGUAGE_FRENCH_SUFFIX   =  "fr";
const QString  LANGUAGE_LATIN_SUFFIX    =  "la";
const QString  LANGUAGE_GERMAN_SUFFIX   =  "de";
const QString  LANGUAGE_SPANISH_SUFFIX  =  "es";
const QString  LANGUAGE_ITALIAN_SUFFIX  =  "it";

// 渲染拓展类型
const QString  REPEAT_RENDER = "REPEAT_RENDER";

// 打印数据协议版本
const QString PRINTDATA_VERSION = "1.0";

const QString SOFTWARE_VERSION = "1";

}
#endif // GLOBAL_H
