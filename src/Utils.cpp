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

#include <QApplication>
#include <QMouseEvent>

#include <octave/oct.h>
#include <octave/graphics.h>

#include "Backend.h"
#include "Container.h"
#include "Object.h"
#include "Utils.h"

//////////////////////////////////////////////////////////////////////////////

namespace QtHandles
{

//////////////////////////////////////////////////////////////////////////////

namespace Utils
{

//////////////////////////////////////////////////////////////////////////////

QString fromStdString (const std::string& s)
{
  return QString::fromLocal8Bit (s.c_str ());
}

//////////////////////////////////////////////////////////////////////////////

std::string toStdString (const QString& s)
{
  return std::string (s.toLocal8Bit ().data ());
}

//////////////////////////////////////////////////////////////////////////////

QStringList fromStringVector (const string_vector& v)
{
  QStringList l;
  octave_idx_type n = v.length ();

  for (octave_idx_type i = 0; i < n; i++)
    l << fromStdString (v[i]);

  return l;
}

//////////////////////////////////////////////////////////////////////////////

string_vector toStringVector (const QStringList& l)
{
  string_vector v (l.length ());
  int i = 0;

  foreach (const QString& s, l)
    v[i++] = toStdString (s);

  return v;
}

//////////////////////////////////////////////////////////////////////////////

template <class T>
QFont computeFont (const typename T::properties& props, int height)
{
  QFont f (fromStdString (props.get_fontname ()));

  static std::map<std::string, QFont::Weight> weightMap;
  static std::map<std::string, QFont::Style> angleMap;
  static bool mapsInitialized = false;

  if (! mapsInitialized)
    {
      weightMap[std::string ("normal")] = QFont::Normal;
      weightMap[std::string ("light")] = QFont::Light;
      weightMap[std::string ("demi")] = QFont::DemiBold;
      weightMap[std::string ("bold")] = QFont::Normal;

      angleMap[std::string ("normal")] = QFont::StyleNormal;
      angleMap[std::string ("italic")] = QFont::StyleItalic;
      angleMap[std::string ("oblique")] = QFont::StyleOblique;

      mapsInitialized = true;
    }

  f.setPointSizeF (props.get_fontsize_points (height));
  f.setWeight (weightMap[props.get_fontweight ()]);
  f.setStyle (angleMap[props.get_fontangle ()]);

  return f;
}

template QFont computeFont<uicontrol> (const uicontrol::properties& props,
				       int height);
template QFont computeFont<uipanel> (const uipanel::properties& props,
				     int height);

//////////////////////////////////////////////////////////////////////////////

QColor fromRgb (const Matrix& rgb)
{
  QColor c;

  if (rgb.numel () == 3)
    c.setRgbF (rgb(0), rgb(1), rgb(2));
  
  return c;
}

//////////////////////////////////////////////////////////////////////////////

Matrix toRgb (const QColor& c)
{
  Matrix rgb (1, 3);
  double* rgbData = rgb.fortran_vec ();

  c.getRgbF (rgbData, rgbData+1, rgbData+2);

  return rgb;
}

//////////////////////////////////////////////////////////////////////////////

std::string figureSelectionType (QMouseEvent* event, bool isDoubleClick)
{
  if (isDoubleClick)
    return std::string ("open");
  else
    {
      Qt::MouseButtons buttons = event->buttons ();
      Qt::KeyboardModifiers mods = event->modifiers ();

      if (mods == Qt::NoModifier)
	{
	  if (buttons == Qt::LeftButton)
	    return std::string ("normal");
	  else if (buttons == Qt::RightButton)
	    return std::string ("alt");
#if defined (Q_WS_WIN)
	  else if (buttons == (Qt::LeftButton|Qt::RightButton))
	    return std::string ("extend");
#elif defined (Q_WS_X11)
	  else if (buttons == Qt::MidButton)
	    return std::string ("extend");
#endif
	}
      else if (buttons == Qt::LeftButton)
	{
	  if (mods == Qt::ShiftModifier)
	    return std::string ("extend");
	  else if (mods == Qt::ControlModifier)
	    return std::string ("alt");
	}
    }

  return std::string ("normal");
}

//////////////////////////////////////////////////////////////////////////////

Matrix figureCurrentPoint (const graphics_object& fig, QMouseEvent* event)
{
  Object* tkFig = Backend::toolkitObject (fig);

  if (tkFig)
    {
      Container* c = tkFig->innerContainer ();

      if (c)
	{
	  QPoint qp = c->mapFromGlobal (event->globalPos ());

	  return
	    tkFig->properties<figure> ().map_from_boundingbox (qp.x (),
							       qp.y ());
	}
    }

  return Matrix (1, 2, 0.0);
}

//////////////////////////////////////////////////////////////////////////////

Qt::Alignment fromHVAlign (const caseless_str& halign,
			   const caseless_str& valign)
{
  Qt::Alignment flags;

  if (halign.compare ("left"))
    flags |= Qt::AlignLeft;
  else if (halign.compare ("center"))
    flags |= Qt::AlignHCenter;
  else if (halign.compare ("right"))
    flags |= Qt::AlignRight;
  else
    flags |= Qt::AlignLeft;

  if (valign.compare ("middle"))
    flags |= Qt::AlignVCenter;
  else if (valign.compare ("top"))
    flags |= Qt::AlignTop;
  else if (valign.compare ("bottom"))
    flags |= Qt::AlignBottom;
  else
    flags |= Qt::AlignVCenter;

  return flags;
}

//////////////////////////////////////////////////////////////////////////////

}; // namespace Utils

//////////////////////////////////////////////////////////////////////////////

}; // namespace QtHandles