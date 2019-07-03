#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QDialog>
#include "resources.hpp"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog( QWidget *parent = nullptr );
    ~SettingsDialog();
private:
    void OnLanguageCustomContextMenuRequested( QPoint const & pos );
    void OnExtensionCustomContextMenuRequested( QPoint const &pos );
    void OnDefaultLanguageChanged( int const );
private:
    Ui::SettingsDialog *ui;
    utilities::AppSettings& app_settings;
    QVector<utilities::LanguageInfo> const & languages;
    int default_ {};
};

#endif // SETTINGSDIALOG_HPP
