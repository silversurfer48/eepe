DEFINES += SKY=1
QT += network \
      xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport
include(../../qextserialport.pri)
HEADERS += mainwindow.h \
    file.h \
    pers.h \
    myeeprom.h \
    modeledit.h \
    generaledit.h \
    mdichild.h \
    helpers.h \
    mixerdialog.h \
    burnconfigdialog.h \
    avroutputdialog.h \ 
    simulatordialog.h \
    donatorsdialog.h \
    printdialog.h \
    preferencesdialog.h \
    mixerslist.h \
    downloaddialog.h \
    stamp-eepskye.h \
    customizesplashdialog.h \
    ../../telemetry.h \
    ../../reviewOutput.h \
    ../../node.h \
    ../../edge.h \
    eeprom_rlc.h \
    VoiceAlarmDialog.h
SOURCES += main.cpp \
    mainwindow.cpp \
    file.cpp \
    pers.cpp \
    modeledit.cpp \
    generaledit.cpp \
    mdichild.cpp \
    mixerdialog.cpp \
    burnconfigdialog.cpp \
    avroutputdialog.cpp \ 
    simulatordialog.cpp \
    donatorsdialog.cpp \
    printdialog.cpp \
    preferencesdialog.cpp \
    mixerslist.cpp \
    downloaddialog.cpp \
    customizesplashdialog.cpp \
    ../../telemetry.cpp \
    ../../reviewOutput.cpp \
    helpers.cpp \
    ../../node.cpp \
    ../../edge.cpp \
    eeprom_rlc.cpp \
    VoiceAlarmDialog.cpp
unix {
SOURCES += mountlist.cpp
}
TEMPLATE = app
FORMS += modeledit.ui \
    generaledit.ui \
    mixerdialog.ui \
    burnconfigdialog.ui \
    avroutputdialog.ui \
    simulatordialog.ui \
    donatorsdialog.ui \
    printdialog.ui \
    preferencesdialog.ui \
    downloaddialog.ui \
    customizesplashdialog.ui \
    ../../telemetryDialog.ui \
    ../../reviewOutput.ui \
    VoiceAlarmDialog.ui
TRANSLATIONS = eepe_.ts    \
               eepe_he.ts  \
               eepe_pt.ts  \
               eepe_ru.ts  \
               eepe_de.ts  \
               eepe_es.ts  \
               eepe_fr.ts
RESOURCES += eepskye.qrc
TARGET = eepskye

unix {
LANGS.path = /usr/bin/eepefiles
LANGS.files = lang/*.qm

ICON.path = /usr/bin/eepefiles
ICON.files += icone.svg

SHORTCUT.path = /usr/share/applications/
SHORTCUT.files += eepskye.desktop

BINFILE.files += eepskye
BINFILE.path = /usr/bin
BINFILE.commands = rm -rf /usr/bin/eepskye
#This removes old eepe file or directory

INSTALLS = BINFILE
INSTALLS += LANGS
INSTALLS += ICON
INSTALLS += SHORTCUT
}

win32 {
RC_FILE += icon.rc
}

mac {
ICON = eepskye.icns
}

OTHER_FILES +=
