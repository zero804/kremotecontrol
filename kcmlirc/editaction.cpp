/*************************************************************************
 * Copyright            : (C) 2003 by Gav Wood <gav@kde.org>             *
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
  * @author Gav Wood
  */

#include "editaction.h"
#include "prototype.h"
#include "profileserver.h"
#include "remoteserver.h"
#include "addaction.h"
//#include "model.h"
#include "dbusinterface.h"
#include <QRegExp>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDomDocument>
#include <QDomElement>

#include <kdebug.h>
#include <klineedit.h>
#include <kapplication.h>
#include <kmessagebox.h>
#include <knuminput.h>
#include <keditlistbox.h>
#include <QAbstractItemModel>

EditAction::EditAction(IRAction *action, QWidget *parent, const bool &modal): KDialog(parent)
{



    theAction = action;
    editActionBaseWidget = new EditActionBaseWidget();
    setMainWidget(editActionBaseWidget);
    setButtons( Ok | Cancel);
    setDefaultButton(Ok);
    setModal(modal);
    //TODO: Layout theValue
    editActionBaseWidget->theDBusApplications->setModel(new DBusProfileModel(0));
    editActionBaseWidget->theDBusFunctions->setModel(new DBusFunctionModel(0));
    mainGroup.addButton(editActionBaseWidget->theUseDBus);
    mainGroup.addButton(editActionBaseWidget->theUseProfile);
    mainGroup.addButton(editActionBaseWidget->theChangeMode);

    connectSignalsAndSlots();
    //initDBusApplications();
    //initApplications();
    readFrom();

}

EditAction::~EditAction()
{
}

void EditAction::connectSignalsAndSlots() {


    //Profile Action

    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theUseProfileAppLabel,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theApplications,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theArguments,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theValue,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theAutoStart,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theFunctions,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theJustStart,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->thePerformFunction,SLOT(setChecked(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->thePerformFunction,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),editActionBaseWidget->theRepeat,SLOT(setEnabled(bool)));
connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)),this,SLOT(updateUseDbusApplicationLabL()));

    //When only just start ist checked
    connect(editActionBaseWidget->theJustStart,SIGNAL(toggled(bool)),editActionBaseWidget->theAutoStart,SLOT(setChecked(bool)));
    connect(editActionBaseWidget->theJustStart,SIGNAL(toggled(bool)),editActionBaseWidget->theFunctions,SLOT(setDisabled(bool)));
    connect(editActionBaseWidget->theJustStart,SIGNAL(toggled(bool)),editActionBaseWidget->theAppDbusOptionsLabel,SLOT(setDisabled(bool)));
    connect(editActionBaseWidget->theJustStart,SIGNAL(toggled(bool)),editActionBaseWidget->theArguments,SLOT(setDisabled(bool)));
    connect(editActionBaseWidget->theJustStart,SIGNAL(toggled(bool)),editActionBaseWidget->theValue,SLOT(setDisabled(bool)));

    connect(editActionBaseWidget->theUseProfile,SIGNAL(toggled(bool)), this,SLOT(updateApplications()));
    connect(editActionBaseWidget->theApplications,SIGNAL(currentIndexChanged ( QString)),this,SLOT(updateFunctions()));


    connect(editActionBaseWidget->theFunctions,SIGNAL(currentIndexChanged ( QString)),this,SLOT(updateArguments()));
    connect(editActionBaseWidget->theFunctions,SIGNAL(currentIndexChanged ( QString)),this,SLOT(updateInstancesOptions()));

    connect(editActionBaseWidget->thePerformFunction,SIGNAL(toggled(bool)),editActionBaseWidget->theAutoStart,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->thePerformFunction,SIGNAL(toggled(bool)),editActionBaseWidget->theFunctions,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->thePerformFunction,SIGNAL(toggled(bool)),editActionBaseWidget->theRepeat,SLOT(setChecked(bool)));
    connect(editActionBaseWidget->thePerformFunction,SIGNAL(toggled(bool)),editActionBaseWidget->theRepeat,SLOT(setEnabled(bool)));

    connect(editActionBaseWidget->thePerformFunction,SIGNAL(toggled(bool)),this,SLOT(updateFunctions()));

    //DBus Action Enabling the widgets
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theDBusApplicationsLabel,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theDBusObjectsLabel,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theDBusFunctionsLabel,SLOT(setEnabled(bool)));

    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theArguments,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theValue,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theAutoStart,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theDBusApplications,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theDBusFunctions,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theDBusObjects,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),editActionBaseWidget->theRepeat,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),this,SLOT(updateUseDbusApplicationLabL()));

    //Fill in DBUS Application Comboboxes
    connect(editActionBaseWidget->theUseDBus,SIGNAL(toggled(bool)),this, SLOT(updateDBusApplications()));
    connect(editActionBaseWidget->theDBusApplications,SIGNAL(currentIndexChanged ( QString)),this,SLOT(updateDBusObjects()));
    connect(editActionBaseWidget->theDBusObjects,SIGNAL(currentIndexChanged ( QString)),this,SLOT(updateDBusFunctions()));
    connect(editActionBaseWidget->theDBusFunctions,SIGNAL(currentIndexChanged ( QString)),this,SLOT(updateArguments()));

    //Mode Action
    connect(editActionBaseWidget->theChangeMode,SIGNAL(toggled(bool)),editActionBaseWidget->theAppDbusOptionsLabel,SLOT(setDisabled(bool)));
    connect(editActionBaseWidget->theChangeMode,SIGNAL(toggled(bool)),editActionBaseWidget->theDoAfter,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theChangeMode,SIGNAL(toggled(bool)),editActionBaseWidget->theDoBefore,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theChangeMode,SIGNAL(toggled(bool)),editActionBaseWidget->theModes,SLOT(setEnabled(bool)));
    connect(editActionBaseWidget->theChangeMode,SIGNAL(toggled(bool)),this,SLOT(updateInstancesOptions()));
    connect(editActionBaseWidget->theChangeMode,SIGNAL(toggled(bool)),this,SLOT(updateUseDbusApplicationLabL()));

    //StackView + Arguments

    connect(editActionBaseWidget->theArguments,SIGNAL(activated(int)),this,SLOT(updateArgument(int)));
    connect(editActionBaseWidget->theValueCheckBox,SIGNAL(toggled(bool)),this,SLOT(slotParameterChanged()));
    connect(editActionBaseWidget->theValueDoubleNumInput,SIGNAL(valueChanged(double)),this,SLOT(slotParameterChanged()));
    connect(editActionBaseWidget->theValueEditListBox,SIGNAL(changed()),this,SLOT(slotParameterChanged()));
    connect(editActionBaseWidget->theValueIntNumInput,SIGNAL(valueChanged(int)),this,SLOT(slotParameterChanged()));
    connect(editActionBaseWidget->theValueLineEdit,SIGNAL(textChanged(QString)),this,SLOT(slotParameterChanged()));
}



