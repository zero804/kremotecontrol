/*
    Copyright (C) 2010  Michael Zanetti <michael_zanetti@gmx.net>

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

#include "modelist.h"

ModeList::ModeList() {

}

void ModeList::loadFromConfig(const KConfig& config) {
  // load configured modes here with append(mode)
  
}

void ModeList::saveToConfig(const KConfig& config) {
  for(ModeList::iterator i = begin(); i != end(); ++i){
    // save (*i) into config
  }
}

ModeList ModeList::findModes(const QString& remote) {
  ModeList retList;
  for(ModeList::iterator i = begin(); i != end(); ++i){
    if((*i).remote() == remote){
      retList.append(*i);
    }
  }
}

Mode ModeList::defaultMode(const QString& remote) {
  if(m_defaultModes.contains(remote)){
    return m_defaultModes.value(remote);
  }
  return Mode("", remote);
}
