# Install script for directory: /Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/lib/libPuMA.dylib")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/lib" TYPE SHARED_LIBRARY FILES "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/cmake-build-debug/libPuMA.dylib")
  if(EXISTS "$ENV{DESTDIR}/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/lib/libPuMA.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/lib/libPuMA.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -x "$ENV{DESTDIR}/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/lib/libPuMA.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/puma.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/logger.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/timer.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/iterativesolvers.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/AMatrix.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/ej_AMatrix.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/ej_diffusion.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/fv_diffusion.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/fv_AMatrix.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/fv_constantvalueboundary.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/fv_symmetricboundary.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/fv_periodicboundary.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/fv_boundarycondition.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/fv_anisotropic_AMatrix.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/fv_anisotropic_diffusion.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/Printer.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/matrix.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/workspace.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/triangle.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/pstring.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/vector.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/cutoff.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/MarchingCubes.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/isosurfacehelper.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/isosurface.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/LookUpTable.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/prng_engine.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/meanfilter3d.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/medianfilter3d.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/bilateralfilter.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/filter.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/export.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/export_STL.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/fio.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/err.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/swapit.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/export_STL_helper.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/ostr.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/export_textfile.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/export_textfile_workspace.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/export_vtk.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/export_3DTiff_Workspace.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/export_3DTiff.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/export_bin.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/raycasting.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/artificialflux.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/artificialflux_AMatrix.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/artificialflux_diffusion.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/orientation.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/structuretensor.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/mp_volumefraction.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/mp_volumefractionhelper.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/meaninterceptlength.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/surfacearea.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/materialproperty.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/tortuosity_unified.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/fv_tortuosity.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/particles_cuberilletortuosity.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/particles_isosurfacetortuosity.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/ej_tortuosity.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/ej_thermalconductivity.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/fv_anisotropic_thermalconductivity.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/fv_thermalconductivity.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/ej_electricalconductivity.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/fv_electricalconductivity.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/operation.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/import_3DTiff.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/import_3DTiff_Workspace.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/import_bin.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/importdata.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/porespace.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/prescribedfibers.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/randomfibers.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/straightflowerfiber.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/straightcirclefiber.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/randomfibersinput.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/curvedcirclefiber.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/curvedflowerfiber.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/fiber.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/generate.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/randomspheresinput.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/sphere.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/randomspheres.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/tpmsinput.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/tpms.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include" TYPE FILE FILES
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/puma.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/logger/logger.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/timer/timer.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/solvers/iterativesolvers.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/solvers/AMatrix.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/solvers/ej_diffusion/ej_AMatrix.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/solvers/ej_diffusion/ej_diffusion.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/solvers/fv_diffusion/fv_diffusion.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/solvers/fv_diffusion/fv_AMatrix.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/solvers/fv_diffusion/fv_constantvalueboundary.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/solvers/fv_diffusion/fv_symmetricboundary.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/solvers/fv_diffusion/fv_periodicboundary.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/solvers/fv_diffusion/fv_boundarycondition.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/solvers/fv_anisotropic_diffusion/fv_anisotropic_AMatrix.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/solvers/fv_anisotropic_diffusion/fv_anisotropic_diffusion.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/primitives/Printer.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/primitives/matrix.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/primitives/workspace.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/primitives/triangle.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/primitives/pstring.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/primitives/vector.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/primitives/cutoff.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/isosurface/MarchingCubes.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/isosurface/isosurfacehelper.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/isosurface/isosurface.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/isosurface/LookUpTable.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/utilities/random/prng_engine.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/filter/mean/meanfilter3d.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/filter/median/medianfilter3d.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/filter/bilateral/bilateralfilter.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/filter/filter.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/export/export.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/export/STL/export_STL.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/export/STL/fio.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/export/STL/err.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/export/STL/swapit.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/export/STL/export_STL_helper.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/export/STL/ostr.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/export/textfile/export_textfile.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/export/textfile/export_textfile_workspace.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/export/vtk/export_vtk.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/export/3D_Tiff/export_3DTiff_Workspace.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/export/3D_Tiff/export_3DTiff.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/export/binary/export_bin.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/orientation/raycasting/raycasting.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/orientation/artificialflux/artificialflux.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/orientation/artificialflux/artificialflux_AMatrix.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/orientation/artificialflux/artificialflux_diffusion.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/orientation/orientation.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/orientation/structuretensor/structuretensor.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/volumefraction/mp_volumefraction.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/volumefraction/mp_volumefractionhelper.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/meaninterceptlength/meaninterceptlength.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/surfacearea/surfacearea.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/materialproperty.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/tortuosity/tortuosity_unified.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/tortuosity/fv_tortuosity.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/tortuosity/particles_cuberilletortuosity.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/tortuosity/particles_isosurfacetortuosity.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/tortuosity/ej_tortuosity.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/thermalconductivity/ej_thermalconductivity.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/thermalconductivity/fv_anisotropic_thermalconductivity.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/thermalconductivity/fv_thermalconductivity.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/electricalconductivity/ej_electricalconductivity.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/materialproperties/electricalconductivity/fv_electricalconductivity.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/operation.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/import/3D_Tiff/import_3DTiff.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/import/3D_Tiff/import_3DTiff_Workspace.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/import/binary/import_bin.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/import/importdata.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/generation/porespace/porespace.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/generation/randomfibers/prescribedfibers.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/generation/randomfibers/randomfibers.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/generation/randomfibers/straightflowerfiber.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/generation/randomfibers/straightcirclefiber.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/generation/randomfibers/randomfibersinput.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/generation/randomfibers/curvedcirclefiber.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/generation/randomfibers/curvedflowerfiber.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/generation/randomfibers/fiber.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/generation/generate.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/generation/randomspheres/randomspheresinput.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/generation/randomspheres/sphere.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/generation/randomspheres/randomspheres.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/generation/tpms/tpmsinput.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/src/generation/tpms/tpms.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin/pumaX_main")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin" TYPE EXECUTABLE FILES "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/cmake-build-debug/pumaX_main")
  if(EXISTS "$ENV{DESTDIR}/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin/pumaX_main" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin/pumaX_main")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/cmake-build-debug"
      "$ENV{DESTDIR}/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin/pumaX_main")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin/pumaX_main")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin/pumaX_examples")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin" TYPE EXECUTABLE FILES "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/cmake-build-debug/pumaX_examples")
  if(EXISTS "$ENV{DESTDIR}/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin/pumaX_examples" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin/pumaX_examples")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/cmake-build-debug"
      "$ENV{DESTDIR}/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin/pumaX_examples")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin/pumaX_examples")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin/pumaX_testing")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin" TYPE EXECUTABLE FILES "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/cmake-build-debug/pumaX_testing")
  if(EXISTS "$ENV{DESTDIR}/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin/pumaX_testing" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin/pumaX_testing")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/cmake-build-debug"
      "$ENV{DESTDIR}/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin/pumaX_testing")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip" -u -r "$ENV{DESTDIR}/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/bin/pumaX_testing")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/mastertest.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/subtest.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/toString.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/color.h;/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include/testresult.h")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/../install/include" TYPE FILE FILES
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/test/mastertest.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/test/testframework/subtest.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/test/testframework/toString.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/test/testframework/color.h"
    "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/test/testframework/testresult.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/cmake-build-debug/src/cmake_install.cmake")
  include("/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/cmake-build-debug/test/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/Users/josephferguson/Desktop/Joseph/Stanford/Codes/puma-dev/cpp/cmake-build-debug/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