void EditAction::readFrom()
{
    editActionBaseWidget->theRepeat->setChecked(theAction->repeat());
    editActionBaseWidget->theAutoStart->setChecked(theAction->autoStart());
    editActionBaseWidget->theDoBefore->setChecked(theAction->doBefore());
    editActionBaseWidget->theDoAfter->setChecked(theAction->doAfter());
    editActionBaseWidget->theDontSend->setChecked(theAction->ifMulti() == IM_DONTSEND);
    editActionBaseWidget->theSendToTop->setChecked(theAction->ifMulti() == IM_SENDTOTOP);
    editActionBaseWidget->theSendToBottom->setChecked(theAction->ifMulti() == IM_SENDTOBOTTOM);
    editActionBaseWidget->theSendToAll->setChecked(theAction->ifMulti() == IM_SENDTOALL);

    if (theAction->isModeChange()) { // change mode
        editActionBaseWidget->theChangeMode->setChecked(true);
        if (theAction->object().isEmpty())
            editActionBaseWidget->theModes->setCurrentIndex(editActionBaseWidget->theModes->findText(i18n("[Exit current mode]")));
        else
            editActionBaseWidget->theModes->setCurrentIndex(editActionBaseWidget->theModes->findText(theAction->object()));
    } else if (theAction->isJustStart()) { // profile action
        editActionBaseWidget->theUseProfile->setChecked(true);
        const Profile *p = ProfileServer::profileServer()->profiles()[theAction->program()];
        editActionBaseWidget->theApplications->setCurrentIndex(editActionBaseWidget->theApplications->findText(p->name()));
        editActionBaseWidget->theJustStart->setChecked(true);
    } else if (ProfileServer::profileServer()->getAction(theAction->program(), theAction->object(), theAction->method().prototype())) { // profile action
        editActionBaseWidget->theUseProfile->setChecked(true);
        const ProfileAction *profileAction = ProfileServer::profileServer()->getAction(theAction->program(), theAction->object(), theAction->method().prototype());
        editActionBaseWidget->theApplications->setCurrentIndex(editActionBaseWidget->theApplications->findText(profileAction->profile()->name()));
        editActionBaseWidget->theFunctions->setCurrentIndex(editActionBaseWidget->theFunctions->findText(profileAction->name()));
        arguments = theAction->arguments();
        editActionBaseWidget->thePerformFunction->setChecked(true);
    } else { // DBus action
        editActionBaseWidget->theUseDBus->setChecked(true);
        arguments = theAction->arguments();
    }
}

