#include "config.h"
#include <QCryptographicHash>
#include <QDataStream>
#include <QDebug>

const QString Config::CONFIG_ORG = "Vibeco";
const QString Config::CONFIG_APP = "Vibeco";
const QString Config::KEY_API_KEY = "GroqApiKey";
const QString Config::KEY_MODEL = "WhisperModel";
const QString Config::DEFAULT_MODEL = "whisper-large-v3-turbo";

Config::Config()
    : m_settings(CONFIG_ORG, CONFIG_APP)
{
    // Ensure the config directory exists
    QString configPath = getConfigPath();
    QDir dir;
    if (!dir.exists(configPath)) {
        dir.mkpath(configPath);
    }
    qDebug() << "Config initialized. Path:" << configPath;
    qDebug() << "Settings file:" << m_settings.fileName();
}

QString Config::getApiKey() const {
    QString encryptedKey = m_settings.value(KEY_API_KEY).toString();
    qDebug() << "Getting API key. Encrypted key exists:" << !encryptedKey.isEmpty();

    if (encryptedKey.isEmpty()) {
        qDebug() << "No API key found in settings";
        return QString();
    }

    // Simple XOR-based obfuscation (not true encryption, but better than plaintext)
    QByteArray key = QByteArray::fromBase64(encryptedKey.toLatin1());
    QByteArray machineId = QSysInfo::machineUniqueId();
    QByteArray result;

    for (int i = 0; i < key.size(); ++i) {
        result.append(key.at(i) ^ machineId.at(i % machineId.size()));
    }

    QString decryptedKey = QString::fromUtf8(result);
    qDebug() << "API key retrieved successfully. Length:" << decryptedKey.length();
    return decryptedKey;
}

bool Config::setApiKey(const QString& key) {
    qDebug() << "Setting API key. Key length:" << key.length();

    if (key.isEmpty()) {
        qDebug() << "Removing API key from settings";
        m_settings.remove(KEY_API_KEY);
        return true;
    }

    // Simple XOR-based obfuscation
    QByteArray machineId = QSysInfo::machineUniqueId();
    QByteArray rawKey = key.toUtf8();
    QByteArray result;

    for (int i = 0; i < rawKey.size(); ++i) {
        result.append(rawKey.at(i) ^ machineId.at(i % machineId.size()));
    }

    QString encryptedKey = QString::fromLatin1(result.toBase64());
    m_settings.setValue(KEY_API_KEY, encryptedKey);
    m_settings.sync(); // Force write to disk

    bool success = m_settings.status() == QSettings::NoError;
    qDebug() << "API key saved:" << success << "Settings file:" << m_settings.fileName();
    return success;
}

QString Config::getModel() const {
    QString model = m_settings.value(KEY_MODEL, DEFAULT_MODEL).toString();
    qDebug() << "Getting model:" << model;
    return model;
}

bool Config::setModel(const QString& model) {
    qDebug() << "Setting model:" << model;
    if (model.isEmpty()) {
        m_settings.setValue(KEY_MODEL, DEFAULT_MODEL);
    } else {
        m_settings.setValue(KEY_MODEL, model);
    }
    m_settings.sync(); // Force write to disk
    return m_settings.status() == QSettings::NoError;
}

QString Config::getConfigPath() {
    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
}
