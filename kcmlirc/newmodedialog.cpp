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

#include <qstringlist.h>
#include "newmodedialog.h"
#include "mode.h"

NewModeDialog::NewModeDialog(QStringList remoteList, QWidget *parent, const bool &modal): KDialog(parent)
{
     newModeBaseWidget = new NewModeBaseWidget();
     setMainWidget(newModeBaseWidget);
     setButtons( Ok | Cancel);
     setDefaultButton(Ok);
     setModal(modal);

     remoteList.sort();
     for (QStringList::iterator it = remoteList.begin(); it != remoteList.end(); ++it) {
       //QTreeWidgetItem *a =
         new QTreeWidgetItem(newModeBaseWidget->theRemotes, (QStringList() << *it));
    }
     newModeBaseWidget->theIcon->setIcon("irkick");
     //TODO: shall a remote be selected. I think its not neccessary
     //In the old implementation the remote from kcmlirc is selected

     //newModeBaseWidget->theRemotes->setCurrentItem()
     //newModeBaseWidget->theRemotes->currentItem()->setSelected(true);
    // signals and slots connections
    connect(newModeBaseWidget->theName, SIGNAL(textChanged(const QString&)),
            this, SLOT(slotTextChanged(const QString&)));
    connect(newModeBaseWidget->theRemotes, SIGNAL(itemActivated(QTreeWidgetItem*,int)), this, SLOT(slotRemoteChanged(QTreeWidgetItem*)));
    connect(newModeBaseWidget->checkBox, SIGNAL(toggled(bool)),  newModeBaseWidget->theIcon, SLOT(setEnabled(bool)));
    enableButtonOk(false);
}

NewModeDialog::~NewModeDialog()
{
}

void NewModeDialog::slotRemoteChanged(QTreeWidgetItem* qTreeWidgetItem)
{
  enableButtonOk(qTreeWidgetItem != 0);
}

void NewModeDialog::slotTextChanged(const QString& newText)
{
    enableButtonOk(!newText.isEmpty() && newModeBaseWidget->theRemotes->currentItem() != 0);
}
void NewModeDialog::setSelectedRemote(QString remote){
  //newModeBaseWidget->theRemotes->sel
  //TODO: remote selection neccessary?
}

Mode NewModeDialog::getMode()
{

  Mode mode = Mode();
  mode.setRemote(newModeBaseWidget->theRemotes->currentItem()->text(0));
  mode.setName(newModeBaseWidget->theName->text());
  mode.setIconFile(newModeBaseWidget->checkBox->isChecked()  ?
       newModeBaseWidget->theIcon->icon() : QString::null);
  return mode;
}

bool NewModeDialog::isDefaultMode()
{
  return newModeBaseWidget->theDefault->isChecked();
}

#include "newmodedialog.moc"
