/****************************************************************************
**
** Copyright (C) VCreate Logic Private Limited, Bangalore
**
** Use of this file is limited according to the terms specified by
** VCreate Logic Private Limited, Bangalore.  Details of those terms
** are listed in licence.txt included as part of the distribution package
** of this file. This file may not be distributed without including the
** licence.txt file.
**
** Contact info@vcreatelogic.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef OBJECTMAP_H
#define OBJECTMAP_H

#include <QMap>
#include "ObjectList.h"
#include "Log.h"

namespace GCF
{

class ObjectMapEventListener
{
public:
    ObjectMapEventListener() { }
    virtual ~ObjectMapEventListener() { }

    virtual QString listenerType() const {
        return "GCF::ObjectMapEventListener";
    }

    virtual void objectInserted(QObject *object) { Q_UNUSED(object); }
    virtual void objectRemoved(QObject *object) { Q_UNUSED(object); }
    virtual void objectDeleted(QObject *object) { Q_UNUSED(object); }
};

template <class T>
class ObjectMap : public GCF::ObjectListEventListener
{
public:
    ObjectMap() : m_listener(0) {
        m_objectList.setEventListener(this);
    }
    ObjectMap(const ObjectMap &other)
        : GCF::ObjectListEventListener(), m_listener(0) {
        m_map = other.m_map;
        m_objectList = other.m_objectList;
        m_objectList.setEventListener(this);
    }
    ObjectMap(const QMap<QObject*,T>& map) : m_listener(0) {
        QObjectList objects = map.keys();
        Q_FOREACH(QObject *obj, objects)
            this->insert(obj, map.value(obj));
        m_objectList.setEventListener(this);
    }
    ~ObjectMap() { }

    void setEventListener(ObjectMapEventListener *listener) {
        m_listener = listener;
    }
    ObjectMapEventListener *eventListener() const {
        return m_listener;
    }

    ObjectMap &operator = (const ObjectMap &other) {
        if( this == &other )
            return *this;

        m_listener = 0;
        m_map = other.m_map;
        m_objectList = other.m_objectList;
        m_objectList.setEventListener(this);
        return *this;
    }

    void insert(QObject *object, const T &value) {
        m_map.insert(object, value);
        m_objectList.add(object);
    }

    void remove(QObject *object) {
        m_objectList.remove(object);
        m_map.remove(object);
    }

    T &operator [] (QObject *object) {
        Q_ASSERT(m_map.contains(object));
        return m_map[object];
    }

    const T &operator [] (QObject *object) const {
        Q_ASSERT(m_map.contains(object));
        return m_map[object];
    }

    int count() const { return m_map.count(); }
    bool isEmpty() const { return m_map.isEmpty(); }
    bool contains(QObject *object) const { return m_map.contains(object); }
    T value(QObject *object, const T &defVal=T()) const { return m_map.value(object, defVal); }
    QObjectList keys() const { return m_map.keys(); }
    QList<T> values() const { return m_map.values(); }
    QMap<QObject*,T> toMap() const { return m_map; }
    const QMap<QObject*,T>& map() const { return m_map; }

    void removeAll() {
        m_objectList.removeAll();
        Q_ASSERT(m_map.isEmpty());
    }

    void deleteAll() {
        m_objectList.deleteAll();
        Q_ASSERT(m_map.isEmpty());
    }

private:
    virtual QString listenerType() const {
        return QString("GCF::ObjectMap");
    }
    virtual void objectAdded(int, QObject *obj) {
        if(m_listener)
            m_listener->objectInserted(obj);
    }
    virtual void objectRemoved(int, QObject *obj) {
        if(m_listener)
            m_listener->objectRemoved(obj);
        m_map.remove(obj);
    }
    virtual void objectDeleted(int, QObject *obj) {
        if(m_listener)
            m_listener->objectDeleted(obj);
    }

private:
    QMap<QObject*,T> m_map;
    GCF::ObjectList m_objectList;
    ObjectMapEventListener *m_listener;
};

#if QT_VERSION >= 0x050000
class ObjectMapEventBroadcaster Q_DECL_FINAL : public ObjectMapEventListener
#else
class ObjectMapEventBroadcaster : public ObjectMapEventListener
#endif
{
public:
    void addListener(ObjectMapEventListener *listener) {
        if(listener && !m_listenerList.contains(listener))
            m_listenerList.append(listener);
    }
    void removeListener(ObjectMapEventListener *listener) {
        if(listener && m_listenerList.contains(listener))
            m_listenerList.removeAll(listener);
    }
    void removeAllListeners() {
        m_listenerList.clear();
    }
    ObjectMapEventListener *listenerAt(int index) const {
        if(index < 0 || index >= m_listenerList.count())
            return 0;
        return m_listenerList.at(index);
    }

    QString listenerType() const {
        return QString("GCF::ObjectMapEventBroadcaster");
    }
    void objectInserted(QObject *obj) {
        Q_FOREACH(ObjectMapEventListener *l, m_listenerList)
            l->objectInserted(obj);
    }
    void objectRemoved(QObject *obj) {
        Q_FOREACH(ObjectMapEventListener *l, m_listenerList)
            l->objectRemoved(obj);
    }
    void objectDeleted(QObject *obj) {
        Q_FOREACH(ObjectMapEventListener *l, m_listenerList)
            l->objectDeleted(obj);
    }

private:
    QList<ObjectMapEventListener*> m_listenerList;
};

#if QT_VERSION >= 0x050000
class GCF_EXPORT ObjectMapWatcher Q_DECL_FINAL : public QObject, public ObjectMapEventListener
#else
class GCF_EXPORT ObjectMapWatcher : public QObject, public ObjectMapEventListener
#endif
{
    Q_OBJECT

public:
    ObjectMapWatcher(QObject *parent=0) : QObject(parent) { }
    ~ObjectMapWatcher() { }

    template <class T>
    void watch(ObjectMap<T>& map) {
        if(map.eventListener())
            GCF::Log::instance()->warning(GCF_DEFAULT_LOG_CONTEXT,
                "Installing a GCF::ObjectMapWatcher on a GCF::ObjectMap that is already being watched "
                "by a listener of type " + map.eventListener()->listenerType());
        map.setEventListener(this);
    }

    QString listenerType() const {
        return QString("GCF::ObjectListWatcher");
    }

signals:
    void objectInserted(QObject *obj);
    void objectRemoved(QObject *obj);
    void objectDeleted(QObject *obj);
};


}

#endif // OBJECTMAP_H