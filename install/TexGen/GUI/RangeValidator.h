/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2011 Louise Brown

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
=============================================================================*/

#pragma once

/// Class to validate a wxTextCtrl input.  Checks input is within upper and lower bounds
/// Taken from wxWidgets user forum: http://forums.wxwidgets.org/viewtopic.php?t=12582

class RangeValidator : public wxValidator
{
	DECLARE_DYNAMIC_CLASS(RangeValidator)

	DECLARE_EVENT_TABLE()

	 public:

		/// Constructor
		RangeValidator(wxString* value, double min, double max);
		RangeValidator(){};
	      

		/// Copy-Constructor
		RangeValidator(const RangeValidator& val);

		/// Returns a copy of this validator
		virtual RangeValidator* Clone(void) const {return (new RangeValidator(*this));};

		/// validate. true when is correct
		virtual bool Validate(wxWindow *parent);

		/// transfer the data to the window
		virtual bool TransferToWindow(void);

		/// transfer the data from the window
		virtual bool TransferFromWindow(void);

	protected:

		/// char-event callback
		void OnChar(wxKeyEvent& event);

		/// check for valid pointers
		virtual bool CheckValidator(void);

		/// check, if the given value is a number in the defined range. The second
		/// parameter specifies, from where the method is called. It will return "0"
		/// if the given value is in the range, a number greater than zero if it is
		/// too big and a number less than zero if it is to small.
		virtual int CheckValue(const wxString& value, bool validate);

	  protected:

		double m_min;
		double m_max;
		wxString* m_value;
};