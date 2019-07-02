#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "counterdialog.hpp"
#include "settingsdialog.hpp"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui( new Ui::MainWindow ), workspace{ new QMdiArea( this ) },
    app_settings{ utilities::AppSettings::GetAppSettings() },
    languages_menu{ new QMenu( "Languages", this ) }
{
    ui->setupUi( this );
    setCentralWidget( workspace );
    QObject::connect( ui->actionExit, &QAction::triggered, this,
                      &MainWindow::close );
    QObject::connect( ui->actionPreference, &QAction::triggered, this,
                      &MainWindow::OnSettingsTriggered );
    QObject::connect( ui->actionLine_Counter, &QAction::triggered, this,
                      &MainWindow::OnLineCounterTriggered );
    QObject::connect( &app_settings, &utilities::AppSettings::language_added,
                      [=]( QString const name )
    {
        if( languages_menu->isEmpty() ) return;
        QActionGroup* action_group{
            qobject_cast<QActionGroup*>( languages_menu->actions()[0]->parent() )
        };
        QAction* action_language { new QAction( name, action_group ) };
        action_language->setCheckable( true );
        languages_menu->addAction( action_language );
        QObject::connect( action_language, &QAction::triggered, [=]{
            app_settings.SetDefault( languages_menu->actions().size() - 1 );
        });
    });

    QObject::connect( &app_settings, &utilities::AppSettings::language_removed,
                      [=]( int const index )
    {
        if( languages_menu->isEmpty() ) return;
        languages_menu->removeAction( languages_menu->actions()[index] );
    });
    auto const all_languages{ app_settings.GetLanguageInfo() };
    int const default_language_index{ app_settings.GetDefaultLanguageIndex() };
    if( !all_languages.isEmpty() ){
        QActionGroup *language_group{ new QActionGroup( languages_menu ) };
        for( int row = 0; row != all_languages.size(); ++row )
        {
            utilities::LanguageInfo const & language{ all_languages[row] };
            QAction* action_language {
                new QAction( language.language_name, language_group )
            };
            action_language->setCheckable( true );
            if( row == default_language_index ){
                action_language->setChecked( true );
            }
            QObject::connect( action_language, &QAction::triggered, [=]{
                app_settings.SetDefault( row );
            });
            languages_menu->addAction( action_language );
        }
        ui->menuFile->insertMenu( ui->actionLine_Counter, languages_menu );
    }
}

void MainWindow::OnSettingsTriggered()
{
    SettingsDialog* dialog{ new SettingsDialog( this ) };
    workspace->addSubWindow( dialog );
    dialog->show();
}

void MainWindow::OnLineCounterTriggered()
{
    CounterDialog* counter{ new CounterDialog( this ) };
    workspace->addSubWindow( counter );
    counter->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
