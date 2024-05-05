#include "config.h"

Config::Config() : QSettings{QSettings::IniFormat, QSettings::UserScope, "KokoVP", "player"} {
}

QVariant Config::get(const QAnyStringView &key) const
{
    return value(key);
}

QVariant Config::get(QAnyStringView key, const QVariant &defaultValue)
{
    if (!contains(key))
        setValue(key, defaultValue);

    return value(key, defaultValue);
}

void Config::set(QAnyStringView key, const QVariant &value)
{
    setValue(key, value);
}
