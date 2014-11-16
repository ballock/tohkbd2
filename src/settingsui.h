/*
    tohkbd2-settings-u, The Otherhalf Keyboard 2 settings UI
*/

#ifndef SETTINGSUI_H
#define SETTINGSUI_H
#include <QObject>
#include <QVariantList>

class SettingsUi : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString version READ readVersion NOTIFY versionChanged())

public:
    explicit SettingsUi(QObject *parent = 0);
    ~SettingsUi();

    QString readVersion();
    Q_INVOKABLE QVariantList getApplications();
    Q_INVOKABLE void startApplication(QString appname);

signals:
    void versionChanged();

    //private:

};


#endif // SETTINGSUI_H
