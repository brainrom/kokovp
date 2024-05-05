#include "cache.h"
#include <QStandardPaths>

Cache::Cache() : QSettings{QStandardPaths::writableLocation(QStandardPaths::CacheLocation)+"/KokoVP.ini", QSettings::IniFormat} {
}

QVariant Cache::get(const QAnyStringView &key) const
{
    return value(key);
}

QVariant Cache::get(QAnyStringView key, const QVariant &defaultValue)
{
    if (!contains(key))
        setValue(key, defaultValue);

    return value(key, defaultValue);
}

void Cache::set(QAnyStringView key, const QVariant &value)
{
    setValue(key, value);
    sync();
}
