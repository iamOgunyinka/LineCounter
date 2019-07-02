#include "counterdialog.hpp"
#include "ui_counterdialog.h"

#include <QFileInfo>
#include <QDirIterator>
#include <QTextStream>
#include <QMessageBox>
#include <QCompleter>
#include <QFileSystemModel>

#include "resources.hpp"

CounterDialog::CounterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CounterDialog)
{
    ui->setupUi( this );
    QCompleter* completer = new QCompleter( this );
    completer->setModel( new QFileSystemModel( completer ) );

    ui->dir_name_edit->setCompleter( completer );
    QObject::connect( ui->start_button, &QPushButton::clicked, this,
                      &CounterDialog::OnStartButtonClicked );

}

CounterDialog::~CounterDialog()
{
    delete ui;
}

void CounterDialog::CheckFile( QString const & name )
{
    QFile file{ name };
    if( !file.open( QIODevice::ReadOnly )) return;
    QTextStream out_file{ &file };
    qint64 line = 0;
    while( !out_file.atEnd() ){
        out_file.readLineInto( nullptr );
        ++line;
    }
    file_list_info.append( FileInfo{ name, line } );
}

void CounterDialog::CheckDirectory( QString const & name,
                                    QStringList const & extensions )
{
    QDirIterator iter{ name, QDirIterator::Subdirectories
                | QDirIterator::FollowSymlinks };
    while( iter.hasNext() ){
        auto const current_file{ iter.fileInfo() };
        if( current_file.isFile() &&
                extensions.contains( current_file.completeSuffix() ) ){
            CheckFile( iter.filePath() );
        }
        iter.next();
    }
}

void CounterDialog::OnStartButtonClicked()
{
    QString const name{ ui->dir_name_edit->text().trimmed() };
    QFileInfo const file_info{ name };
    if( !file_info.exists() ){
        QMessageBox::information( this, "Error",
                                  "Does not exist or not a valid directory" );
        return;
    }
    auto const &app_settings{ utilities::AppSettings::GetAppSettings() };
    auto const languages{ app_settings.GetLanguageInfo() };
    int const default_language_index { app_settings.GetDefaultLanguageIndex() };

    if( file_info.isFile() ) CheckFile( name );
    else CheckDirectory( name, languages[default_language_index].extensions );
    if( file_list_info.isEmpty() ){
        FileInfoModel* previous_model{
            qobject_cast<FileInfoModel*>( ui->tableView->model() )
        };
        delete previous_model;
        ui->tableView->setModel( nullptr );
        QMessageBox::information( this, "Find", "Nothing found" );
    } else DisplayInfo();
}

void CounterDialog::DisplayInfo()
{
    ui->tableView->setVisible( false );
    FileInfoModel* previous_model{
        qobject_cast<FileInfoModel*>( ui->tableView->model() )
    };
    ui->tableView->setModel( new FileInfoModel( file_list_info, ui->tableView ));
    delete previous_model;
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setSectionResizeMode(
                QHeaderView::Stretch );
    ui->tableView->setVisible( true );
}

FileInfoModel::FileInfoModel( QVector<FileInfo> &file_info, QObject *parent ):
    QAbstractTableModel ( parent ), file_info_{ file_info }
{
}

Qt::ItemFlags FileInfoModel::flags( QModelIndex const & ) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

int FileInfoModel::rowCount( QModelIndex const & ) const
{
    return file_info_.size() + 2;
}

int FileInfoModel::columnCount( QModelIndex const & ) const
{
    return 2;
}

QVariant FileInfoModel::data( QModelIndex const &index, int role ) const
{
    int const column{ index.column() };
    int const row{ index.row() };
    int const row_count{ file_info_.size() };

    if( role == Qt::DisplayRole ){
        if( row < row_count ){
            FileInfo const & info{ file_info_[index.row()] };
            switch( column ){
            case 0:
                return info.filename;
            case 1:
                return info.number_of_lines;
            default:
                return QVariant{};
            }
        } else if( row == row_count ) {
            return "";
        } else {
            qint64 total_line_number{};
            for( auto const &info: file_info_ )
                total_line_number += info.number_of_lines;
            switch( column ){
            case 0:
                return QString( "Total files(%1)" ).arg( row_count );
            case 1:
                return total_line_number;
            default:
                return QVariant{};
            }
        }
    } else if( role == Qt::ToolTipRole ) {
        return data( index, Qt::DisplayRole );
    }
    return QVariant{};
}

QVariant FileInfoModel::headerData( int section, Qt::Orientation orientation,
                                    int role) const
{
    if( role != Qt::DisplayRole ) return QVariant{};
    if( orientation == Qt::Horizontal ){
        switch( section ){
        case 0:
            return QString( "Filename" );
        case 1:
            return QString( "LOC" );
        default:
            return QVariant{};
        }
    }
    return section + 1;
}
