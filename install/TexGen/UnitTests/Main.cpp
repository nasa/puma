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

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "../Core/PrecompiledHeaders.h"

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

//#include "SolverTests.h"
//#include "MesherTests.h"
//#include "GeometricTests.h"
//#include "PropertyTests.h"

int main(int argc, char **argv)
{
#ifdef _MSC_VER
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

//	_crtBreakAlloc = 2649;

	CppUnit::TextUi::TestRunner runner;
	CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
	runner.addTest(registry.makeTest());
//	runner.addTest(CSolverTests::suite());
//	runner.addTest(CMesherTests::suite());
//	runner.addTest(CGeometricTests::suite());
//	runner.addTest(CPropertyTests::suite());
	TexGen::CTimer Timer;
	Timer.start();
	bool bSuccess = runner.run();
	Timer.check();
	Timer.stop();

	return bSuccess?0:1;
}








