QT       += core gui printsupport serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    analyser/analyser.cpp \
    analyser/client.cpp \
    comparedialog.cpp \
    ledindicator/ledindicator.cpp \
    main.cpp \
    preferencesdialog.cpp \
    projectdialog.cpp \
    valvemodel/circuit/pentodecommoncathode.cpp \
    valvemodel/data/dataset.cpp \
    valvemodel/data/measurement.cpp \
    valvemodel/data/project.cpp \
    valvemodel/data/sample.cpp \
    valvemodel/data/sweep.cpp \
    valvemodel/model/cohenhelietriode.cpp \
    valvemodel/model/device.cpp \
    valvemodel/model/estimate.cpp \
    valvemodel/model/gardinerpentode.cpp \
    valvemodel/model/linearsolver.cpp \
    valvemodel/model/model.cpp \
    valvemodel/model/modelfactory.cpp \
    valvemodel/model/korentriode.cpp \
    valvemodel/model/quadraticsolver.cpp \
    valvemodel/model/reefmanpentode.cpp \
    valvemodel/model/simpletriode.cpp \
    valvemodel/circuit/circuit.cpp \
    valvemodel/circuit/triodeaccathodefollower.cpp \
    valvemodel/circuit/triodecommoncathode.cpp \
    valvemodel/circuit/triodedccathodefollower.cpp \
    valvemodel/model/template.cpp \
    valvemodel/ui/uibridge.cpp \
    valvemodel/ui/plot.cpp \
    valvemodel/ui/parameter.cpp \
    valveworkbench.cpp

HEADERS += \
    analyser/analyser.h \
    analyser/client.h \
    comparedialog.h \
    ledindicator/ledindicator.h \
    ngspice/sharedspice.h \
    preferencesdialog.h \
    projectdialog.h \
    valvemodel/circuit/pentodecommoncathode.h \
    valvemodel/constants.h \
    valvemodel/data/dataset.h \
    valvemodel/data/measurement.h \
    valvemodel/data/project.h \
    valvemodel/data/sample.h \
    valvemodel/data/sweep.h \
    valvemodel/model/cohenhelietriode.h \
    valvemodel/model/device.h \
    valvemodel/model/estimate.h \
    valvemodel/model/gardinerpentode.h \
    valvemodel/model/korentriode.h \
    valvemodel/model/linearsolver.h \
    valvemodel/model/model.h \
    valvemodel/model/modelfactory.h \
    valvemodel/model/quadraticsolver.h \
    valvemodel/model/reefmanpentode.h \
    valvemodel/model/simpletriode.h \
    valvemodel/circuit/circuit.h \
    valvemodel/circuit/triodeaccathodefollower.h \
    valvemodel/circuit/triodecommoncathode.h \
    valvemodel/circuit/triodedccathodefollower.h \
    valvemodel/model/template.h \
    valvemodel/ui/uibridge.h \
    valvemodel/ui/plot.h \
    valvemodel/ui/parameter.h \
    valvemodeller.h \
    valveworkbench.h

FORMS += \
    comparedialog.ui \
    preferencesdialog.ui \
    projectdialog.ui \
    valveworkbench.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$(CMAKE_PREFIX_PATH)/ceres-solver/lib/ -lceres
else:win32:CONFIG(debug, debug|release): LIBS += -L$$(CMAKE_PREFIX_PATH)/ceres-solver/lib/ -lceres-debug
else:unix: LIBS += -L$$(CMAKE_PREFIX_PATH)/ceres-solver/lib/ -lceres

INCLUDEPATH += $$(CMAKE_PREFIX_PATH)/ceres-solver/include
DEPENDPATH += $$(CMAKE_PREFIX_PATH)/ceres-solver/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$(CMAKE_PREFIX_PATH)/ceres-solver/lib/libceres.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$(CMAKE_PREFIX_PATH)/ceres-solver/lib/libceresd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$(CMAKE_PREFIX_PATH)/CMake/ceres-solver/lib/ceres.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$(CMAKE_PREFIX_PATH)/ceres-solver/lib/ceres-debug.lib
else:unix: PRE_TARGETDEPS += $$(CMAKE_PREFIX_PATH)/ceres-solver/lib/libceres.a

win32:CONFIG(release, debug|release): LIBS += -L$$(CMAKE_PREFIX_PATH)/glog/lib/ -lglog
else:win32:CONFIG(debug, debug|release): LIBS += -L$$(CMAKE_PREFIX_PATH)/glog/lib/ -lglogd
else:unix: LIBS += -L$$(CMAKE_PREFIX_PATH)/glog/lib/ -lglog

INCLUDEPATH += $$(CMAKE_PREFIX_PATH)/glog/include
DEPENDPATH += $$(CMAKE_PREFIX_PATH)/glog/include

win32:CONFIG(release, debug|release): LIBS += -L$$(CMAKE_PREFIX_PATH)/gflags/lib/ -lgflags_static
else:win32:CONFIG(debug, debug|release): LIBS += -L$$(CMAKE_PREFIX_PATH)/gflags/lib/ -lgflags_static_debug
else:unix: LIBS += -L$$(CMAKE_PREFIX_PATH)/gflags/lib/ -lgflags_static

INCLUDEPATH += $$(CMAKE_PREFIX_PATH)/gflags/include
DEPENDPATH += $$(CMAKE_PREFIX_PATH)/gflags/include

INCLUDEPATH += $$(CMAKE_PREFIX_PATH)/eigen3/include/eigen3
DEPENDPATH += $$(CMAKE_PREFIX_PATH)/eigen3/include/eigen3

win32: LIBS += -L$$PWD/ngspice/ -lngspice

INCLUDEPATH += $$PWD/ngspice
DEPENDPATH += $$PWD/ngspice

RESOURCES += \
    icons.qrc
