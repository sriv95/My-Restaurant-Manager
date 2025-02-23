QT       += core gui widgets charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ui/configwindow.cpp \
    ui/editmenu.cpp \
    ui/analysis.cpp \
    ui/employee.cpp \
    func/json.cpp \
    func/stocks.cpp \
    func/tables.cpp \
    main.cpp \
    mainwindow.cpp \
    ui/jsoncheck.cpp \
    ui/oderfood.cpp \
    ui/opentabledialog.cpp \
    ui/orderstock.cpp \
    ui/reserve.cpp \
    ui/statement.cpp \
    ui/stockwindow.cpp \

HEADERS += \
    ui/configwindow.h \
    ui/editmenu.h \
    ui/analysis.h \
    ui/employee.h \
    header/json.h \
    header/nlohmann-json.hpp \
    header/tables.h \
    mainwindow.h \
    ui/jsoncheck.h \
    ui/opentabledialog.h \
    ui/orderfood.h \
    ui/orderstock.h \
    ui/reserve.h \
    ui/statement.h \
    ui/stockwindow.h \

FORMS += \
    ui/configwindow.ui \
    ui/editmenu.ui \
    ui/analysis.ui \
    ui/employee.ui \
    mainwindow.ui \
    ui/jsoncheck.ui \
    ui/opentabledialog.ui \
    ui/orderstock.ui \
    ui/reserve.ui \
    ui/statement.ui \
    ui/stockwindow.ui \


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    data.json \

RC_FILE = app.rc