void EditAction::writeBack()
{
    if (editActionBaseWidget->theChangeMode->isChecked()) {
        theAction->setProgram("");
        if (editActionBaseWidget->theModes->currentText() == i18n("[Exit current mode]"))
            theAction->setObject("");
        else
            theAction->setObject(editActionBaseWidget->theModes->currentText());
        theAction->setDoBefore(editActionBaseWidget->theDoBefore->isChecked());
        theAction->setDoAfter(editActionBaseWidget->theDoAfter->isChecked());
    } else if (editActionBaseWidget->theUseProfile->isChecked()) {
        QString application = editActionBaseWidget->theApplications->currentText();
        QString function = editActionBaseWidget->theFunctions->currentText();
        const ProfileAction *profileAction = ProfileServer::profileServer()->getAction(applicationMap[application], functionMap[function]);
        if ( profileAction  || (editActionBaseWidget->theJustStart->isChecked() &&  ProfileServer::profileServer()->profiles()[application])) {
            theAction->setProgram(ProfileServer::profileServer()->profiles()[applicationMap[application]]->id());
            if (editActionBaseWidget->theJustStart->isChecked()) {
                theAction->setObject("");
            } else {
                kDebug() << "wrote back: " << applicationMap[application];
                theAction->setObject(profileAction->objId());
                theAction->setMethod(profileAction->prototype());
                theAction->setArguments(arguments);
            }
        }
    } else {
        theAction->setProgram(getCurrentDbusApp());
        theAction->setObject(editActionBaseWidget->theDBusObjects->currentText());
        theAction->setMethod(getCurrentDbusFunction());
        theAction->setArguments(arguments);
    }
    theAction->setRepeat(editActionBaseWidget->theRepeat->isChecked());
    theAction->setAutoStart(editActionBaseWidget->theAutoStart->isChecked());
    theAction->setUnique(isUnique);

    if (editActionBaseWidget->theDontSend->isChecked()) {
        theAction->setIfMulti(IM_DONTSEND);
    } else if ( editActionBaseWidget->theSendToTop->isChecked()) {
        theAction->setIfMulti(IM_SENDTOTOP);
    } else if ( editActionBaseWidget->theSendToBottom->isChecked()) {
        theAction->setIfMulti(IM_SENDTOBOTTOM);
    } else {
        theAction->setIfMulti(IM_SENDTOALL);
    }
}

void EditAction::updateArguments()
{
    editActionBaseWidget->theArguments->clear();
    if (editActionBaseWidget->theUseProfile->isChecked()) {
        const ProfileAction *profileAction = ProfileServer::profileServer()->getAction(applicationMap[editActionBaseWidget->theApplications->currentText()], functionMap[editActionBaseWidget->theFunctions->currentText()]);
        if (!profileAction ||  editActionBaseWidget->theJustStart->isChecked()) {
            arguments.clear();
            updateArgument(-1);
            return;
        }
        const QList<ProfileActionArgument> &profileActionArguments = profileAction->arguments();
        if (profileActionArguments.count() != arguments.count()) {
            arguments.clear();
            for (int i = 0; i < profileActionArguments.count(); ++i) {
                arguments.append(QVariant(""));
            }
        }
        editActionBaseWidget->theArguments->setEnabled(profileActionArguments.count());
        for (int i = 0; i < profileActionArguments.count(); ++i) {
            editActionBaseWidget->theArguments->addItem(profileActionArguments[i].comment() + " (" + profileActionArguments[i].type() + ')');
            arguments[i].convert(QVariant::nameToType(profileActionArguments[i].type().toLocal8Bit()));
        }
        profileActionArguments.count() ? updateArgument(0) : updateArgument(-1);

    } else if ( editActionBaseWidget->theUseDBus->isChecked()) {
        Prototype p(editActionBaseWidget->theDBusFunctions->currentText());
        if (p.count() != arguments.count()) {
            arguments.clear();
            for (int i = 0; i < p.count(); i++)
                arguments.append(QVariant(""));
        }
        editActionBaseWidget->theArguments->setEnabled(p.count());
        for (int i = 0; i < p.count(); i++) {
            editActionBaseWidget->theArguments->addItem(QString().setNum(i + 1) + ": " + (p.name(i).isEmpty() ? p.type(i) : p.name(i) + " (" + p.type(i) + ')'));
            arguments[i].convert(QVariant::nameToType(p.type(i).toLocal8Bit()));
        }
        p.count() ?  updateArgument(0) : updateArgument(-1);
    }
}

