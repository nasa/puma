/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2013 Louise Brown

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

#include "PrecompiledHeaders.h"
#include "RangeValidator.h"

IMPLEMENT_DYNAMIC_CLASS(RangeValidator, wxValidator)

BEGIN_EVENT_TABLE(RangeValidator, wxValidator)
  EVT_CHAR(RangeValidator::OnChar)
END_EVENT_TABLE()

RangeValidator::RangeValidator(wxString* value, double min, double max)
:m_min(min), m_max(max), m_value(value)
{
}

RangeValidator::RangeValidator(const RangeValidator& val)
  /*: wxValidator()*/
{
	  Copy(val);
	  m_min = val.m_min;
	  m_max = val.m_max;
	  m_value = val.m_value;
}

void RangeValidator::OnChar(wxKeyEvent& event)
{
	  bool bCheck = this->CheckValidator();
	  bool bSkip = true;

	  int iKeyCode = event.GetKeyCode();

	  if((iKeyCode < 32)              ||      // Space
		 (iKeyCode == WXK_DELETE)     ||
		 (iKeyCode >= 300))                   // Home, Shift, F..., etc.
	  {
			bCheck = false;
	  }

	  if(bCheck)
	  {
		// stringvalues to store the value which is actually in the ctrl and the
		// value which will be in the ctrl, if we let the keyevent go to the native
		// ctrl.
		wxString oldvalue;
		wxString newvalue;

		// check if it is worth to go on
		if((!(wxIsdigit(iKeyCode)))   &&
		   (iKeyCode != '.')          &&
		   (iKeyCode != ',')          &&
		   (iKeyCode != '-'))
		{
		  //bCheck = false;
		  bSkip = false;
		}
		else
		{
		  wxTextCtrl* txt = (wxTextCtrl*) this->GetWindow();
		  oldvalue << txt->GetValue();

		  // take care of the insertion point in your textctrl
		  long lInsertionPoint = txt->GetInsertionPoint();
		  long lOldLength = oldvalue.Length();
		  newvalue << oldvalue.Mid(0, (size_t) lInsertionPoint);
		  newvalue << (wxChar) iKeyCode;
		  if(lInsertionPoint < lOldLength)
		  {
			newvalue << oldvalue.Mid((size_t) lInsertionPoint);
		  }

		  bSkip = ((this->CheckValue(newvalue, false)) == 0);
		}
	  }

	  if(bSkip)
	  {
		event.Skip();
	  }
}

int RangeValidator::CheckValue(const wxString& value, bool validate)
{
	  int TO_BIG    =  1;
	  int TO_SMALL  = -1;
	  int OK        =  0;


	  // if the value is just "-", we must let it through. Otherwise we got
	  // no chance to type negative values
	  if(!validate && value.Cmp(ConvertString(stringify("-"))) == 0)
	  {
			if(m_min >= 0.0)
			{
				return TO_SMALL;
			}
			else
			{
				return OK;
			}
	  }

	  double number = 0.0;
	  if(!value.ToDouble(&number))
	  {
		// this shouldn't happen
			return TO_SMALL;
	  }

	  if(number > m_max)
	  {
			return TO_BIG;
	  }

	  // if validate is calling, we must check the whole value...
	  // it is correct or not, so we can stop after that
	  if(validate)
	  {
			if(number < m_min)
			{
				return TO_SMALL;
			}

			return OK;
	  }

	  // if the number is "not in range", we must check, if the user "could"
	  // type the correct number
	  if(number < m_min)
	  {
			wxString minvalue;
			minvalue << m_min;
			minvalue.Truncate(value.Length());

			double tmp_min = 0.0;
			if(minvalue.ToDouble(&tmp_min))
			{
				  if(number < tmp_min)
				  {
						return TO_SMALL;
				  }

				  return OK;
			}
			else
			{
				  // this won't happen... I hope ;-)
				  return TO_SMALL;
			}
	  }

	  return OK;
}


bool RangeValidator::CheckValidator(void)
{
	  if(!m_value)
			return false;

	  if(m_max < m_min)
			return false;

	  if(!wxDynamicCast(this->GetWindow(), wxTextCtrl))
			return false;

	  return true;
}

bool RangeValidator::Validate(wxWindow *parent)
{
	  if(!this->CheckValidator())
			return false;

	  wxString value = ((wxTextCtrl*) this->GetWindow())->GetValue();

	  int i = this->CheckValue(value, true);

	  /*if(i < 0)
	  {
		::wxMessageBox(wxT("value <"+value+"> is too small"));
	  }

	  if (i > 0)
	  {
		::wxMessageBox(wxT("value <"+value+"> is too big"));
	  }*/

	  return (i == 0);
}

bool RangeValidator::TransferToWindow()
{
	  if(!this->CheckValidator())
			return false;

	  ((wxTextCtrl*) this->GetWindow())->SetValue(*m_value);

	  return true;
}

bool RangeValidator::TransferFromWindow()
{
	if(!this->CheckValidator())
		return false;

	*m_value = ((wxTextCtrl*) this->GetWindow())->GetValue();

	return true;
}
