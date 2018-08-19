
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Find the QtWidgets library
find_package(Qt5Widgets)

qt5_wrap_ui(ui_FILES mainwindow.ui)

aux_source_directory(. SRCS)
add_executable(gltest ${SRCS} ${ui_FILES})

target_link_libraries(gltest
    Qt5::Widgets
    )
