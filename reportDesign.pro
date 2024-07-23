#-------------------------------------------------
#
# Project created by QtCreator 2023-05-18T11:48:05
#
#-------------------------------------------------

QT       += core gui
QT       += printsupport
QT       += xml
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = reportDesigner
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

INCLUDEPATH += $$PWD/src
Debug:DESTDIR = $$PWD/target/Debug
Release:DESTDIR = $$PWD/target/Release

QMAKE_CXXFLAGS_WARN_ON -= -w34100
QMAKE_CXXFLAGS_WARN_ON -= -w34189
# mingw不支持该cmd,msvc编译器支持
#QMAKE_CXXFLAGS_WARN_ON += -wd4819

win32{
    # 编译后执行bat文件,只有源文件修改后（重新编译后）才会执行此句
    # QMAKE_POST_LINK += $$system($$PWD/pre_build.bat)
    # message($$PWD)
}

SOURCES += \
        src/main.cpp \
    src/design_main_window.cpp \
    src/design_widget.cpp \
    src/global.cpp \
    src/item/page_scene.cpp \
    src/item/page_item.cpp \
    src/item/base_design_item.cpp \
    src/item/page_view.cpp \
    src/item/text_item.cpp \
    src/serializators/lrxmlbasetypesserializators.cpp \
    src/serializators/lrxmlqrectserializator.cpp \
    src/serializators/lrxmlreader.cpp \
    src/serializators/lrxmlwriter.cpp \
    src/lrsimplecrypt.cpp \
    src/lrreporttranslation.cpp \
    src/item/table_base_item.cpp \
    src/item/table_item.cpp \
    src/item/group_text_item.cpp \
    src/item/image_base_item.cpp \
    src/item/resource_path_image_item.cpp \
    src/item/variable_image_item.cpp \
    src/item/shape_item.cpp \
    src/item/page_number_item.cpp \
    src/data_manager/device_variant_manager.cpp \
    src/data_manager/text_variant.cpp \
    src/data_manager/image_variant.cpp \
    src/data_manager/table_variant.cpp \
    src/data_manager/base_homogeneous_variant.cpp \
    src/report_manager.cpp \
    src/mask_dialog/device_type_dialog.cpp \
    src/mask_dialog/dialog_background.cpp \
    src/mask_dialog/module_type_dialog.cpp \
    src/mask_dialog/base_mask_dialog.cpp \
    src/mask_dialog/preview_report_widget.cpp \
    src/mask_dialog/base_push_button.cpp \
    src/print_processor.cpp \
    src/item/watermark_item.cpp \
    src/mask_dialog/watermark_setting_dialog.cpp \
    src/mask_dialog/message_info_dialog.cpp \
    src/mask_dialog/language_type_dialog.cpp \
    src/ini_manager.cpp \
    src/mask_dialog/variable_edit_dialog.cpp \
    src/data_manager/print_data_manager.cpp \
    src/object_prop/object_prop_widget.cpp \
    src/object_prop/base_prop_widget.cpp \
    src/object_prop/property_widget_factory.cpp \
    src/object_prop/page_prop_editor_widget.cpp \
    src/object_prop/alignment_prop_widget.cpp \
    src/object_prop/geometry_prop_widget.cpp \
    src/object_prop/prop_base_widget/prop_line_edit.cpp \
    src/object_prop/prop_base_widget/prop_combox.cpp \
    src/object_prop/font_prop_widget.cpp \
    src/object_prop/prop_base_widget/prop_color_button.cpp \
    src/object_prop/prop_base_widget/prop_spinbox_widget.cpp \
    src/object_prop/line_prop_edit.cpp \
    src/object_prop/boder_lines_prop_widget.cpp \
    src/object_prop/tabel_prop_widget.cpp \
    src/object_prop/shape_prop_widget.cpp \
    src/mask_dialog/about_dialog.cpp \
    src/item/synchronization/item_synchronization.cpp \
    src/render/abstract_render.cpp \
    src/render/page_render_factory.cpp \
    src/render/repeat_render.cpp \
    src/mask_dialog/group_property_dialog.cpp \
    src/data_manager/group_variant.cpp \
    src/item/col_header_table.cpp \
    src/render/report_render.cpp

