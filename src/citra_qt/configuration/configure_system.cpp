// Copyright 2016 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <QMessageBox>
#include "citra_qt/configuration/configure_system.h"
#include "citra_qt/ui_settings.h"
#include "core/core.h"
#include "core/hle/service/cfg/cfg.h"
#include "core/hle/service/fs/archive.h"
#include "ui_configure_system.h"

static const std::array<int, 12> days_in_month = {{
    31,
    29,
    31,
    30,
    31,
    30,
    31,
    31,
    30,
    31,
    30,
    31,
}};

ConfigureSystem::ConfigureSystem(QWidget* parent) : QWidget(parent), ui(new Ui::ConfigureSystem) {
    ui->setupUi(this);

    connect(ui->combo_birthmonth,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &ConfigureSystem::updateBirthdayComboBox);
    connect(ui->button_regenerate_console_id, &QPushButton::clicked, this,
            &ConfigureSystem::refreshConsoleID);

    setConfiguration();
}

ConfigureSystem::~ConfigureSystem() {}

void ConfigureSystem::setConfiguration() {
    enabled = !Core::System::GetInstance().IsPoweredOn();

    if (!enabled) {
        cfg = Service::CFG::GetCurrentModule();
        ReadSystemSettings();
        ui->group_system_settings->setEnabled(false);
        ui->link_country_codes->hide();
    } else {
        // This tab is enabled only when game is not running (i.e. all service are not initialized).
        // Temporarily register archive types and load the config savegame file to memory.
        Service::FS::RegisterArchiveTypes();
        cfg = std::make_shared<Service::CFG::Module>();
        Service::FS::UnregisterArchiveTypes();

        ReadSystemSettings();
        ui->label_disable_info->hide();
        ui->link_country_codes->setOpenExternalLinks(true);
        ui->link_country_codes->setText(
            tr("<a "
               "href='https://3dbrew.org/wiki/Country_Code_List'>Country Codes</a>"));
    }
}

void ConfigureSystem::ReadSystemSettings() {
    // set username
    username = cfg->GetUsername();
    // TODO(wwylele): Use this when we move to Qt 5.5
    // ui->edit_username->setText(QString::fromStdU16String(username));
    ui->edit_username->setText(
        QString::fromUtf16(reinterpret_cast<const ushort*>(username.data())));

    // set birthday
    std::tie(birthmonth, birthday) = cfg->GetBirthday();
    ui->combo_birthmonth->setCurrentIndex(birthmonth - 1);
    updateBirthdayComboBox(
        birthmonth -
        1); // explicitly update it because the signal from setCurrentIndex is not reliable
    ui->combo_birthday->setCurrentIndex(birthday - 1);

    // set system language
    language_index = cfg->GetSystemLanguage();
    ui->combo_language->setCurrentIndex(language_index);

    // set model
    model_index = static_cast<int>(cfg->GetSystemModel());
    ui->combo_model->setCurrentIndex(model_index);

    // set country
    std::tie(unknown, country_index) = cfg->GetCountryInfo();
    ui->spinbox_country->setValue(country_index);

    // set sound output mode
    sound_index = cfg->GetSoundOutputMode();
    ui->combo_sound->setCurrentIndex(sound_index);

    // set the console id
    u64 console_id = cfg->GetConsoleUniqueId();
    ui->label_console_id->setText(
        tr("Console ID: 0x%1").arg(QString::number(console_id, 16).toUpper()));
}

