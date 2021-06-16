QT       += core gui printsupport 3dcore 3drender 3dinput 3dextras
CONFIG -= app_bundle

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PuMA_V3_GUI
TEMPLATE = app

INCLUDEPATH += ${CONDA_PREFIX}/include
INCLUDEPATH += ${CONDA_PREFIX}/include/eigen3/Eigen 
INCLUDEPATH += ${PuMA_DIR}/install/include
LIBS += -L${CONDA_PREFIX}/lib -ltiff -lfftw3_threads -lfftw3 -lomp
LIBS += -L${PuMA_DIR}/install/lib -lPuMA

QMAKE_LFLAGS += "-Wl,-rpath,\'\${PuMA_DIR}/install/lib\'"


QMAKE_CXXFLAGS += --std=c++0x

SOURCES += \
        $$(PuMA_DIR)/gui/src/*.cpp \
        $$(PuMA_DIR)/gui/src/Export/*.cpp \
        $$(PuMA_DIR)/gui/src/Generate/*.cpp \
        $$(PuMA_DIR)/gui/src/Import/*.cpp \
        $$(PuMA_DIR)/gui/src/Log/*.cpp \
        $$(PuMA_DIR)/gui/src/MaterialProperties/*.cpp \
        $$(PuMA_DIR)/gui/src/Process/*.cpp \
        $$(PuMA_DIR)/gui/src/Utilities/*.cpp \
        $$(PuMA_DIR)/gui/src/Utilities/qcustomplot/*.cpp \
        $$(PuMA_DIR)/gui/src/Plugins/*.cpp \
        $$(PuMA_DIR)/gui/src/Visualization/*.cpp \


HEADERS += \
        $$(PuMA_DIR)/gui/src/*.h  \
        $$(PuMA_DIR)/gui/src/Export/*.h \
        $$(PuMA_DIR)/gui/src/Generate/*.h \
        $$(PuMA_DIR)/gui/src/Import/*.h \
        $$(PuMA_DIR)/gui/src/Log/*.h \
        $$(PuMA_DIR)/gui/src/MaterialProperties/*.h \
        $$(PuMA_DIR)/gui/src/Process/*.h \
        $$(PuMA_DIR)/gui/src/Utilities/*.h \
        $$(PuMA_DIR)/gui/src/Utilities/qcustomplot/*.h \
        $$(PuMA_DIR)/gui/src/Plugins/*.h \
        $$(PuMA_DIR)/gui/src/Visualization/*.h \


FORMS += \
        $$(PuMA_DIR)/gui/src/*.ui \
        $$(PuMA_DIR)/gui/src/Export/*.ui \
        $$(PuMA_DIR)/gui/src/Generate/*.ui \
        $$(PuMA_DIR)/gui/src/Import/*.ui \
        $$(PuMA_DIR)/gui/src/Log/*.ui \
        $$(PuMA_DIR)/gui/src/MaterialProperties/*.ui \
        $$(PuMA_DIR)/gui/src/Process/*.ui \
        $$(PuMA_DIR)/gui/src/Plugins/*.ui \
        $$(PuMA_DIR)/gui/src/Visualization/*.ui \

RESOURCES += \
    $$(PuMA_DIR)/gui/extras/resource.qrc