void EditAction::updateInstancesOptions()
{
    if (editActionBaseWidget->theUseProfile->isChecked()) {
        ProfileServer *theServer = ProfileServer::profileServer();
        if (editActionBaseWidget->theApplications->currentIndex() == -1) return;
        const Profile *p = theServer->profiles()[applicationMap[editActionBaseWidget->theApplications->currentText()]];
        isUnique = p->unique();
    } else if ( editActionBaseWidget->theUseDBus->isChecked()) {
        program =  editActionBaseWidget->theDBusApplications->currentText();
        if ( program.isNull() ||  program.isEmpty()) {
            return;
        }
        isUnique = uniqueProgramMap[program];
    } else
        isUnique = true;

    editActionBaseWidget->theIMLabel->setEnabled(!isUnique);
    // theIMGroup->setEnabled(!isUnique);
    editActionBaseWidget->theDontSend->setEnabled(!isUnique);
    editActionBaseWidget->theSendToTop->setEnabled(!isUnique);
    editActionBaseWidget->theSendToBottom->setEnabled(!isUnique);
    editActionBaseWidget->theSendToAll->setEnabled(!isUnique);
}

// called when the textbox/checkbox/whatever changes value
void EditAction::slotParameterChanged()
{
    int index =  editActionBaseWidget->theArguments->currentIndex();
    int type = arguments[editActionBaseWidget->theArguments->currentIndex()].type();
    kDebug() << type ;
    switch (type) {
    case QVariant::Int:
    case QVariant::UInt:
        arguments[index] = editActionBaseWidget->theValueIntNumInput->value();
        break;
    case QVariant::Double:
        arguments[index] = editActionBaseWidget->theValueDoubleNumInput->value();
        break;
    case QVariant::Bool:
        arguments[index] = editActionBaseWidget->theValueCheckBox->isChecked();
        break;
    case QVariant::StringList:
        arguments[index] = editActionBaseWidget->theValueEditListBox->items();
        break;
    default:
        arguments[index] = editActionBaseWidget->theValueLineEdit->text();
    }
    arguments[index].convert(QVariant::Type(type));
    kDebug() << "out: " << arguments[index].toString() ;

}

void EditAction::updateArgument(int index)
{
kDebug()<< "update arguments";
    kDebug() << " i: " << index ;
    if (index >= 0 && ! arguments.isEmpty()) {       
        switch (arguments[index].type()) {
        
	case QVariant::StringList: {
            editActionBaseWidget->theValue->setCurrentIndex(0);
            QStringList backup = arguments[index].toStringList();
            // backup needed because calling clear will kill what ever has been saved.
            editActionBaseWidget->theValueEditListBox->clear();
            editActionBaseWidget->theValueEditListBox->insertStringList(backup);
            arguments[index] = backup;
            break;
	  }
	 case QVariant::Int:
        case QVariant::UInt:
            editActionBaseWidget->theValue->setCurrentIndex(1);
            editActionBaseWidget->theValueIntNumInput->setValue(arguments[index].toInt());
            break;        
        case QVariant::Double:
            editActionBaseWidget->theValue->setCurrentIndex(2);
            editActionBaseWidget->theValueDoubleNumInput->setValue(arguments[index].toDouble());
            break;
        case QVariant::Bool:
            editActionBaseWidget->theValue->setCurrentIndex(3);
            editActionBaseWidget->theValueCheckBox->setChecked(arguments[index].toBool());
            break;
        default:
            editActionBaseWidget->theValue->setCurrentIndex(4);
            editActionBaseWidget->theValueLineEdit->setText(arguments[index].toString());
        }
        editActionBaseWidget->theValue->setEnabled(true);
    } else {
        editActionBaseWidget->theValueLineEdit->setText("");
        editActionBaseWidget->theValueCheckBox->setChecked(false);
        editActionBaseWidget->theValueIntNumInput->setValue(0);
        editActionBaseWidget->theValueDoubleNumInput->setValue(0.0);
        editActionBaseWidget->theValue->setEnabled(false);
	editActionBaseWidget->theValue->setCurrentIndex(5);
    }
}

