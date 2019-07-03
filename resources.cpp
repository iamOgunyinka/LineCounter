#include "resources.hpp"
#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>

namespace utilities
{
QJsonObject LanguageInfo::to_json() const
{
    QJsonObject result { { "name", language_name } };
    QJsonArray extension_list{};
    for( auto const & ext: extensions ) extension_list.append( ext );
    result["extensions"] = extension_list;
    return result;
}

QStringList LanguageInfo::from_json( QJsonArray const & extensions )
{
    QStringList list {};
    for( QJsonValue const ext: extensions ) list.append( ext.toString() );
    return list;
}

LanguageInfo::LanguageInfo( QString const & name, QJsonArray const & list ):
    language_name{ name }, extensions{ from_json( list ) }
{
}

AppSettings& AppSettings::GetAppSettings()
{
    static AppSettings app_settings{};
    return app_settings;
}

void AppSettings::SetDefault( int value )
{
    if( value == GetDefaultLanguageIndex() ) return;
    settings.setValue( "default", value );
    emit default_changed( value );
}

void AppSettings::WriteSettings()
{
    QJsonArray extensions {};
    for( auto const & lang: languages ){
        QJsonArray exts {};
        for( auto const & ext: lang.extensions ) exts.append( ext );
        extensions.append( QJsonObject { { "name", lang.language_name },
                                         { "extensions", exts } } );
    }
    if( extensions.isEmpty() ) {
        settings.remove( "extensions" );
        settings.remove( "default" );
    } else {
        settings.setValue( "default", GetDefaultLanguageIndex() );
        settings.setValue( "extensions", QJsonDocument{ extensions }.toJson() );
    }
}

AppSettings::AppSettings():
    settings{ GetSettings( "Froist Inc.", "LineCounter" ) }
{
    QVariant extensions{ settings.value( "extensions" ) };
    QByteArray const app_settings_data{ extensions.toByteArray() };

    if( !extensions.isValid() || app_settings_data.isEmpty() ) return;
    QJsonArray language_list {
        QJsonDocument::fromJson( app_settings_data ).array()
    };
    while( !language_list.isEmpty() ){
        QJsonValue const value{ language_list[0] };
        if( value.isArray() ) language_list = value.toArray();
        else break;
    }
    for( auto const lang: language_list ){
        QJsonObject const object{ lang.toObject() };
        LanguageInfo language{ object.value( "name").toString(),
                    object.value( "extensions" ).toArray() };
        if( !language.language_name.isEmpty() ) languages.append( language );
    }
}

int AppSettings::GetDefaultLanguageIndex() const
{
    QVariant default_{ settings.value( "default", 0 ) };
    return default_.toInt();
}

QSettings &AppSettings::GetSettings( QString const &organisation,
                                     QString const &application )
{
    static QSettings settings{ organisation, application };
    return settings;
}

QVector<LanguageInfo> const & AppSettings::GetLanguageInfo() const
{
    return languages;
}

void AppSettings::RemoveLanguage( int const index )
{
    languages.remove( index );
    emit language_removed( index );
}

void AppSettings::RemoveExtension( int const index, int const ext_index )
{
    languages[index].extensions.removeAt( ext_index );
}

void AppSettings::AddLanguage( QString const name )
{
    languages.append( LanguageInfo( name, QJsonArray{} ) );
    emit language_added( name );
}

void AppSettings::AddExtension(const int index, QStringList list)
{
    languages[index].extensions.append( list );
}

}
