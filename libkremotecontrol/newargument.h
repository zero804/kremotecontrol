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

#ifndef NEWARGUMENT_H
#define NEWARGUMENT_H

#include <kdemacros.h>

#include <QVariant>

class KDE_EXPORT NewArgument
{
  public:
    NewArgument();
    NewArgument(const QVariant &value);
    NewArgument(const QVariant &defaultValue, const QString &description);
    
    QVariant value() const;
    QVariant defaultValue() const;
    QString description() const;
    
    bool operator==(const NewArgument &other) const;
    
  protected:
    QVariant m_value;
    QVariant m_defaultValue;
    QString m_description;
};

#endif // NEWARGUMENT_H