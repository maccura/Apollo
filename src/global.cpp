#include "global.hpp"
#include <QFontMetrics>
#include <QFontDatabase>
#include <QFontInfo>
#include <qDebug>
namespace ReportDesign
{

QColor generateRandomLightColor()
{
    // 定义20种浅色
    static int lightColorsIndex = 0;
    static QList<QColor> lightColors =
    {
        QColor("#FFC0CB"), // 粉红色
        QColor("#FFA07A"), // 浅橙色
        QColor("#FFD700"), // 金色
        QColor("#FFFFE0"), // 浅黄色
        QColor("#ADFF2F"), // 绿黄色
        QColor("#00FFFF"), // 青色
        QColor("#87CEFA"), // 浅蓝色
        QColor("#ADD8E6"), // 浅灰蓝色
        QColor("#E0FFFF"), // 淡青色
        QColor("#F0FFF0"), // 蜜色
        QColor("#FFE4E1"), // 淡红色
        QColor("#FAFAD2"), // 浅黄绿色
        QColor("#FFF5EE"), // 浅黄色
        QColor("#F0E68C"), // 卡其色
        QColor("#EEE8AA"), // 浅黄色
        QColor("#B0C4DE"), // 浅钢蓝色
        QColor("#FFEBCD"), // 浅橙色
        QColor("#F5DEB3")  // 小麦色
    };

    if (++lightColorsIndex >= lightColors.count())
    {
        lightColorsIndex = 0;
    }
    return lightColors[lightColorsIndex];

}

QString extractClassName(QString className)
{
    int startPos = className.lastIndexOf("::");
    if (startPos == -1)
    {
        startPos = 0;
    }
    else
    {
        startPos += 2;
    }
    return className.right(className.length() - startPos);
}

bool isColorDark(QColor color)
{
    qreal darkness = 1 - (0.299 * color.red() + 0.587 * color.green() + 0.114 * color.blue()) / 255;
    if (darkness < 0.5)
    {
        return false;
    }
    else
    {
        return true;
    }
}

bool compareFonts(const QFont &font1, const QFont &font2)
{
    // 比较字体的大小、粗细和斜体属性
    if (font1.pointSize() != font2.pointSize() ||
        font1.bold() !=  font2.bold() ||
        font1.italic() != font2.italic())
    {
        return false;
    }


    // 判断字体大小是否相同
    if (font1.pointSize() != font2.pointSize())
    {
        return false;
    }

    // 判断字体样式是否相同
    if (font1.style() != font2.style())
    {
        return false;
    }

    // 判断字体粗细是否相同
    if (font1.weight() != font2.weight())
    {
        return false;
    }

    // 判断字体斜体是否相同
    if (font1.italic() != font2.italic())
    {
        return false;
    }

    // 判断字体下划线是否相同
    if (font1.underline() != font2.underline())
    {
        return false;
    }

    // 判断字体删除线是否相同
    if (font1.strikeOut() != font2.strikeOut())
    {
        return false;
    }

    // 判断字体间距是否相同
    if (font1.letterSpacing() != font2.letterSpacing())
    {
        return false;
    }

    // 判断字体首选字重是否相同
    if (font1.hintingPreference() != font2.hintingPreference())
    {
        return false;
    }

    // 判断字体是否为粗体
    if (font1.bold() != font2.bold())
    {
        return false;
    }

    // 判断字体是否为等宽字体
    if (font1.fixedPitch() != font2.fixedPitch())
    {
        return false;
    }

    // 判断字体是否为强制大写字母
    if (font1.capitalization() != font2.capitalization())
    {
        return false;
    }

    // 判断字体是否为伸缩字体
    if (font1.stretch() != font2.stretch())
    {
        return false;
    }

    // 判断字体是否为斜体
    if (font1.italic() != font2.italic())
    {
        return false;
    }

    if (font1.family() != font2.family())
    {
        return false;
    }

    if (QFontMetrics(font1).width("test") != QFontMetrics(font2).width("test"))
    {
        return false;
    }

    return true;
}

bool compareItems(QGraphicsItem *item1, QGraphicsItem *item2)
{
    return item1->y() < item2->y();
}

void timerStart(QElapsedTimer &timer)
{
    // 函数开始时创建并启动计时器
    timer.start();
}

void timerStop(QString Fun, QElapsedTimer &timer)
{
    // 函数结束时获取并打印执行时间（以毫秒为单位）
    qint64 elapsed = timer.elapsed();
    qDebug() << QString("yourFunction( %1) took").arg(Fun) << elapsed << " milliseconds to execute.";
}

}
