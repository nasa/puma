/*=============================================================================
TexGen: Geometric textile modeller.
Copyright (C) 2006 Martin Sherburn

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
#include "PythonWrapper.h"
#include "LoggerGUI.h"

CPythonWrapper::CPythonWrapper(void)
: m_pConsoleInstance(NULL)
, m_pCompleterInstance(NULL)
, m_iHistoryPosition(-1)
{
	m_pConsoleInstance = CreateInstance("code", "InteractiveConsole");
	if (!m_pConsoleInstance)
		wxMessageBox(wxT("Unable to create python instance of InteractiveConsole in code module!"), wxT("Error"), wxOK | wxICON_ERROR);

	if (m_pConsoleInstance)
	{
		PyObject *pArgs, *pNameSpace;
		pNameSpace = PyObject_GetAttrString(m_pConsoleInstance, "locals");
		if (pNameSpace)
		{
			pArgs = PyTuple_New(1);		// new ref
			PyTuple_SetItem(pArgs, 0, pNameSpace);		// pNameSpace reference stolen here
			m_pCompleterInstance = CreateInstance("TexGen.Completer", "Completer", pArgs);
			if (!m_pCompleterInstance)
				wxMessageBox(wxT("Unable to complete python code with TAB, completer module not found!"), wxT("Error"), wxOK | wxICON_ERROR);
			Py_DECREF(pArgs);
		}
		else
			assert(false);
	}

	PyRun_SimpleString("");		// Seems to force the exceptions to be shown...
}

CPythonWrapper::~CPythonWrapper(void)
{
	DeleteInstance(m_pConsoleInstance);
	DeleteInstance(m_pCompleterInstance);
}

PyObject *CPythonWrapper::CreateInstance(char szModule[], char szClass[], PyObject *pArgs)
{
	PyObject *pName, *pModule, *pClass;
	pName = PyString_FromString(szModule);		// new ref
    pModule = PyImport_Import(pName);		// new ref
    Py_DECREF(pName);
    if (pModule == NULL)
	{
		return NULL;
	}

    pClass = PyObject_GetAttrString(pModule, szClass);		// new ref
    Py_DECREF(pModule);
	if (pClass == NULL)
	{
		return NULL;
	}

	PyObject *pInstance = PyInstance_New(pClass, pArgs, NULL);		// new ref
	Py_DECREF(pClass);

	return pInstance;
}

void CPythonWrapper::DeleteInstance(PyObject *pInstance)
{
	if (pInstance)
	{
		Py_DECREF(pInstance);
		pInstance = NULL;
	}
}

string CPythonWrapper::GetPrevHistoryCommand()
{
	++m_iHistoryPosition;
	if (m_iHistoryPosition >= (int)m_CommandHistory.size())
		m_iHistoryPosition = (int)m_CommandHistory.size()-1;
	if (m_iHistoryPosition >= 0)
		return m_CommandHistory[m_iHistoryPosition];
	return "";
}

string CPythonWrapper::GetNextHistoryCommand()
{
	--m_iHistoryPosition;
	if (m_iHistoryPosition < -1)
		m_iHistoryPosition = -1;
	if (m_iHistoryPosition >= 0)
		return m_CommandHistory[m_iHistoryPosition];
	return "";
}

bool CPythonWrapper::SendCommand(string Command)
{
	if (!m_pConsoleInstance)
		return false;

	m_iHistoryPosition = -1;
	m_CommandHistory.push_front(Command);

	bool bReturn = false;

	PyRun_SimpleString("sys.stdout = OutputCatcher('interactive_stdout')");
	PyRun_SimpleString("sys.stderr = OutputCatcher('interactive_stderr')");
	if ( CTexGen::GetInstance().GetMessagesOn() )
		CTexGen::GetInstance().SetLogger(CLoggerGUI(true));

	PyObject *pArgs, *pValue, *pFunc;
	pArgs = PyTuple_New(1);		// new ref
	pValue = PyString_FromString(Command.c_str());		// new ref
	PyTuple_SetItem(pArgs, 0, pValue);		// pValue reference stolen here
	pFunc = PyObject_GetAttrString(m_pConsoleInstance, "push");		// new ref
	pValue = PyObject_CallObject(pFunc, pArgs);		// new ref
	Py_DECREF(pArgs);
	Py_DECREF(pFunc);
	if (pValue != NULL)
	{
		bReturn = PyInt_AsLong(pValue)?true:false;
		Py_DECREF(pValue);
	}

	PyRun_SimpleString("sys.stdout = OutputCatcher('script_stdout')");
	PyRun_SimpleString("sys.stderr = OutputCatcher('script_stderr')");
	if ( CTexGen::GetInstance().GetMessagesOn() )
		CTexGen::GetInstance().SetLogger(CLoggerGUI(false));

	return bReturn;
}

bool CPythonWrapper::SendCodeBlock(string Code)
{
	if (!m_pConsoleInstance)
		return false;

	PyObject *pCode = Py_CompileString(Code.c_str(), "gui", Py_file_input);		// new ref

	if (!pCode)
		return false;

	PyObject *pArgs, *pValue, *pFunc;
	pArgs = PyTuple_New(1);		// new ref
	PyTuple_SetItem(pArgs, 0, pCode);		// pCode reference stolen here
	pFunc = PyObject_GetAttrString(m_pConsoleInstance, "runcode");		// new ref
	pValue = PyObject_CallObject(pFunc, pArgs);		// new ref
	Py_DECREF(pArgs);
	Py_DECREF(pFunc);
	Py_DECREF(pValue);

	return true;
}
/*
void CPythonWrapper::RunScript(string Filename)
{
	if (!m_pConsoleInstance)
		return;

	ifstream Input(Filename.c_str());
	stringstream StringStream;
	StringStream << Input.rdbuf();
	PyObject *pCode = Py_CompileString(StringStream.str().c_str(), Filename.c_str(), Py_file_input);		// new ref

	if (!pCode)
		return;

	PyObject *pArgs, *pValue, *pFunc;
	pArgs = PyTuple_New(1);		// new ref
	PyTuple_SetItem(pArgs, 0, pCode);		// pCode reference stolen here
	pFunc = PyObject_GetAttrString(m_pConsoleInstance, "runcode");		// new ref
	pValue = PyObject_CallObject(pFunc, pArgs);		// new ref
	Py_DECREF(pArgs);
	Py_DECREF(pFunc);
	Py_DECREF(pValue);
}
*/
string CPythonWrapper::Complete(string Text, long iState)
{
	if (!m_pCompleterInstance)
		return "";

	PyObject *pArgs, *pValue, *pFunc;
	pArgs = PyTuple_New(2);		// new ref
	pValue = PyString_FromString(Text.c_str());		// new ref
	PyTuple_SetItem(pArgs, 0, pValue);		// pValue reference stolen here
	pValue = PyInt_FromLong(iState);		// new ref
	PyTuple_SetItem(pArgs, 1, pValue);		// pValue reference stolen here
	pFunc = PyObject_GetAttrString(m_pCompleterInstance, "complete");		// new ref
	pValue = PyObject_CallObject(pFunc, pArgs);		// new ref
	Py_DECREF(pArgs);
	Py_DECREF(pFunc);
	if (pValue != NULL)
	{
		if (PyString_Check(pValue))
		{
			const char * pString = PyString_AsString(pValue);
			Py_DECREF(pValue);
			if (strlen(pString) <= Text.size())
				return "";
			return string(pString+Text.size());
		}
		else
			Py_DECREF(pValue);
	}
	return "";
}

