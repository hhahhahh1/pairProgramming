INCLUDE(${QT_USE_FILE})

SET(SOURCES
    main.cpp
    MainWindow.cpp
    FindDialog.cpp
    QuickDialog.cpp)

SET(HEADERS
    MainWindow.h
    FindDialog.h
    QuickDialog.h)

SET(RESOURCE
    ../Ned.qrc)

SET(QT_USE_QT3SUPPORT TRUE)

SET(TARGET_NAME ../Ned)

QT4_ADD_RESOURCES(RCC ${RESOURCE})
QT4_WRAP_CPP(MOCS ${HEADERS})

ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_EXECUTABLE(${TARGET_NAME}
    ${SOURCES}
    ${MOCS}
    ${RCC})
TARGET_LINK_LIBRARIES(${TARGET_NAME} ${QT_LIBRARIES} pthread)
