/*************************************************************************
 * Copyright            : (C) 2002 by Gav Wood <gav@kde.org>             *
 *                                                                       *
 * This program is free software; you can redistribute it and/or         *
 * modify it under the terms of the GNU General Public License as        *
 * published by the Free Software Foundation; either version 2 of        *
 * the License or (at your option) version 3 or any later version        *
 * accepted by the membership of KDE e.V. (or its successor approved     *
 * by the membership of KDE e.V.), which shall act as a proxy            *
 * defined in Section 14 of version 3 of the license.                    *
 *                                                                       *
 * This program is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have received a copy of the GNU General Public License     *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *************************************************************************/


/**
  * @author Gav Wood, Michael Zanetti
  */

// irkick.cpp  -  Implementation of the main window

#include "irkick.h"
#include "profileserver.h"
#include "irkickadaptor.h"
#include "dbusinterface.h"
#include "profile.h"
#include "newprofileserver.h"
#include "modeswitchaction.h"
#include "profileaction.h"
#include "actionlist.h"
#include "executionengine.h"

#include <QtDBus/qdbusconnection.h>

#include <kapplication.h>
#include <kactioncollection.h>
#include <ksystemtrayicon.h>
#include <kmessagebox.h>
#include <kmenu.h>
#include <kdebug.h>
#include <klocale.h>
#include <kwindowsystem.h>
#include <kconfiggroup.h>
#include <knotification.h>
#include <ktoolinvocation.h>
#include <khelpmenu.h>
#include <kdelirc/kdelirc/dbusinterface.h>

using namespace Solid::Control;

IRKick::IRKick():
        KStatusNotifierItem(), npApp(QString())
{

    setIconByName("irkick");
    setCategory(Hardware);

    m_menu = new KMenu(associatedWidget());
    setContextMenu(m_menu);
    
    // We'll add the quit action ourselves because we have to clear() and repopulate the menu later
    setStandardActionsEnabled(false); 
    
    new IrkickAdaptor(this);
    QDBusConnection dBusConnection = QDBusConnection::sessionBus();
    dBusConnection.registerObject("/IRKick", this,
                                  QDBusConnection::ExportAllSlots);

    

    kDebug() << "loading solid";
                                  
    if (!Solid::Control::RemoteControlManager::connected()) {
        kDebug() << "Lirc not ready yet...";
    }
    theFlashOff = new QTimer(this);
    theFlashOff->setSingleShot(true);
    connect(theFlashOff, SIGNAL(timeout()), SLOT(flashOff()));

    theResetCount = 0;
    resetModes();
    slotReloadConfiguration();
    connect(RemoteControlManager::notifier(), SIGNAL(statusChanged(bool)), this, SLOT(slotStatusChanged(bool)));
    foreach(const QString &remote, RemoteControl::allRemoteNames()){
        RemoteControl *rc = new RemoteControl(remote);
        kDebug() << "connecting to remote" << remote;
        connect(rc, 
                SIGNAL(buttonPressed(const Solid::Control::RemoteControlButton &)), 
                this, 
                SLOT(gotMessage(const Solid::Control::RemoteControlButton &)));
    }











// this is some test and sample code for the new Action/Profile framework
// create a new  ProfileActionTemplate
  NewProfile profile("testprofile");
  Prototype prototype("void toggleDashboard()");
  QList<NewArgument> argumentList;
  QVariant parameter(QVariant::Bool);
  parameter.setValue(true);
  NewArgument argument(parameter, "This is the testparm for testfunc. True or False");
//  argumentList.append(argument);
  ProfileActionTemplate actionTemplate(profile.name(),
				 "testtemplate",
				 "org.kde.plasma-desktop",
				 "Desktop",
				 "App",
				 prototype,
				 "A simplete test function",
				 argumentList,
				 NewProfileAction::Unique,
				 true,
				 true,
				 "Menu");
  profile.addTemplate(actionTemplate);
  
  QString remote = Solid::Control::RemoteControl::allRemotes().first()->name();
  
  
  // test for actionTemaplateList() (aka. Autopopulate)
  NewProfileAction *origProfileAction;
  foreach(const ProfileActionTemplate &tmp, NewProfileServer::actionTemplateList(remote, profile)){
    kDebug() << "ActionTemplate " << tmp.templateID() << "matches";
    origProfileAction = tmp.createAction(Solid::Control::RemoteControlButton(remote,"Menu"));
  }
  
  // Test for ModeSwitchAction
  ModeSwitchAction *origModeSwitchAction = new ModeSwitchAction(Solid::Control::RemoteControlButton(remote,"Menu"));
  origModeSwitchAction->setNewMode(Mode(remote,"Testmode"));
  origModeSwitchAction->setExecuteActionsAfterSwitch(true);
  
  // Testind ActionList
  ActionList actionList;
  actionList.loadFromConfig();
  actionList.addAction(origProfileAction);
  actionList.addAction(origModeSwitchAction);
  
  // Test for casting Actions
  foreach(Action *action, actionList.allActions()){
    switch(action->type()){
      case Action::ModeSwitchAction:{
	kDebug() << "Action is a ModeSwitchAction";
	ModeSwitchAction *modeSwitchAction = dynamic_cast<ModeSwitchAction*>(action);
	kDebug() << "Modeswitch to:" << modeSwitchAction->newMode().name() << "executeAfter:" << modeSwitchAction->executeActionsAfterSwitch();
	}
	break;
      case Action::DBusAction:
      case Action::ProfileAction:
	kDebug() << "Action is a DBusAction";
	DBusAction *dbusAction = dynamic_cast<DBusAction*>(action);
	if(dbusAction){
	  kDebug() << "cast ok";
	  kDebug() << "function prototype is: " << dbusAction->function().prototype();
//	  kDebug() << "argument is" << dbusAction->arguments().first().value() << dbusAction->arguments().first().description();
	} 
	ExecutionEngine::executeAction(dbusAction);
	  
	
	break;
	
      
    }
  }
  delete origProfileAction;
  delete origModeSwitchAction;











}

