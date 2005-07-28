//
//
// C++ Interface: $MODULE$
//
// Description:
//
//
// Author: Gav Wood <gav@kde.org>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <q3valuelist.h>
#include <qvariant.h>

/**
@author Gav Wood
*/

class Arguments : public Q3ValueList<QVariant>
{
public:
	const QString toString() const;

	Arguments();
	~Arguments();
};

#endif