vector<string> CPythonWrapper::GetCompleteOptions(string Text)
{
	if (!m_pCompleterInstance)
		return vector<string>();

	PyObject *pArgs, *pValue, *pFunc, *pList;
	pArgs = PyTuple_New(1);		// new ref
	pValue = PyString_FromString(Text.c_str());		// new ref
	PyTuple_SetItem(pArgs, 0, pValue);		// pValue reference stolen here
	pFunc = PyObject_GetAttrString(m_pCompleterInstance, "getcompleteoptions");		// new ref
	pList = PyObject_CallObject(pFunc, pArgs);		// new ref
	Py_DECREF(pArgs);
	Py_DECREF(pFunc);
	if (pList != NULL)
	{
		if (PyList_Check(pList))
		{
			vector<string> Completions;
			int iSize = PyList_Size(pList);
			int i;
			for (i=0; i<iSize; ++i)
			{
				PyObject *pStringObject = PyList_GetItem(pList, i);	// Borrowed ref
				if (PyString_Check(pStringObject))
				{
					const char * pString = PyString_AsString(pStringObject);
					Completions.push_back(pString);
				}
			}
			Py_DECREF(pList);
			sort(Completions.begin(), Completions.end());		// sort
			vector<string>::iterator itNewEnd;
			itNewEnd = unique(Completions.begin(), Completions.end());		// remove duplicates
			Completions.erase(itNewEnd, Completions.end());
			return Completions;
		}
		else
			Py_DECREF(pList);
	}
	return vector<string>();
}


