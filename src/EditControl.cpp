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

#include <QLineEdit>

#include "Container.h"
#include "EditControl.h"
#include "TextEdit.h"
#include "Utils.h"

//////////////////////////////////////////////////////////////////////////////

namespace QtHandles
{

//////////////////////////////////////////////////////////////////////////////

EditControl* EditControl::create (const graphics_object& go)
{
  Object* parent = Object::parentObject (go);

  if (parent)
    {
      Container* container = parent->innerContainer ();

      if (container)
	{
	  uicontrol::properties& up = Utils::properties<uicontrol> (go);

	  if ((up.get_max () - up.get_min ()) > 1)
	    return new EditControl (go, new TextEdit (container));
	  else
	    return new EditControl (go, new QLineEdit (container));
	}
    }

  return 0;
}

//////////////////////////////////////////////////////////////////////////////

EditControl::EditControl (const graphics_object& go, QLineEdit* edit)
     : BaseControl (go, edit), m_multiLine (false), m_textChanged (false)
{
  init (edit);
}

//////////////////////////////////////////////////////////////////////////////

void EditControl::init (QLineEdit* edit, bool callBase)
{
  if (callBase)
    BaseControl::init (edit, callBase);

  m_multiLine = false;
  initCommon (edit);

  uicontrol::properties& up = properties<uicontrol> ();

  edit->setText (Utils::fromStdString (up.get_string_string ()));
  edit->setAlignment (Utils::fromHVAlign (up.get_horizontalalignment (),
					  up.get_verticalalignment ()));

  connect (edit, SIGNAL (textEdited (const QString&)),
	   SLOT (textChanged (void)));
  connect (edit, SIGNAL (editingFinished (void)),
	   SLOT (editingFinished (void)));
}

//////////////////////////////////////////////////////////////////////////////

EditControl::EditControl (const graphics_object& go, TextEdit* edit)
     : BaseControl (go, edit), m_multiLine (true), m_textChanged (false)
{
  init (edit);
}

//////////////////////////////////////////////////////////////////////////////

void EditControl::init (TextEdit* edit, bool callBase)
{
  if (callBase)
    BaseControl::init (edit, callBase);

  m_multiLine = true;
  initCommon (edit);

  uicontrol::properties& up = properties<uicontrol> ();

  edit->setAcceptRichText (false);
  // FIXME: support string_vector
  edit->setPlainText (Utils::fromStdString (up.get_string_string ()));
  
  connect (edit, SIGNAL (textChanged (void)),
	   SLOT (textChanged (void)));
  connect (edit, SIGNAL (editingFinished (void)),
	   SLOT (editingFinished (void)));
}

//////////////////////////////////////////////////////////////////////////////

EditControl::~EditControl (void)
{
}

//////////////////////////////////////////////////////////////////////////////

void EditControl::initCommon (QWidget*)
{
  m_textChanged = false;
}

//////////////////////////////////////////////////////////////////////////////

void EditControl::update (int pId)
{
  bool handled = false;

  if (m_multiLine)
    handled = updateMultiLine (pId);
  else
    handled = updateSingleLine (pId);

  if (! handled)
    {
      switch (pId)
	{
	default:
	  BaseControl::update (pId);
	  break;
	}
    }
}

//////////////////////////////////////////////////////////////////////////////

bool EditControl::updateSingleLine (int pId)
{
  uicontrol::properties& up = properties<uicontrol> ();
  QLineEdit* edit = qWidget<QLineEdit> ();

  switch (pId)
    {
    case uicontrol::properties::ID_STRING:
      edit->setText (Utils::fromStdString (up.get_string_string ()));
      return true;
    case uicontrol::properties::ID_HORIZONTALALIGNMENT:
    case uicontrol::properties::ID_VERTICALALIGNMENT:
      edit->setAlignment (Utils::fromHVAlign (up.get_horizontalalignment (),
					      up.get_verticalalignment ()));
      return true;
    case uicontrol::properties::ID_MIN:
    case uicontrol::properties::ID_MAX:
      if ((up.get_max () - up.get_min ()) > 1)
	{
	  QWidget* container = edit->parentWidget ();

	  delete edit;
	  init (new TextEdit (container), true);
	}
      return true;
    default:
      break;
    }

  return false;
}

//////////////////////////////////////////////////////////////////////////////

bool EditControl::updateMultiLine (int pId)
{
  uicontrol::properties& up = properties<uicontrol> ();
  TextEdit* edit = qWidget<TextEdit> ();

  switch (pId)
    {
    case uicontrol::properties::ID_STRING:
      edit->setPlainText (Utils::fromStdString (up.get_string_string ()));
      return true;
    case uicontrol::properties::ID_MIN:
    case uicontrol::properties::ID_MAX:
      if ((up.get_max () - up.get_min ()) <= 1)
	{
	  QWidget* container = edit->parentWidget ();

	  delete edit;
	  init (new QLineEdit (container), true);
	}
      return true;
    default:
      break;
    }

  return false;
}

//////////////////////////////////////////////////////////////////////////////

void EditControl::textChanged (void)
{
  m_textChanged = true;
}

//////////////////////////////////////////////////////////////////////////////

void EditControl::editingFinished (void)
{
  if (m_textChanged)
    {
      QString txt = (m_multiLine
		     ? qWidget<TextEdit> ()->toPlainText ()
		     : qWidget<QLineEdit> ()->text ());

      gh_manager::post_set (m_handle, "string", Utils::toStdString (txt), false);
      gh_manager::post_callback (m_handle, "callback");

      m_textChanged = false;
    }
}

//////////////////////////////////////////////////////////////////////////////

}; // namespace QtHandles

//////////////////////////////////////////////////////////////////////////////
