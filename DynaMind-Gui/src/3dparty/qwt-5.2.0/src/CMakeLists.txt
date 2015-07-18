file(GLOB QWT_CPPS *.cpp)
file(GLOB QWT_H *.h)
QT4_WRAP_CPP(QWT_CPPS ${QWT_H})

add_library(qwt MODULE ${QWT_CPPS})
target_link_libraries(qwt ${QT_LIBRARIES} )