void EditAction::updateApplications()
{
    ProfileServer *theServer = ProfileServer::profileServer();
    editActionBaseWidget->theApplications->clear();
    applicationMap.clear();

    QHash<QString, Profile*> dict = theServer->profiles();
    QHash<QString, Profile*>::const_iterator i;
    QStringList tList;
    for (i = dict.constBegin(); i != dict.constEnd(); ++i) {
        tList << i.value()->name();
        applicationMap[i.value()->name()] = i.key();

    }
    tList.sort();
    editActionBaseWidget->theApplications->addItems(tList);
}

void EditAction::updateFunctions()
{
  kDebug()<< "Updating dbus  functions";
    editActionBaseWidget->theFunctions->clear();
    functionMap.clear();

    if (editActionBaseWidget->theJustStart->isChecked()) {
     
        return;
    }
    QString application = editActionBaseWidget->theApplications->currentText();
    if (application.isNull() || application.isEmpty()) {
        return;
    }
    QHash<QString, ProfileAction*> dict = ProfileServer::profileServer()->profiles()[applicationMap[application]]->actions();
    kDebug() << dict;
    QHash<QString, ProfileAction*>::const_iterator i;
    QStringList theFunctions;
    for (i = dict.constBegin(); i != dict.constEnd(); ++i) {
        theFunctions << i.value()->name();
        functionMap[i.value()->name()] = i.key();
    }
    theFunctions.sort();
    editActionBaseWidget->theFunctions->addItems(theFunctions);
//    updateArguments();
}

void EditAction::updateDBusApplications()
{
kDebug()<< "update dbus applications";
    //TODO: handling if iraction is dbusaction and application is not running
    editActionBaseWidget->theDBusApplications->clear();
    editActionBaseWidget->theDBusApplications->addItems(DBusInterface::getInstance()->getRegisteredPrograms());
    editActionBaseWidget->theDBusApplications->model()->sort( Qt::AscendingOrder);
    int ti = editActionBaseWidget->theDBusApplications->findData(theAction->program());
    editActionBaseWidget->theDBusApplications->setCurrentIndex(ti == -1 ? 0 : ti);

}

void EditAction::updateDBusObjects()
{
      kDebug()<< "update dbus objects";
      //TODO: handling if iraction is dbusaction and application is not running
    editActionBaseWidget->theDBusObjects->clear();
    editActionBaseWidget->theDBusObjects->insertItems(0, DBusInterface::getInstance()->getObjects(getCurrentDbusApp()));
    editActionBaseWidget->theDBusObjects->model()->sort( Qt::AscendingOrder);
    int ti = editActionBaseWidget->theDBusObjects->findText(theAction->object());
    editActionBaseWidget->theDBusObjects->setCurrentIndex(ti == -1 ? 0 : ti);
}

void EditAction::updateDBusFunctions()
{
    //TODO: handling if iraction is dbusaction and application is not running
    editActionBaseWidget->theDBusFunctions->clear();
    QList<Prototype> tList = DBusInterface::getInstance()->getFunctions(getCurrentDbusApp(), editActionBaseWidget->theDBusObjects->currentText());
    foreach(Prototype tType, tList) {	
        editActionBaseWidget->theDBusFunctions->addItem(0, qVariantFromValue(tType));	
    }
    editActionBaseWidget->theDBusFunctions->model()->sort( Qt::AscendingOrder);
    int ti =editActionBaseWidget->theDBusFunctions->findData(qVariantFromValue(theAction->method()));
    editActionBaseWidget->theDBusFunctions->setCurrentIndex(ti == -1 ? 0 : ti);
}

void EditAction::addItem(QString item)
{
    editActionBaseWidget->theModes->addItem(item);
}

void EditAction::updateUseDbusApplicationLabL() {
QString labelDesc;
if(editActionBaseWidget->theUseProfile->isChecked()){
labelDesc=i18n("Profile argument options:");
}
else if(editActionBaseWidget->theUseDBus->isChecked()){
labelDesc=i18n("DBus function arguments:");
}else{
labelDesc=i18n("A&pplication / D-Bus options:");
}
editActionBaseWidget->theAppDbusOptionsLabel->setText(labelDesc);
}

#include "editaction.moc"
