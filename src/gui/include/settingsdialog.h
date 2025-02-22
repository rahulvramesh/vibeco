#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private slots:
    void saveSettings();
    void loadSettings();

private:
    void setupUi();
    
    QLineEdit* m_apiKeyEdit;
    QComboBox* m_modelCombo;
};

#endif // SETTINGSDIALOG_H 