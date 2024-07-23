#include "object_prop_widget.hpp"
#include "property_widget_factory.hpp"
#include "item/page_item.hpp"
#define  PROPWIDGETOFFSET   15
namespace ReportDesign
{
object_prop_widget::object_prop_widget(QWidget *parent /*= nullptr*/): QWidget(parent), m_object(nullptr)
{
    init();
    m_VLayout = new QVBoxLayout(this);
    m_VLayout->setContentsMargins(0, 0, 0, 0);
    m_VLayout->setSpacing(0);
}

void object_prop_widget::init()
{
    m_propSortTable.push_back(PROPERTY_TO_STRING(ARTBOARD_BACKGROUND_PROPERTY));
    m_propSortTable.push_back(PROPERTY_TO_STRING(PAGE_SIZE_PROPERTY));
    m_propSortTable.push_back(PROPERTY_TO_STRING(PAGE_ORIENTATION_PROPERTY));
    m_propSortTable.push_back(PROPERTY_TO_STRING(ITEM_ALIGNMENT_PROPERTY));
    m_propSortTable.push_back(PROPERTY_TO_STRING(GEOMETRY_PROPERTY));
    m_propSortTable.push_back(PROPERTY_TO_STRING(FONT_PROPERTY));
    m_propSortTable.push_back(PROPERTY_TO_STRING(BORDERLINESIZE_PROPERTY));
    m_propSortTable.push_back(PROPERTY_TO_STRING(SHAPE_PROPERTY));
    m_propSortTable.push_back(PROPERTY_TO_STRING(BORDER_PROPERTY));
    m_propSortTable.push_back(PROPERTY_TO_STRING(TABLE_ROW_COUNT_PROPERTY));
}

void object_prop_widget::setObject(QObject *object)
{
    base_design_item *reportItem = dynamic_cast<base_design_item *>(object);
    if (reportItem)
    {
        // 第五个连接参数需要与上Qt::UniqueConnection，防止同一个item重复connect 导致参函数执行多次
        connect(reportItem, SIGNAL(propertyChanged(QString, QVariant, QVariant)),
                this, SLOT(slotPropertyChanged(QString, QVariant, QVariant)),
                Qt::ConnectionType(Qt::AutoConnection | Qt::UniqueConnection));

        connect(reportItem, SIGNAL(propertyUpdate(QString)),
                this, SLOT(slotPropertyUpdate(QString)),
                Qt::ConnectionType(Qt::AutoConnection | Qt::UniqueConnection));
    }

    m_objects.clear();
    if (m_object != object)
    {
        if (m_object)
        {
            // 解除上一个对象的所有信号关联。
            disconnect(m_object, SIGNAL(destroyed(QObject *)), this,
                       SLOT(slotObjectDestroyed(QObject *)));
        }

        m_object = object;
        if (m_object)
        {
            connect(m_object, SIGNAL(destroyed(QObject *)), this,
                    SLOT(slotObjectDestroyed(QObject *)), Qt::ConnectionType(Qt::AutoConnection | Qt::UniqueConnection));

            foreach (QObject *item, m_objects)
            {
                connect(item, SIGNAL(destroyed(QObject *)), this, SLOT(slotObjectDestroyed(QObject *)), Qt::ConnectionType(Qt::AutoConnection | Qt::UniqueConnection));
            }
        }
        updatePropWidget();
    }
}

void object_prop_widget::setMultiObjects(QList<QObject *> *list)
{
    m_objects.clear();
    if (list->isEmpty())
    {
        return;
    }

    if (!list->contains(m_object))
    {
        m_object = list->at(0);
        list->removeAt(0);
        m_objects.append(*list);
        updatePropWidget();
    }
    else
    {
        list->removeOne(m_object);
        m_objects.append(*list);
    }
}

const QObject *object_prop_widget::object()
{
    return m_object;
}

void object_prop_widget::slotObjectDestroyed(QObject *obj)
{
    m_objects.removeOne(obj);
    if (m_object == obj)
    {
        m_object = nullptr;
        updatePropWidget();
    }
}

void object_prop_widget::setPropMap(QMap<QString, base_prop_widget *> map)
{
    //QMutexLocker  locker(&m_propWidgetMapMutex);
    m_propWidgetMap = map;
}

void object_prop_widget::clearPropMap()
{
    //QMutexLocker  locker(&m_propWidgetMapMutex);
    m_propWidgetMap.clear();
}

void object_prop_widget::updatePropMap(QString property)
{
    //QMutexLocker  locker(&m_propWidgetMapMutex);
    if (m_propWidgetMap.contains(property))
    {
        m_propWidgetMap.value(property)->update(property);
    }
}

void object_prop_widget::updatePropWidget()
{
    clearPropWidget();
    QVector<base_prop_widget *> widgetList;

    widgetList = genPropWidgets(m_object, &m_objects, this);
    widgetList = sortPropWidget(m_propWidgetMap);

    for (base_prop_widget *propWidget : widgetList)
    {
        m_VLayout->addWidget(propWidget);
    }
}

void object_prop_widget::clearPropWidget()
{
    // 遍历QVBoxlayout中的所有QWidget
    QLayoutItem *item;
    while ((item = m_VLayout->takeAt(0)) != nullptr)
    {
        delete item->widget();
        delete item;
    }
    clearPropMap();
}

QVector<base_prop_widget *> object_prop_widget::sortPropWidget(QMap<QString, base_prop_widget *> propWidgetMap)
{
    QVector<base_prop_widget *> sortWidgetVector;
    for (QString it : m_propSortTable)
    {
        if (propWidgetMap.contains(it))
        {
            sortWidgetVector.push_back(propWidgetMap.value(it));
        }
    }
    return sortWidgetVector;
}

bool object_prop_widget::creatorContains(QVector<CreatePropWidget> vector, CreatePropWidget Creator)
{
    for (CreatePropWidget it : vector)
    {
        if (it == Creator)
        {
            return true;
        }
    }
    return false;
}

CreatePropWidget object_prop_widget::propWidgetCreateor(QString property)
{
    return property_widget_factory::instance().creator(property);
}

bool object_prop_widget::propFilter(QObject *obj, QString property)
{
    // page页不显示的属性
    page_item *pageItem = dynamic_cast<page_item *>(obj);
    if (pageItem)
    {
        if (!(property == PROPERTY_TO_STRING(PAGE_SIZE_PROPERTY) ||
              property == PROPERTY_TO_STRING(PAGE_ORIENTATION_PROPERTY)))
        {
            return true;
        }
    }
    return false;
}

QVector<base_prop_widget *> object_prop_widget::genPropWidgets(QObject *object, QList<QObject *> *objects, QWidget *parent)
{
    if (object == nullptr)
    {
        return QVector<base_prop_widget *>();
    }

    QVector<CreatePropWidget> createorVector;
    QMap<QString, CreatePropWidget> CreateMap;
    const QMetaObject *metaObject = object->metaObject();
    //  顺序为子类属性遍历->父类属性遍历
    for (int i = 0; i < metaObject->propertyCount(); i++)
    {
        QString propertyName = metaObject->property(i).name();
        if (metaObject->property(i).isDesignable() && metaObject->property(i).isReadable()
            && !propFilter(object, propertyName))
        {
            // objectName属性不展示
            if (QString("objectName").compare(propertyName) == 0)
            {
                continue;
            }

            // map 中有重复， vector中为去重后的
            CreatePropWidget propWidgetCreator = propWidgetCreateor(propertyName);
            if (propWidgetCreator != nullptr)
            {
                CreateMap.insert(propertyName, propWidgetCreator);
                if (!creatorContains(createorVector, propWidgetCreator))
                {
                    createorVector.push_back(propWidgetCreator);
                }
            }
        }
    }

    QMap<QString, base_prop_widget *> tem_map;
    QVector<base_prop_widget *> prop_widget_vec;
    for (CreatePropWidget creator : createorVector)
    {
        base_prop_widget *propWidget = creator(object, objects, parent);
        if (!propWidget)
        {
            continue;
        }

        prop_widget_vec.push_back(propWidget);
        for (QMap<QString, CreatePropWidget>::iterator it = CreateMap.begin(); it != CreateMap.end(); ++it)
        {
            if (it.value() == creator)
            {
                tem_map.insert(it.key(), propWidget);
            }
        }
    }

    setPropMap(tem_map);
    return prop_widget_vec;
}

void object_prop_widget::slotPropertyUpdate(const QString &propertyName)
{
    updatePropMap(propertyName);
}

void object_prop_widget::slotPropertyChanged(const QString &propertyName, const QVariant &oldValue, const QVariant &newValue)
{
    Q_UNUSED(oldValue);
    Q_UNUSED(newValue);
    slotPropertyUpdate(propertyName);
}

}
