QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
LIBS += -lopengl32 -lglu32

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CoreTime.cpp \
    component.cpp \
    coreengine.cpp \
    cube.cpp \
    editor.cpp \
    glwindow.cpp \
    inspector.cpp \
    level.cpp \
    level1.cpp \
    levelscreator.cpp \
    main.cpp \
    model.cpp \
    transform.cpp \
    vector3.cpp

HEADERS += \
    CoreTime.h \
    component.h \
    coreengine.h \
    cube.h \
    editor.h \
    glwindow.h \
    inspector.h \
    level.h \
    level1.h \
    levelscreator.h \
    model.h \
    transform.h \
    vector3.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Models.qrc
