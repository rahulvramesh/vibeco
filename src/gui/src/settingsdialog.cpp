#include "settingsdialog.h"
#include "config.h"
#include "transcriptionservice.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
    loadSettings();
}

void SettingsDialog::setupUi()
{
    setWindowTitle(tr("Settings"));
    setMinimumWidth(400);

    auto mainLayout = new QVBoxLayout(this);

    // API Key section
    auto apiKeyLayout = new QHBoxLayout;
    auto apiKeyLabel = new QLabel(tr("Groq API Key:"), this);
    m_apiKeyEdit = new QLineEdit(this);
    m_apiKeyEdit->setEchoMode(QLineEdit::Password);
    m_apiKeyEdit->setPlaceholderText(tr("Enter your Groq API key"));
    apiKeyLayout->addWidget(apiKeyLabel);
    apiKeyLayout->addWidget(m_apiKeyEdit);
    mainLayout->addLayout(apiKeyLayout);

    // Model selection section
    auto modelLayout = new QHBoxLayout;
    auto modelLabel = new QLabel(tr("Whisper Model:"), this);
    m_modelCombo = new QComboBox(this);
    m_modelCombo->addItems(TranscriptionService::availableModels());
    modelLayout->addWidget(modelLabel);
    modelLayout->addWidget(m_modelCombo);
    mainLayout->addLayout(modelLayout);

    // Buttons
    auto buttonLayout = new QHBoxLayout;
    auto saveButton = new QPushButton(tr("Save"), this);
    auto cancelButton = new QPushButton(tr("Cancel"), this);
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(saveButton, &QPushButton::clicked, this, &SettingsDialog::saveSettings);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void SettingsDialog::loadSettings()
{
    // Load API key
    QString apiKey = Config::instance().getApiKey();
    m_apiKeyEdit->setText(apiKey);

    // Load selected model
    QString currentModel = Config::instance().getModel();
    int index = m_modelCombo->findText(currentModel);
    if (index >= 0) {
        m_modelCombo->setCurrentIndex(index);
    }
}

void SettingsDialog::saveSettings()
{
    QString apiKey = m_apiKeyEdit->text().trimmed();
    QString model = m_modelCombo->currentText();

    // Validate API key
    if (apiKey.isEmpty()) {
        QMessageBox::warning(this, tr("Error"),
            tr("Please enter your Groq API key. You can get one from https://console.groq.com"));
        return;
    }

    // Validate API key format (basic check)
    if (!apiKey.startsWith("gsk_") || apiKey.length() < 20) {
        QMessageBox::warning(this, tr("Error"),
            tr("Invalid API key format. Groq API keys should start with 'gsk_' and be at least 20 characters long."));
        return;
    }

    bool success = true;
    
    // Save API key
    if (!Config::instance().setApiKey(apiKey)) {
        success = false;
        QMessageBox::warning(this, tr("Error"), 
            tr("Failed to save API key. Please check your permissions."));
    }

    // Save model selection
    if (!Config::instance().setModel(model)) {
        success = false;
        QMessageBox::warning(this, tr("Error"),
            tr("Failed to save model selection. Please check your permissions."));
    }

    if (success) {
        QMessageBox::information(this, tr("Success"),
            tr("Settings saved successfully."));
        accept();
    }
} 