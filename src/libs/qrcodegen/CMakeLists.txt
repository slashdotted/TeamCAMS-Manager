add_library(QrCodeGenerator STATIC
    src/qrcodegen.cpp
    src/qrcodegen.h
)

find_package(Qt6 REQUIRED COMPONENTS Gui)
target_link_libraries(QrCodeGenerator PRIVATE Qt6::Gui)
