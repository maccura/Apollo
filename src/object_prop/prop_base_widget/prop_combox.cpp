#include "prop_combox.hpp"
#define PROP_COMBOX_MIN_HEIGHT  28
namespace ReportDesign
{
prop_combox::prop_combox(QWidget *parent /*= nullptr*/): QComboBox(parent)
{
    setMinimumHeight(PROP_COMBOX_MIN_HEIGHT);
}
}
