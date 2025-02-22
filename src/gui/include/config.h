#ifndef VIBECO_CONFIG_H
#define VIBECO_CONFIG_H

#include <QString>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

class Config {
public:
    static Config& instance() {
        static Config instance;
        return instance;
    }

    QString getApiKey() const;
    bool setApiKey(const QString& key);
    
    QString getModel() const;
    bool setModel(const QString& model);
    
    static QString getConfigPath();

private:
    Config(); // Private constructor for singleton
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    QSettings m_settings;
    
    // Constants
    static const QString CONFIG_ORG;
    static const QString CONFIG_APP;
    static const QString KEY_API_KEY;
    static const QString KEY_MODEL;
    static const QString DEFAULT_MODEL;
};

#endif // VIBECO_CONFIG_H 