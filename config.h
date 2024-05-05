#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>

class Config : public QSettings
{
public:
    Config(const Config& root) = delete;
    Config& operator=(const Config&) = delete;
    static Config& i()
    {
        static Config instance;
        return instance;
    }
    QVariant get(const QAnyStringView &key) const;
    QVariant get(QAnyStringView key, const QVariant &defaultValue);
    void set(QAnyStringView key, const QVariant &value);
private:
    Config();
};

#endif // CONFIG_H
