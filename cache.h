#ifndef CACHE_H
#define CACHE_H

#include <QSettings>

class Cache : public QSettings
{
public:
    Cache(const Cache& root) = delete;
    Cache& operator=(const Cache&) = delete;
    static Cache& i()
    {
        static Cache instance;
        return instance;
    }
    QVariant get(const QAnyStringView &key) const;
    QVariant get(QAnyStringView key, const QVariant &defaultValue);
    void set(QAnyStringView key, const QVariant &value);
private:
    Cache();
};

#endif // CACHE_H
