/*************************************************************************
 * Copyright (C) 2010 by Michael Zanetti <michael_zanetti@gmx.net>       *
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

#ifndef KCMREMOTECONTROL_H
#define KCMREMOTECONTROL_H

#include "ui_configurationwidget.h"
#include "model.h"
#include "action.h"
#include "remotelist.h"

#include <kcmodule.h>

#include <QtCore/QMap>
#include <QtCore/QVariant>
#include <QtGui/QStandardItemModel>

class Profile;
class Remote;

class KCMRemoteControl: public KCModule
{
    Q_OBJECT

private:
    Ui::ConfigurationWidget ui;
    
    RemoteList m_remoteList;
    RemoteModel *m_remoteModel;
    ActionModel *m_actionModel;
        
public:
    virtual void load();
    virtual void save();
    explicit KCMRemoteControl(QWidget *parent = 0, const QVariantList &args = QVariantList());
    ~KCMRemoteControl();

private Q_SLOTS:
    void addAction();
    void removeAction();
    void editAction();
    void copyAction();
    void moveActionUp();
    void moveActionDown();
    
    void addMode();
    void editMode();
    void removeMode();
    void moveModeUp();
    void moveModeDown();
    
    void updateModes();
    void updateActions(Mode *mode);
    void modeSelectionChanged(const QItemSelection &selection);
    void actionSelectionChanged(const QItemSelection &selection);
    void addUnconfiguredRemotes();

    void autoPopulate();
    void actionDropped(Mode *mode);

};

#endif
