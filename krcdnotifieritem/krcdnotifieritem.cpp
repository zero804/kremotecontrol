/*
    Copyright (C) 2010 Michael Zanetti <michael_zanetti@gmx.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "krcdnotifieritem.h"
#include "dbusinterface.h"
#include "remotecontrolmanager.h"

#include <klocalizedstring.h>
#include <kicon.h>
#include <kiconloader.h>
#include <ktoolinvocation.h>
#include <kdebug.h>

#include <QtCore/QTimer>
#include <QtDBus/QDBusConnection>

KrcdNotifierItem::KrcdNotifierItem(){
    setCategory(KStatusNotifierItem::Hardware);
    updateTray();
    setContextMenu(&m_menu);

    // No need for close button...
    setStandardActionsEnabled(false);
    setAssociatedWidget(&m_menu);
    updateContextMenu();
    QDBusConnection::sessionBus().connect(QLatin1String( "org.kde.kded" ), QLatin1String( "/modules/kremotecontroldaemon" ), QLatin1String( "org.kde.krcd" ), QLatin1String( "connectionChanged" ),  this, SLOT(updateTray()));
    QDBusConnection::sessionBus().connect(QLatin1String( "org.kde.kded" ), QLatin1String( "/modules/kremotecontroldaemon" ), QLatin1String( "org.kde.krcd" ), QLatin1String( "remoteControlAdded" ),  this, SLOT(updateTray()));
    QDBusConnection::sessionBus().connect(QLatin1String( "org.kde.kded" ), QLatin1String( "/modules/kremotecontroldaemon" ), QLatin1String( "org.kde.krcd" ), QLatin1String( "remoteControlAdded" ),  this, SLOT(updateContextMenu()));
    QDBusConnection::sessionBus().connect(QLatin1String( "org.kde.kded" ), QLatin1String( "/modules/kremotecontroldaemon" ), QLatin1String( "org.kde.krcd" ), QLatin1String( "remoteControlRemoved" ),  this, SLOT(updateTray()));
    QDBusConnection::sessionBus().connect(QLatin1String( "org.kde.kded" ), QLatin1String( "/modules/kremotecontroldaemon" ), QLatin1String( "org.kde.krcd" ), QLatin1String( "remoteControlRemoved" ),  this, SLOT(updateContextMenu()));
    QDBusConnection::sessionBus().connect(QLatin1String( "org.kde.kded" ), QLatin1String( "/modules/kremotecontroldaemon" ), QLatin1String( "org.kde.krcd" ), QLatin1String( "buttonPressed" ),  this, SLOT(flash()));
    QDBusConnection::sessionBus().connect(QLatin1String( "org.kde.kded" ), QLatin1String( "/modules/kremotecontroldaemon" ), QLatin1String( "org.kde.krcd" ), QLatin1String( "modeChanged" ),  this, SLOT(updateContextMenu()));
}

void KrcdNotifierItem::updateTray() {
    QString toolTipHeader = i18n("Remote Controls\n");
    QString toolTip;
    QString icon = QLatin1String( "krcd" );
    if (!RemoteControlManager::connected()) {
        toolTipHeader += i18nc("The state of kremotecontrol", "Stopped");
        toolTip += i18n("No Remote Control is currently available.");
        icon = QLatin1String( "krcd_off" );
        setStatus(Passive);
    } else {
        toolTipHeader += i18nc("The state of kremotecontrol", "Ready");
        foreach(const QString &remote, DBusInterface::getInstance()->configuredRemotes()) {
            QString mode = DBusInterface::getInstance()->currentMode(remote);
            toolTip += remote + QLatin1String( " <i>(" ) + mode + QLatin1String( ")</i><br>" );
        }
        setStatus(Active);
    }
    setToolTip(QLatin1String( "infrared-remote" ), toolTipHeader, toolTip);
    setIconByName(icon);
}

void KrcdNotifierItem::updateContextMenu(){
    m_menu.clear();
    m_menu.addTitle(KIcon( QLatin1String( "infrared-remote")), i18n( "Remote Controls" ) );
    m_menu.addAction(SmallIcon(QLatin1String( "configure" )), i18n("&Configure..."), this, SLOT(slotConfigure()));

    foreach(const QString &remote, RemoteControl::allRemoteNames()){
        KMenu *modeMenu = new KMenu(remote, &m_menu);
        QActionGroup *actionGroup = new QActionGroup(modeMenu);
        actionGroup->setExclusive(true);
        modeMenu->addTitle(KIcon( QLatin1String( "infrared-remote")), i18n("Switch mode to" ));
        foreach(const QString &mode, DBusInterface::getInstance()->modesForRemote(remote)){
            QAction *entry = modeMenu->addAction(mode);
            entry->setActionGroup(actionGroup);
            entry->setCheckable(true);
            entry->setIcon(KIcon(DBusInterface::getInstance()->modeIcon(remote, mode)));
            if(DBusInterface::getInstance()->currentMode(remote) == mode){
                entry->setChecked(true);
            }
            entry->setData(QStringList() << remote << mode);
        }
        modeMenu->addSeparator();
        QAction *entry = modeMenu->addAction(i18n("Pause remote"));
        entry->setCheckable(true);
        entry->setData(QStringList() << remote);
        if(DBusInterface::getInstance()->eventsIgnored(remote)){
            entry->setChecked(true);
        }
        m_menu.addMenu(modeMenu);
        connect(modeMenu, SIGNAL(triggered(QAction*)), this, SLOT(slotModeSelected(QAction*)));

    }
}

void KrcdNotifierItem::slotConfigure() {
    KToolInvocation::startServiceByDesktopName(QLatin1String( "kcm_remotecontrol" ));
}

void KrcdNotifierItem::slotModeSelected(QAction* action) {
    if(action->data().toStringList().count() > 1){
        QString remote = action->data().toStringList().first();
        QString mode = action->data().toStringList().last();
        DBusInterface::getInstance()->changeMode(remote, mode);
        action->setChecked(true);
    } else {
        if(action->isChecked()){
            DBusInterface::getInstance()->ignoreButtonEvents(action->data().toStringList().first());
        } else {
            DBusInterface::getInstance()->considerButtonEvents(action->data().toStringList().first());
        }
    }
    updateTray();
}

void KrcdNotifierItem::flash() {
    setIconByName(QLatin1String( "krcd_flash" ));
    QTimer::singleShot(200, this, SLOT(flashOff()));
}

void KrcdNotifierItem::flashOff() {
    setIconByName(QLatin1String( "krcd" ));
}