IRKick::~IRKick()
{
}

void IRKick::slotStatusChanged(bool connected)
{
  
    if(connected){
    KNotification::event("global_event", i18n("A connection to the infrared system has been made. Remote controls may now be available."),
                SmallIcon("irkick"), associatedWidget());
    updateTray();
    foreach(const QString &remote, RemoteControl::allRemoteNames()){
        RemoteControl *rc = new RemoteControl(remote);
        kDebug() << "connecting to remote" << remote;
        connect(rc, 
                SIGNAL(buttonPressed(const Solid::Control::RemoteControlButton &)), 
                this, 
                SLOT(gotMessage(const Solid::Control::RemoteControlButton &)));
    }
    resetModes();
    } else {
      KNotification::event("global_event", i18n("The infrared system has severed its connection. Remote controls are no longer available."), SmallIcon("irkick"), associatedWidget());
      updateTray();
    }
}

void IRKick::flashOff()
{
    setIconByName("irkick");
}

void IRKick::resetModes()
{
    kDebug()<< "resseting modes";
    if (theResetCount > 1) {
        KNotification::event("global_event", i18n("Resetting all modes."),
                             SmallIcon("irkick"), associatedWidget());
    }

    if (!theResetCount)
        allModes.generateNulls(Solid::Control::RemoteControl::allRemoteNames());

    foreach (const QString &remote, Solid::Control::RemoteControl::allRemoteNames()) {
        kDebug() << "adding remote" << remote << "to modes";
        currentModes[remote] = allModes.getDefault(remote).name();
    }
    updateContextMenu();
    updateTray();
    ++theResetCount;
}

void IRKick::slotReloadConfiguration()
{
    // load configuration from config file
    KConfig theConfig("irkickrc");
    allActions.loadFromConfig(theConfig);
    allModes.loadFromConfig(theConfig);
    if (currentModes.count() && theResetCount) {
        kDebug()<< "reloading conf";
        resetModes();
    }
}

void IRKick::slotConfigure()
{
    KToolInvocation::startServiceByDesktopName("kcm_lirc");
}

void IRKick::slotModeSelected(QAction *action)
{
    Mode mode = qVariantValue<Mode>(action->data());
    currentModes[mode.remote()] = mode.name();
    action->setChecked(true);
    updateTray();
}

void IRKick::updateTray()
{
    QString toolTipHeader = i18n("KDE Lirc Server: ");
    QString toolTip;
    QString icon = "irkick";
    if (!Solid::Control::RemoteControlManager::connected()) {
        toolTipHeader += i18nc("The state of kdelirc", "Stopped");
        toolTip += i18n("Lirc daemon is currently not available.");
        icon = "irkickoff";
        setStatus(Passive);
    } else if (currentModes.size() == 0) {
        toolTipHeader += i18nc("The state of kdelirc", "Stopped");
        toolTip += i18n("No infra-red remote controls found.");
        setStatus(Passive);
    } else {
        toolTipHeader += i18nc("The state of kdelirc", "Ready");
        for (QMap<QString, QString>::const_iterator i = currentModes.constBegin(); i != currentModes.constEnd(); ++i) {
            Mode mode = allModes.getMode(i.key(), i.value());
            toolTip += mode.remote() + " <i>(";
            toolTip += mode.name().isEmpty() ? i18n("Master") : mode.name();
            toolTip +=")</i><br>";
        }
        setStatus(Active);
    }
    setToolTip("infrared-remote", toolTipHeader, toolTip);
    setIconByName(icon);
}

