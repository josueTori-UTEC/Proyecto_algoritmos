cmake_minimum_required(VERSION 3.10)
project(proyecto_enC)

set(CMAKE_CXX_STANDARD 11)


set(SOURCES
        main.cpp
        Quadtree.cpp
        PointQuadtree.cpp
)


set(HEADERS
        Point.h
        Rectangle.h
        Quadtree.h
        PointQuadtree.h
)

add_executable(proyecto_enC ${SOURCES} ${HEADERS})
