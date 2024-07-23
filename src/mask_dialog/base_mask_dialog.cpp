#include "base_mask_dialog.hpp"
#include <QMenuBar>
#include <QFrame>
#include <QPushButton>
#include <QPainter>
#include <QLabel>
#include <QToolButton>
namespace ReportDesign
{
base_mask_dialog::base_mask_dialog(QWidget *parent, bool menuBar, bool mask) : QDialog(parent),
    m_mask(nullptr), m_VLayout(new QVBoxLayout(this))
{
    setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint | Qt::CustomizeWindowHint
                   | Qt::WindowTitleHint | Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setContentsMargins(3, 3, 3, 3);
    if (mask)
    {
        m_mask =  new dialog_background(parent);
    }

    if (menuBar)
    {
        QPalette palette;
        //设置主窗口背景颜色
        palette.setColor(QPalette::Window, QColor(255, 255, 255));
        this->setPalette(palette);

        m_menuBar = new QMenuBar(this);
        m_closeToolBar = new QToolBar(this); // 创建一个工具栏
        m_closeToolBar->setContentsMargins(2, 2, 2, 2);
        m_closeToolBar->setMovable(false); // 设置QToolBar不可移动
        m_closeToolBar->resize(width(), 50);
        m_closeWindow = new QAction(this);
        m_closeWindow->setIcon(QIcon(":/close"));
        connect(m_closeWindow, &QAction::triggered, [this]
        {
            close();
        });

        m_closeToolBar->addAction(m_closeWindow); // 将动作添加到工具栏中
        QLabel *button = new QLabel(this);
        // 设置按钮的样式表，将右边留出10个像素的空白距离
        button->setFixedSize(10, 3);
        // 将按钮添加到工具栏中
        m_closeToolBar->addWidget(button);
        m_menuBar->setCornerWidget(m_closeToolBar, Qt::TopRightCorner);

        m_TitleWindow = new QAction(tr("title"), this);
        m_TitleWindow->setEnabled(false);

        // 创建自定义字体对象
        QFont font;
        font.setFamily("HarmonyOS Sans SC Medium");
        font.setPointSize(14);

        m_TitleWindow->setFont(font);
        m_menuBar->addAction(m_TitleWindow);

        // 在对话框布局中添加菜单栏和工具栏
        m_VLayout->setMenuBar(m_menuBar);
    }
}

void base_mask_dialog::SetTitle(QString Title)
{
    m_closeToolBar->resize(width(), 50);
    m_TitleWindow->setText(" " + Title);
}

void base_mask_dialog::closeEvent(QCloseEvent *event)
{
    if (m_mask)
    {
        m_mask->hide();
    }
    QDialog::closeEvent(event);
}

void base_mask_dialog::paintEvent(QPaintEvent *event)
{
    if (m_mask)
    {
        m_mask->show();
    }


    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // 绘制圆角矩形
    QPainterPath path;
    path.addRoundedRect(rect(), 4, 4);
    painter.fillPath(path, QColor(0xeb, 0xec, 0xed));

    QPainterPath path2;
    path2.addRoundedRect(rect().adjusted(1, 1, -1, -1), 4, 4);
    painter.fillPath(path2, Qt::white);

    // 标题栏下的横线
    painter.setPen(QColor(0xe4, 0xe7, 0xec));
    painter.drawLine(QPoint(0, m_closeToolBar->height()), QPoint(width(), m_closeToolBar->height()));

    QDialog::paintEvent(event);
}

base_mask_dialog::~base_mask_dialog()
{
    if (m_mask)
    {
        m_mask->deleteLater();
    }
}

QVBoxLayout *base_mask_dialog::VLayout()
{
    return m_VLayout;
}


base_mask_widget::base_mask_widget(QWidget *parent, bool menuBar, bool mask) : QWidget(parent),
    m_mask(nullptr), m_VLayout(new QVBoxLayout(this))
{
    if (mask)
    {
        m_mask =  new dialog_background(parent);
    }

    if (menuBar)
    {
        QPalette palette;
        //设置主窗口背景颜色
        palette.setColor(QPalette::Window, QColor(255, 255, 255));
        this->setPalette(palette);

        m_menuBar = new QMenuBar(this);
        m_closeToolBar = new QToolBar(this); // 创建一个工具栏
        m_closeToolBar->setMovable(false); // 设置QToolBar不可移动
        m_closeToolBar->resize(width(), 50);
        m_closeWindow = new QAction(tr("close"), this);
        m_closeWindow->setIcon(QIcon(":/close"));
        connect(m_closeWindow, &QAction::triggered, [this] {close();});
        m_closeToolBar->addAction(m_closeWindow); // 将动作添加到工具栏中
        m_menuBar->setCornerWidget(m_closeToolBar, Qt::TopRightCorner);

        m_TitleWindow = new QAction(tr("title"), this);
        m_TitleWindow->setEnabled(false);
        m_menuBar->addAction(m_TitleWindow);

        // 在对话框布局中添加菜单栏和工具栏
        m_VLayout->setMenuBar(m_menuBar);
    }
    setLayout(m_VLayout);
}

void base_mask_widget::SetTitle(QString Title)
{
    m_closeToolBar->resize(width(), 50);
    m_TitleWindow->setText(" " + Title);
}

void base_mask_widget::closeEvent(QCloseEvent *event)
{
    if (m_mask)
    {
        m_mask->hide();
    }
    QWidget::closeEvent(event);
}

void base_mask_widget::paintEvent(QPaintEvent *event)
{
    if (m_mask)
    {
        m_mask->show();
    }
    QWidget::paintEvent(event);
}

base_mask_widget::~base_mask_widget()
{
    if (m_mask)
    {
        m_mask->deleteLater();
    }
}

QVBoxLayout *base_mask_widget::VLayout()
{
    return m_VLayout;
}

}
