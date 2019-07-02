#ifndef RESOURCES_HPP
#define RESOURCES_HPP

#include <QSettings>
#include <QJsonObject>
#include <QVector>

namespace utilities
{
struct LanguageInfo
{
    QString     language_name {};
    QStringList extensions {};
    LanguageInfo() = default;
    LanguageInfo( QString const & name, QJsonArray const & list );
    QJsonObject to_json() const;
    static QStringList from_json( QJsonArray const & );
};

class AppSettings: public QObject
{
    Q_OBJECT

signals:
    void language_added( QString const name );
    void language_removed( int index );
    void language_edited( int index );
    void default_changed( int index );
private:
    AppSettings();
public:
    static AppSettings& GetAppSettings();
    int GetDefaultLanguageIndex() const;
    QVector<LanguageInfo> const & GetLanguageInfo() const;
    void WriteSettings();
    void SetDefault( int value );
    void RemoveLanguage( int const index );
    void AddLanguage( QString const name );
    void RemoveExtension( int const index, int const ext_index );
    void AddExtension( int const index, QStringList list );
private:
    static QSettings& GetSettings( QString const & organisation,
                                   QString const & application );
private:
    QSettings& settings;
    QVector<LanguageInfo> languages {};
};

}
#endif // RESOURCES_HPP
