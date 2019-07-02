#include "settingsdialog.hpp"
#include "ui_settingsdialog.h"

#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include <QJsonArray>

SettingsDialog::SettingsDialog( QWidget *parent ) :
    QDialog(parent),
    ui( new Ui::SettingsDialog ),
    app_settings{ utilities::AppSettings::GetAppSettings() },
    languages{ app_settings.GetLanguageInfo() }
{
    ui->setupUi( this );

    QObject::connect( ui->languages_list_widget, &QListWidget::currentRowChanged,
                      [=]( int const row )
    {
        ui->extensions_list_widget->clear();
        ui->extensions_list_widget->addItems( languages[row].extensions );
    });

    if( !languages.isEmpty() ){
        for( auto const & language: languages ){
            ui->languages_list_widget->addItem( language.language_name );
        }
        default_ = app_settings.GetDefaultLanguageIndex();
        ui->languages_list_widget->setCurrentRow( default_ );
        ui->languages_list_widget->currentItem()->setText(
                    languages[default_].language_name + "(default)" );
    }
    ui->languages_list_widget->setContextMenuPolicy( Qt::CustomContextMenu );
    ui->extensions_list_widget->setContextMenuPolicy( Qt::CustomContextMenu );
    QObject::connect( ui->languages_list_widget,
                      &QListWidget::customContextMenuRequested, this,
                      &SettingsDialog::OnLanguageCustomContextMenuRequested );
    QObject::connect( ui->extensions_list_widget,
                      &QListWidget::customContextMenuRequested, this,
                      &SettingsDialog::OnExtensionCustomContextMenuRequested );
}

void SettingsDialog::OnExtensionCustomContextMenuRequested( QPoint const & pos )
{
    QModelIndex const index{ ui->extensions_list_widget->currentIndex() };
    QModelIndex const lang_index{ ui->languages_list_widget->currentIndex() };
    if( !lang_index.isValid() ){
        QMessageBox::information( this, "Info", "Select language on the LHS" );
        return;
    }
    int const lang_row = lang_index.row();
    QMenu menu{};
    auto add_event = [=]() mutable {
        QString const extension {
            QInputDialog::getText( this, "Add",
                                   "Enter extension, multiple extensions can be"
                                   " separated by ;" ).trimmed()
        };
        QStringList const exts{ extension.split( ';' ) };
        if( exts.isEmpty() ) return;
        ui->extensions_list_widget->addItems( exts );
        app_settings.AddExtension( lang_row, exts );
    };
    auto edit_event = [=]() mutable {
        int const extension_row = ui->extensions_list_widget->currentRow();
        QString const & default_{
            languages[lang_row].extensions[extension_row]
        };
        QString const new_text {
            QInputDialog::getText( this, "Edit", "Rename extension",
                                   QLineEdit::Normal, default_ ).trimmed()
        };
        if( new_text.isEmpty() ){
            app_settings.RemoveExtension( lang_row, extension_row );
        }
        delete ui->extensions_list_widget->currentItem();
    };

    auto remove_event = [=]() mutable {
        int const row = ui->extensions_list_widget->currentRow();
        app_settings.RemoveExtension( lang_row, row );
        delete ui->extensions_list_widget->currentItem();
    };

    QAction* action_add_extension{ new QAction( "Add extension" ) };
    menu.addAction( action_add_extension );
    QObject::connect( action_add_extension, &QAction::triggered, add_event );
    if( index.isValid() ){
        QAction* action_edit_extension{ new QAction( "Edit" ) };
        QAction* action_remove_extension{ new QAction( "Remove" ) };

        connect( action_edit_extension, &QAction::triggered, edit_event );
        connect( action_remove_extension, &QAction::triggered, remove_event );
        menu.addAction( action_edit_extension );
        menu.addAction( action_remove_extension );
    }
    menu.exec( ui->extensions_list_widget->mapToGlobal( pos ) );
}

void SettingsDialog::OnLanguageCustomContextMenuRequested( QPoint const & pos )
{
    QModelIndex const index{ ui->languages_list_widget->currentIndex() };

    auto add_event = [=]{
        QString const language_name{
            QInputDialog::getText( this, "Language", "Language name" ).trimmed()
        };
        if( language_name.isEmpty() ) return;
        app_settings.AddLanguage( language_name );
        ui->languages_list_widget->addItem( language_name );
    };
    auto remove_event = [=]{
        if( QMessageBox::question( this, "Remove", "Are you sure?" ) ==
                QMessageBox::No ) return;
        int const count{ ui->extensions_list_widget->count() };
        for( int x = 0; x != count; ++x ){
            delete ui->extensions_list_widget->item( x );
        }
        app_settings.RemoveLanguage( index.row() );
        delete ui->languages_list_widget->currentItem();
    };
    auto default_event = [=]() mutable {
        int const previous_default = default_;
        this->default_ = ui->languages_list_widget->currentRow();
        QString const old_text { languages[previous_default].language_name };
        QString const new_text {
            ui->languages_list_widget->currentItem()->text() + "(default)" };
        ui->languages_list_widget->currentItem()->setText( new_text );
        ui->languages_list_widget->item( previous_default )->setText( old_text );
    };
    auto edit_event = [=]{};

    QMenu menu {};
    QAction* action_add_language{ new QAction( "Add language" ) };
    menu.addAction( action_add_language );
    QObject::connect( action_add_language, &QAction::triggered, add_event );

    if( index.isValid() ){
        QAction* action_edit_language{ new QAction( "Edit name" ) };
        QAction* action_remove_language{ new QAction( "Remove" ) };
        QAction* action_make_default{ new QAction( "Set as default" ) };

        connect( action_edit_language, &QAction::triggered, edit_event );
        connect( action_remove_language, &QAction::triggered, remove_event );
        connect( action_make_default, &QAction::triggered, default_event );

        menu.addAction( action_make_default );
        menu.addAction( action_edit_language );
        menu.addAction( action_remove_language );
    }
    menu.exec( ui->languages_list_widget->mapToGlobal( pos ) );
}


SettingsDialog::~SettingsDialog()
{
    app_settings.WriteSettings();
    delete ui;
}
