cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(test_det_1)

set(CMAKE_PREFIX_PATH /home/user/Documents/geant4.10.06_build)

option(WITH_GEANT4_UIVIS "Build project with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(test_det_1 main.cc ${sources} ${headers})
target_link_libraries(test_det_1 ${Geant4_LIBRARIES})

set(Si_det_SCRIPTS
        init_vis.mac
        vis.mac
        run.mac
        loop_1_run.mac
        loop_2_run.mac
        gps_gun_vis.mac
        gps_gun.mac
        test.stl
        )

foreach(_script ${Si_det_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()


install(TARGETS test_det_1 DESTINATION bin)


