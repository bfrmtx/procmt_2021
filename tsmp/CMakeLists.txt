project(tsmp LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(${CMAKE_SOURCE_DIR}/cmake/rpath.cmake)

IF(WIN32)
    message("WINDOWS build, msvc or msys2")

    # On WINDOWS it is easier to make STATIC libs
    # Some libs will be dynamic (dll); add PATH to the lib folder
    add_compile_definitions(_USE_MATH_DEFINES _msvc)
    IF(CMAKE_BUILD_TYPE MATCHES DEBUG)
        message("debug build ---------------------")
        add_compile_options(-DDEBUG)
        set(CMAKE_VERBOSE_MAKEFILE ON)
    ENDIF()
    IF(CMAKE_BUILD_TYPE MATCHES RELEASE)
        message("release build ---------------------")
        add_compile_options(-DNDEBUG -O2)
    ENDIF()
ENDIF()

find_package(Qt6 REQUIRED COMPONENTS Core)

set(SOURCES
    adu06cal.cpp
    adu07cal.cpp
    atmfile.cpp
    atsfilename.cpp
    atsheader72.cpp
    atsheader75.cpp
    atsheader80.cpp
    coherency.cpp
    date_utils.cpp
    eDateTime.cpp
    efp05cal.cpp
    emi_bfcal.cpp
    emimt24header.cpp
    emisyscal.cpp
    fgs01cal.cpp
    fgs02cal.cpp
    fubheader.cpp
    icalibration.cpp
    itsheader.cpp
    koordi.cpp
    ltscal.cpp
    main.cpp
    mfs07cal.cpp
    mtstd.cpp
    mtxcal.cpp
    my_valarray_double.cpp
    plaincal.cpp
    spectra.cpp
    string_utils.cpp
    tsdata.cpp
    tsheader.cpp
)

set(HEADERS
    adu06cal.h
    adu07cal.h
    allinclude.h
    atmfile.h
    ats_consts.h
    atsfilename.h
    atsheader72.h
    atsheader75.h
    atsheader80.h
    bfr_iter.h
    bt_utils.h
    c_slice_iter.h
    coherency.h
    date_utils.h
    eDateTime.h
    efp05cal.h
    emi_bfcal.h
    emimt24header.h
    emisyscal.h
    fgs01cal.h
    fgs02cal.h
    fubheader.h
    icalibration.h
    itsheader.h
    koordi.h
    ltscal.h
    mfs07cal.h
    mtstd.h
    mtx_iter.h
    mtxcal.h
    my_valarray.h
    my_valarray_double.h
    mysql_simple_vector.h
    myvector.h
    plaincal.h
    slice_iter.h
    spectra.h
    string_utils.h
    tsdata.h
    tsheader.h
)

set(PROJECT_SOURCES ${HEADERS} ${SOURCES})

include(${CMAKE_SOURCE_DIR}/cmake/qt_console_1.cmake)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/tsmp.pdf DESTINATION ${CMAKE_INSTALL_PREFIX}/doc)