HEADERS += \
    src/design_main_window.hpp \
    src/design_widget.hpp \
    src/base/attribs_abstract_factory.hpp \
    src/base/singleton.hpp \
    src/global.hpp \
    src/item/page_scene.hpp \
    src/item/page_item.hpp \
    src/item/base_design_item.hpp \
    src/item/page_view.hpp \
    src/item/design_elements_factory.hpp \
    src/data_manager/device_variant_manager.hpp \
    src/data_manager/image_variant.hpp \
    src/data_manager/table_variant.hpp \
    src/report_manager.hpp \
    src/mask_dialog/dialog_background.hpp \
    src/mask_dialog/module_type_dialog.hpp \
    src/mask_dialog/device_type_dialog.hpp \
    src/mask_dialog/base_mask_dialog.hpp \
    src/mask_dialog/preview_report_widget.hpp \
    src/mask_dialog/base_push_button.hpp \
    src/print_processor.hpp \
    src/data_manager/text_variant.hpp \
    src/base/simple_abstract_factory.hpp \
    src/data_manager/base_homogeneous_variant.hpp \
    src/item/group_text_item.hpp \
    src/item/image_base_item.hpp \
    src/item/lrcollection.hpp \
    src/item/page_number_item.hpp \
    src/item/resource_path_image_item.hpp \
    src/item/shape_item.hpp \
    src/item/table_base_item.hpp \
    src/item/table_item.hpp \
    src/item/text_item.hpp \
    src/item/variable_image_item.hpp \
    src/serializators/lrserializatorintf.hpp \
    src/serializators/lrstorageintf.hpp \
    src/serializators/lrxmlbasetypesserializators.hpp \
    src/serializators/lrxmlqrectserializator.hpp \
    src/serializators/lrxmlreader.hpp \
    src/serializators/lrxmlserializatorsfactory.hpp \
    src/serializators/lrxmlwriter.hpp \
    src/lrcollection.hpp \
    src/lrreporttranslation.hpp \
    src/lrsimplecrypt.hpp \
    src/item/watermark_item.hpp \
    src/mask_dialog/watermark_setting_dialog.hpp \
    src/mask_dialog/message_info_dialog.hpp \
    src/mask_dialog/language_type_dialog.hpp \
    src/ini_manager.hpp \
    src/mask_dialog/variable_edit_dialog.hpp \
    src/data_manager/print_data_manager.hpp \
    src/object_prop/prop_base_widget/prop_color_button.hpp \
    src/object_prop/prop_base_widget/prop_combox.hpp \
    src/object_prop/prop_base_widget/prop_line_edit.hpp \
    src/object_prop/prop_base_widget/prop_spinbox_widget.hpp \
    src/object_prop/alignment_prop_widget.hpp \
    src/object_prop/base_prop_widget.hpp \
    src/object_prop/boder_lines_prop_widget.hpp \
    src/object_prop/font_prop_widget.hpp \
    src/object_prop/geometry_prop_widget.hpp \
    src/object_prop/line_prop_edit.hpp \
    src/object_prop/object_prop_widget.hpp \
    src/object_prop/page_prop_editor_widget.hpp \
    src/object_prop/property_widget_factory.hpp \
    src/object_prop/shape_prop_widget.hpp \
    src/object_prop/tabel_prop_widget.hpp \
    src/mask_dialog/about_dialog.hpp \
    src/render/report_render.hpp \
    src/item/synchronization/item_synchronization.hpp \
    src/render/abstract_render.hpp \
    src/mask_dialog/group_property_dialog.hpp \
    src/data_manager/group_variant.hpp \
    src/render/repeat_render.hpp \
    src/render/page_render_factory.hpp \
    src/item/col_header_table.hpp \
    src/render/report_render.hpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

FORMS += \
    src/variable_edit_dialog.ui \
    src/device_type_dialog.ui \
    src/module_type_dialog.ui

TRANSLATIONS += \
    config/language/language_zh.ts \
    config/language/language_en.ts \
    config/language/language_ru.ts \
    config/language/language_fr.ts \
    config/language/language_es.ts \
    config/language/language_it.ts

RC_ICONS = rc/images/LOGO.ico