void IRKick::updateContextMenu(){
    m_menu->clear();
    m_menu->addTitle(KIcon("infrared-remote"), "IRKick");
    m_menu->addAction(SmallIcon("configure"), i18n("&Configure..."), this, SLOT(slotConfigure()));

    modeActions.clear();
    
    foreach(const QString &remote, RemoteControl::allRemoteNames()){
        KMenu *modeMenu = new KMenu(remote, m_menu);
        QActionGroup *actionGroup = new QActionGroup(modeMenu);
	modeActions.insert(remote, actionGroup);
        actionGroup->setExclusive(true);
        modeMenu->addTitle(KIcon("infrared-remote"), i18n("Switch mode to"));
        foreach(const Mode &mode, allModes.getModes(remote)){
            QAction *entry = modeMenu->addAction(mode.name().isEmpty() ? i18n("Master") : mode.name());
            entry->setActionGroup(actionGroup);
            entry->setCheckable(true);
            if(currentModes[remote] == mode.name()){
                entry->setChecked(true);
            }
            entry->setData(qVariantFromValue(mode));
        }
        m_menu->addMenu(modeMenu);
        connect(modeMenu, SIGNAL(triggered(QAction*)), this, SLOT(slotModeSelected(QAction*)));

    }

    KHelpMenu *helpMenu = new  KHelpMenu(0, KGlobal::mainComponent().aboutData());
    m_menu->addAction(KIcon("help-contents"), i18n("&Help"), helpMenu, SLOT(appHelpActivated()));
    m_menu->addAction(KIcon("irkick"), i18n("&About"), helpMenu,SLOT(aboutApplication()));

    m_menu->addSeparator();
    m_menu->addAction(actionCollection()->action(KStandardAction::name(KStandardAction::Quit)));
}

void IRKick::gotMessage(const RemoteControlButton &button)
{
    kDebug() << "Got message: " << button.remoteName() << ": " << button.name() << " (" << button.repeatCounter() << ")";
    if (!npApp.isEmpty()) {
        QString theApp = npApp;
        npApp.clear();
        // send notifier by DBUS to npApp/npModule/npMethod(theRemote, theButton);
        kDebug() << "Sending keypress to: " << theApp << ":" << npModule << ":" << npMethod;
        kDebug() << "Parameters: " << button.remoteName() << button.name();
        QDBusMessage m = QDBusMessage::createMethodCall(theApp, npModule, "",
                         npMethod);
        m << button.remoteName() << button.name();
        QDBusMessage response = QDBusConnection::sessionBus().call(m);
        if (response.type() == QDBusMessage::ErrorMessage) {
            kDebug() << response.errorMessage();
        }
    } else {
        if (currentModes[button.remoteName()].isNull()) {
            currentModes[button.remoteName()] = "";
        }
        kDebug() << "current mode:" << currentModes[button.remoteName()];
        IRActions tActions = allActions.findByModeButton(Mode(button.remoteName(), currentModes[button.remoteName()]), button.name());
        if (!currentModes[button.remoteName()].isEmpty())
            tActions += allActions.findByModeButton(Mode(button.remoteName(), ""), button.name());
        bool doBefore = true, doAfter = false;
        for (int i = 0; i < tActions.size(); ++i)
            if (tActions.at(i)->isModeChange() && !button.repeatCounter()) { // mode switch
                currentModes[button.remoteName()] = tActions.at(i)->modeChange();
                Mode mode = allModes.getMode(button.remoteName(), tActions.at(i)->modeChange());
                updateTray();
                foreach(QAction *action, modeActions[mode.remote()]->actions()){
                    if(qVariantValue<Mode>(action->data()) == mode){
                        action->setChecked(true);
                    }
                }
                doBefore = tActions.at(i)->doBefore();
                doAfter = tActions.at(i)->doAfter();
                KNotification::event(
                               "mode_event", "<b>" + mode.remote() + ":</b><br>" +
                               i18n("Mode switched to %1" , currentModes[button.remoteName()].isEmpty() ? i18nc("Default mode in notification", "Default") : currentModes[button.remoteName()]),
                               DesktopIcon(mode.iconFile().isEmpty() ? "infrared-remote" : mode.iconFile()),
                               associatedWidget());
                break;
            }

        for (int after = 0; after < 2; after++) {
            if ((doBefore && !after) || (doAfter && after))
                for (int i = 0; i < tActions.size(); ++i) {
                    if (!tActions.at(i)->isModeChange() && (tActions.at(i)->repeat() || !button.repeatCounter())) {
                        DBusInterface::getInstance()->executeAction(*tActions.at(i));
                    }
                }
            if (!after && doAfter) {
                tActions = allActions.findByModeButton(Mode(button.remoteName(), currentModes[button.remoteName()]), button.name());
                if (!currentModes[button.remoteName()].isEmpty()) {
                    tActions += allActions.findByModeButton(Mode(button.remoteName(), ""), button.name());
                }
            }
        }
    }
    setIconByName("irkickflash");
    theFlashOff->start(200);
}

const QStringList IRKick::buttons(QString theRemote) {
    QStringList retList;
    foreach(const Solid::Control::RemoteControlButton &button, Solid::Control::RemoteControl(theRemote).buttons()){
        retList.append(button.name());
    }
    return retList;
}

void IRKick::stealNextPress(QString app, QString module, QString method)
{
    npApp = app;
    npModule = module;
    npMethod = method;
}

void IRKick::dontStealNextPress()
{
    npApp.clear();
}

#include "irkick.moc"
