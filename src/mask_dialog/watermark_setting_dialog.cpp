#include "watermark_setting_dialog.hpp"
#include "base_push_button.hpp"
#include <QFileDialog>
#include <QImageReader>
#define IMAGELOADTEXT  "导入图片"
namespace ReportDesign
{
watermark_setting_dialog::watermark_setting_dialog(QWidget *parent): base_mask_dialog(parent),
    m_cancel(new base_push_button(this)), m_ok(new gbase_push_button(this)), m_textRadioButton(nullptr),
    m_imageRadioButton(nullptr), m_textEdit(new QTextEdit(this)), m_loadImage(new QPushButton(this))
{
    init();
}

watermark_setting_dialog::~watermark_setting_dialog()
{

}

void watermark_setting_dialog::init()
{
    setFixedSize(650, 300);
    SetTitle(tr("watermark setting"));

    m_textRadioButton = new image_Radio_button(this, ":/watermask_radio", ":/watermask_radio", ":/watermask_radio_clieck");
    m_textRadioButton->setChecked(true);
    connect(m_textRadioButton, &QRadioButton::clicked, [this]
    {
        m_textEdit->show();
        m_loadImage->hide();
    });
    base_Label *textLable = new base_Label(tr("text watermark"), this);
    textLable->setFixedWidth(150);
    //textLable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 设置大小策略为自动扩展
    // 调整大小以适应内容
    //textLable->adjustSize();

    m_imageRadioButton = new image_Radio_button(this, ":/watermask_radio", ":/watermask_radio", ":/watermask_radio_clieck");
    connect(m_imageRadioButton, &QRadioButton::clicked, [this]
    {
        m_textEdit->hide();
        m_loadImage->show();
    });
    base_Label *imageLable = new base_Label(this);
    imageLable->setFixedWidth(150);
    imageLable->setText(tr("image watermark")),
               imageLable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // 设置大小策略为自动扩展
    // 调整大小以适应内容
    imageLable->adjustSize();

    m_textEdit->setFixedSize(610, 110);
    m_textEdit->setPlaceholderText(tr("enter watermark text"));
    m_loadImage->setFixedSize(610, 110);
    m_loadImage->setObjectName("watermarkImageImport");
    m_loadImage->setText(tr("Import picture"));
    m_loadImage->hide();
    connect(m_loadImage, &QPushButton::clicked, [this]
    {
        // 获取Qt支持的所有图片格式
        QList<QByteArray> supportedFormats = QImageReader::supportedImageFormats();

        // 构建文件过滤器
        QString filter = "Images (";
        foreach (const QByteArray &format, supportedFormats)
        {
            filter += "*." + format + " ";
        }
        filter += ")";
        QString fileName = QFileDialog::getOpenFileName(this, tr("file selection"), "", filter.toStdString().c_str());
        if (!fileName.isEmpty())
        {
            m_loadImage->setText(fileName);
        }
    });

    // 按钮
    m_cancel->setText(tr("cancle"));
    m_cancel->setFixedSize(110, 44);
    connect(m_cancel, &QPushButton::clicked, [this]
    {
        close();
    });

    m_ok->setText(tr("ok"));
    m_ok->setFixedSize(110, 44);
    connect(m_ok, &QPushButton::clicked, this, &watermark_setting_dialog::slotOk);

    QVBoxLayout *Vlayout = VLayout();
    QHBoxLayout *Hlayout = new QHBoxLayout();
    Vlayout->addLayout(Hlayout);
    Hlayout->addWidget(m_textRadioButton);
    Hlayout->addWidget(textLable);
    Hlayout->addSpacing(69);
    Hlayout->addWidget(m_imageRadioButton);
    Hlayout->addWidget(imageLable);
    Hlayout->addStretch();

    Vlayout->addWidget(m_textEdit);
    Vlayout->addWidget(m_loadImage);

    Hlayout = new QHBoxLayout();
    Vlayout->addSpacing(15);
    Vlayout->addLayout(Hlayout);
    Hlayout->addStretch();
    Hlayout->addWidget(m_cancel);
    Hlayout->addWidget(m_ok);
    Hlayout->addSpacing(12);
    Vlayout->addSpacing(15);
}

void watermark_setting_dialog::slotOk()
{
    QString text = m_textEdit->toPlainText();
    QString imagePath = m_loadImage->text();
    if (m_textRadioButton->isChecked() && !text.isEmpty())
    {
        watermarkSetting(text, "");
    }

    if (m_imageRadioButton->isChecked() && imagePath != tr("Import picture"))
    {
        watermarkSetting("", imagePath);
    }
    close();
}

}
