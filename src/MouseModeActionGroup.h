/*

Copyright (C) 2011 Michael Goffioul.

This file is part of QtHandles.

Foobar is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

QtHandles is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __QtHandles_MouseModeActionGroup__
#define __QtHandles_MouseModeActionGroup__ 1

#include <QList>
#include <QObject>

#include "Figure.h"

class QAction;

//////////////////////////////////////////////////////////////////////////////

namespace QtHandles
{

//////////////////////////////////////////////////////////////////////////////

class MouseModeActionGroup : public QObject
{
  Q_OBJECT

public:
  MouseModeActionGroup (QObject* parent = 0);
  ~MouseModeActionGroup (void);

  QList<QAction*> actions (void) const { return m_actions; }
  MouseMode mouseMode (void) const;

signals:
  void modeChanged (MouseMode mode);

private slots:
  void actionToggled (bool checked);

private:
  QList<QAction*> m_actions;
  QAction* m_current;
};

//////////////////////////////////////////////////////////////////////////////

};

//////////////////////////////////////////////////////////////////////////////

#endif
