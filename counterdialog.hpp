#ifndef COUNTERDIALOG_HPP
#define COUNTERDIALOG_HPP

#include <QDialog>
#include <QAbstractTableModel>

namespace Ui {
class CounterDialog;
}

struct FileInfo{
    QString filename;
    qint64  number_of_lines;
};

class CounterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CounterDialog(QWidget *parent = nullptr);
    ~CounterDialog();

private:
    void OnStartButtonClicked();
    void CheckFile( QString const & name );
    void CheckDirectory( QString const & name,
                         QStringList const &extensions );
    void DisplayInfo();
private:
    Ui::CounterDialog *ui;
    QVector<FileInfo> file_list_info{};
};


class FileInfoModel: public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit FileInfoModel( QVector<FileInfo>& file_info,
                            QObject* parent = nullptr );
    Qt::ItemFlags flags( QModelIndex const & index ) const override;
    int columnCount( QModelIndex const &parent = QModelIndex() ) const override;
    QVariant data( QModelIndex const & index, int role = Qt::EditRole
            ) const override;
    QVariant headerData( int section, Qt::Orientation orientation,
                         int role = Qt::DisplayRole ) const override;
    int rowCount( QModelIndex const &parent = QModelIndex{} ) const override;

private:
    QVector<FileInfo> &file_info_;
};


#endif // COUNTERDIALOG_HPP