void ConfigureSystem::applyConfiguration() {
    if (!enabled)
        return;

    bool modified = false;

    // apply username
    // TODO(wwylele): Use this when we move to Qt 5.5
    // std::u16string new_username = ui->edit_username->text().toStdU16String();
    std::u16string new_username(
        reinterpret_cast<const char16_t*>(ui->edit_username->text().utf16()));
    if (new_username != username) {
        cfg->SetUsername(new_username);
        modified = true;
    }

    // apply birthday
    int new_birthmonth = ui->combo_birthmonth->currentIndex() + 1;
    int new_birthday = ui->combo_birthday->currentIndex() + 1;
    if (birthmonth != new_birthmonth || birthday != new_birthday) {
        cfg->SetBirthday(new_birthmonth, new_birthday);
        modified = true;
    }

    // apply language
    int new_language = ui->combo_language->currentIndex();
    if (language_index != new_language) {
        cfg->SetSystemLanguage(static_cast<Service::CFG::SystemLanguage>(new_language));
        modified = true;
    }

    // apply model
    int new_model = ui->combo_model->currentIndex();
    if (model_index != new_model) {
        cfg->SetSystemModel(static_cast<Service::CFG::SystemModel>(new_model));
        modified = true;
    }

    // apply country
    if (!ValidateCountry()) {
        ui->spinbox_country->setValue(1);
        QString error_text = tr("Invalid country id, changed to 1.");
        QMessageBox::critical(this, tr("Error"), error_text, QMessageBox::Ok);
    }

    int new_country = ui->spinbox_country->value();
    if (country_index != new_country) {
        cfg->SetCountryInfo(unknown, static_cast<u8>(ui->spinbox_country->value()));
        modified = true;
    }

    // apply sound
    int new_sound = ui->combo_sound->currentIndex();
    if (sound_index != new_sound) {
        cfg->SetSoundOutputMode(static_cast<Service::CFG::SoundOutputMode>(new_sound));
        modified = true;
    }

    // update the config savegame if any item is modified.
    if (modified)
        cfg->UpdateConfigNANDSavegame();
}

void ConfigureSystem::updateBirthdayComboBox(int birthmonth_index) {
    if (birthmonth_index < 0 || birthmonth_index >= 12)
        return;

    // store current day selection
    int birthday_index = ui->combo_birthday->currentIndex();

    // get number of days in the new selected month
    int days = days_in_month[birthmonth_index];

    // if the selected day is out of range,
    // reset it to 1st
    if (birthday_index < 0 || birthday_index >= days)
        birthday_index = 0;

    // update the day combo box
    ui->combo_birthday->clear();
    for (int i = 1; i <= days; ++i) {
        ui->combo_birthday->addItem(QString::number(i));
    }

    // restore the day selection
    ui->combo_birthday->setCurrentIndex(birthday_index);
}

void ConfigureSystem::refreshConsoleID() {
    QMessageBox::StandardButton reply;
    QString warning_text = tr("This will replace your current virtual 3DS with a new one. "
                              "Your current virtual 3DS will not be recoverable. "
                              "This might have unexpected effects in games. This might fail, "
                              "if you use an outdated config savegame. Continue?");
    reply = QMessageBox::critical(this, tr("Warning"), warning_text,
                                  QMessageBox::No | QMessageBox::Yes);
    if (reply == QMessageBox::No)
        return;
    u32 random_number;
    u64 console_id;
    cfg->GenerateConsoleUniqueId(random_number, console_id);
    cfg->SetConsoleUniqueId(random_number, console_id);
    cfg->UpdateConfigNANDSavegame();
    ui->label_console_id->setText("Console ID: 0x" + QString::number(console_id, 16).toUpper());
}

bool ConfigureSystem::ValidateCountry() {
    if (ui->spinbox_country->value() > 1 && ui->spinbox_country->value() < 8) {
        return false;
    } else if (ui->spinbox_country->value() > 52 && ui->spinbox_country->value() < 64) {
        return false;
    } else if (ui->spinbox_country->value() > 128 && ui->spinbox_country->value() < 136) {
        return false;
    } else if (ui->spinbox_country->value() > 136 && ui->spinbox_country->value() < 144) {
        return false;
    } else if (ui->spinbox_country->value() > 155 && ui->spinbox_country->value() < 160) {
        return false;
    } else if (ui->spinbox_country->value() > 160 && ui->spinbox_country->value() < 168) {
        return false;
    } else if (ui->spinbox_country->value() > 177 && ui->spinbox_country->value() < 184) {
        return false;
    }

    return true;
}

void ConfigureSystem::retranslateUi() {
    ui->retranslateUi(this);
}
