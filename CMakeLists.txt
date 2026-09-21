cmake_minimum_required(VERSION 3.5)

project(
  TeamCAMSManager
  VERSION 2026.9
  LANGUAGES C CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FIXME: Headless builds need to be fixed (or removed)
option(WITH_GUI "Enable manager GUI" ON)

set(QT_COMPONENTS Widgets LinguistTools Network Qml Svg)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CONFIG MATCHES "Debug")
  set(ENABLE_QT_TEST TRUE)
  list(APPEND QT_COMPONENTS Test)
else()
  set(ENABLE_QT_TEST FALSE)
endif()

if(EMSCRIPTEN)
  list(APPEND QT_COMPONENTS WebSockets)
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS ${QT_COMPONENTS})

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS ${QT_COMPONENTS})

set(TS_FILES i18n/teamcamsmanager_it.ts i18n/teamcamsmanager_fr.ts
             i18n/teamcamsmanager_de.ts i18n/teamcamsmanager_es.ts)

add_custom_target(
  release_translations
  COMMAND lrelease ${TS_FILES}
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

set(TEST_SOURCES
    src/tests/testcabin.cpp
    src/tests/testcabin.h
    src/tests/testcomponent.cpp
    src/tests/testcomponent.h
    src/tests/testcontrol.cpp
    src/tests/testcontrol.h
    src/tests/testcooler.cpp
    src/tests/testcooler.h
    src/tests/testcrew.cpp
    src/tests/testcrew.h
    src/tests/testdehumidifier.cpp
    src/tests/testdehumidifier.h
    src/tests/testexperiment.cpp
    src/tests/testexperiment.h
    src/tests/testexperimentlist.cpp
    src/tests/testexperimentlist.h
    src/tests/testheater.cpp
    src/tests/testheater.h
    src/tests/testmixer.cpp
    src/tests/testmixer.h
    src/tests/testpipe.cpp
    src/tests/testpipe.h
    src/tests/testregistry.cpp
    src/tests/testregistry.h
    src/tests/testscheduler.cpp
    src/tests/testscheduler.h
    src/tests/testscrubber.cpp
    src/tests/testscrubber.h
    src/tests/testsensor.cpp
    src/tests/testsensor.h
    src/tests/testsimulator.cpp
    src/tests/testsimulator.h
    src/tests/testtank.cpp
    src/tests/testtank.h
    src/tests/testtriggers.cpp
    src/tests/testtriggers.h
    src/tests/testuserlist.cpp
    src/tests/testuserlist.h
    src/tests/testvalve.cpp
    src/tests/testvalve.h
    src/tests/testvent.cpp
    src/tests/testvent.h
    src/tests/testxmlconfigurationparser.cpp
    src/tests/testxmlconfigurationparser.h
    src/testscripts/test_cool_ineff_fault.xml
    src/testscripts/test_cool_sensor_fault.xml
    src/testscripts/test_cool_sensor_fault_lower.xml
    src/testscripts/test_cool_sensor_fault_upper.xml
    src/testscripts/test_cool_stuck_on_fault.xml
    src/testscripts/test_dehum_ineff_fault.xml
    src/testscripts/test_dehum_sensor_fault_lower.xml
    src/testscripts/test_dehum_sensor_fault_upper.xml
    src/testscripts/test_dehumidifier_sensor_fault.xml
    src/testscripts/test_dehumidifier_stuck_on_fault.xml
    src/testscripts/test_error_clear.xml
    src/testscripts/test_error_diagnostic_miss.xml
    src/testscripts/test_heat_ineff_fault.xml
    src/testscripts/test_heat_sensor_fault.xml
    src/testscripts/test_heat_sensor_fault_lower.xml
    src/testscripts/test_heat_sensor_fault_upper.xml
    src/testscripts/test_heater_stuck_on_fault.xml
    src/testscripts/test_mixer_block_fault.xml
    src/testscripts/test_ni_sensor_fault.xml
    src/testscripts/test_ni_sensor_fault_lower.xml
    src/testscripts/test_ni_sensor_fault_upper.xml
    src/testscripts/test_ni_valve_block_fault.xml
    src/testscripts/test_ni_valve_leak_fault.xml
    src/testscripts/test_ni_valve_stuck_open_fault.xml
    src/testscripts/test_ox_sensor_fault.xml
    src/testscripts/test_ox_sensor_fault_lower.xml
    src/testscripts/test_ox_sensor_fault_upper.xml
    src/testscripts/test_ox_valve_block_fault.xml
    src/testscripts/test_ox_valve_leak_fault.xml
    src/testscripts/test_ox_valve_stuck_open_fault.xml
    src/testscripts/test_scrub_ineff_fault.xml
    src/testscripts/test_scrub_sensor_fault_lower.xml
    src/testscripts/test_scrub_sensor_fault_upper.xml
    src/testscripts/test_scrub_stuck_on_fault.xml
    src/testscripts/test_scrubber_sensor_fault.xml
    src/testscripts/test_vent_ineff_fault.xml
    src/testscripts/test_vent_sensor_fault.xml
    src/testscripts/test_vent_sensor_fault_lower.xml
    src/testscripts/test_vent_sensor_fault_upper.xml
    src/testscripts/test_vent_stuck_on_fault.xml)

set(PROJECT_SOURCES
    src/main.cpp
    ${TS_FILES}
    src/events/scheduler.cpp
    src/events/scheduler.h
    src/events/triggers.cpp
    src/events/triggers.h
    src/experiment/operators/operatoradapter.cpp
    src/experiment/operators/operatoradapter.h
    src/experiment/config/xmlconfighandler.cpp
    src/experiment/config/xmlconfighandler.h
    src/experiment/config/xmlconfigparser.cpp
    src/experiment/config/xmlconfigparser.h
    src/experiment/operators/operator.cpp
    src/experiment/operators/operator.h
    src/experiment/operators/operatorsmanager.cpp
    src/experiment/operators/operatorsmanager.h
    src/experiment/operators/messagefilter.cpp
    src/experiment/operators/messagefilter.h
    src/experiment/content/contentlibrary.cpp
    src/experiment/content/contentlibrary.h
    src/experiment/operators/access/accessmanager.cpp
    src/experiment/operators/access/accessmanager.h
    src/experiment/operators/access/accesscard.cpp
    src/experiment/operators/access/accesscard.h
    src/experiment/logging/consolelogger.cpp
    src/experiment/logging/consolelogger.h
    src/experiment/logging/datalogger.h
    src/experiment/logging/filelogger.cpp
    src/experiment/logging/filelogger.h
    src/experiment/logging/loggingmanager.cpp
    src/experiment/logging/loggingmanager.h
    src/experiment/runtime/executionmanager.cpp
    src/experiment/runtime/executionmanager.h
    src/experiment/scripting/scriptingmanager.cpp
    src/experiment/scripting/scriptingmanager.h
    src/experiment/exceptions.h
    src/experiment/experiment.cpp
    src/experiment/experiment.h
    src/gui/delegates/feedbackonupdatedelegate.cpp
    src/gui/delegates/feedbackonupdatedelegate.h
    src/gui/delegates/messagetypecombodelegate.cpp
    src/gui/delegates/messagetypecombodelegate.h
    src/gui/delegates/trafficdirectioncombodelegate.cpp
    src/gui/delegates/trafficdirectioncombodelegate.h
    src/gui/delegates/userrolecombodelegate.cpp
    src/gui/delegates/userrolecombodelegate.h
    src/gui/appconfigurationdialog.cpp
    src/gui/appconfigurationdialog.h
    src/gui/appconfigurationdialog.ui
    src/gui/configureexperimentdialog.cpp
    src/gui/configureexperimentdialog.h
    src/gui/configureexperimentdialog.ui
    src/gui/mainwindow.cpp
    src/gui/mainwindow.h
    src/gui/mainwindow.ui
    src/gui/newexperimentdialog.cpp
    src/gui/newexperimentdialog.h
    src/gui/newexperimentdialog.ui
    src/gui/asyncmessageboxfactory.h
    src/gui/asyncmessageboxfactory.cpp
    src/utils/persistentsettings.h
    src/utils/persistentsettings.cpp
    src/models/experimentlist.cpp
    src/models/experimentlist.h
    src/models/filterlist.cpp
    src/models/filterlist.h
    src/models/userlist.cpp
    src/models/userlist.h
    src/models/propertylist.cpp
    src/models/propertylist.h
    src/scripts/adaptive.js
    src/scripts/automation.js
    src/scripts/cams.js
    src/scripts/cooler_ineffective_fault.js
    src/scripts/cooler_sensor_fault.js
    src/scripts/cooler_stuckon_fault.js
    src/scripts/dehumidifier_ineffective_fault.js
    src/scripts/dehumidifier_stuckon_fault.js
    src/scripts/fault_definitions.js
    src/scripts/heater_ineffective_fault.js
    src/scripts/heater_sensor_fault.js
    src/scripts/heater_stuckon_fault.js
    src/scripts/humidity_sensor_fault.js
    src/scripts/interface.js
    src/scripts/mixer_block_fault.js
    src/scripts/nitrogen_block_fault.js
    src/scripts/nitrogen_leak_fault.js
    src/scripts/nitrogen_stuckopen_fault.js
    src/scripts/oxygen_block_fault.js
    src/scripts/oxygen_leak_fault.js
    src/scripts/oxygen_sensor_fault.js
    src/scripts/oxygen_stuckopen_fault.js
    src/scripts/pressure_sensor_fault.js
    src/scripts/scrubber_ineffective_fault.js
    src/scripts/scrubber_sensor_fault.js
    src/scripts/scrubber_stuckon_fault.js
    src/scripts/secondary_tasks.js
    src/scripts/tutorial.js
    src/scripts/vent_ineffective_fault.js
    src/scripts/vent_sensor_fault.js
    src/scripts/vent_stuckon_fault.js
    src/shim/connectionadapter.cpp
    src/shim/connectionadapter.h
    src/shim/contentlibraryadapter.cpp
    src/shim/contentlibraryadapter.h
    src/shim/executionadapter.cpp
    src/shim/executionadapter.h
    src/shim/experimentadapter.cpp
    src/shim/experimentadapter.h
    src/shim/loggingadapter.cpp
    src/shim/loggingadapter.h
    src/shim/messagefilteradapter.cpp
    src/shim/messagefilteradapter.h
    src/shim/registryadapter.cpp
    src/shim/registryadapter.h
    src/shim/scheduleradapter.cpp
    src/shim/scheduleradapter.h
    src/shim/scriptingadapter.cpp
    src/shim/scriptingadapter.h
    src/shim/simulatoradapter.cpp
    src/shim/simulatoradapter.h
    src/shim/triggersadapter.cpp
    src/shim/triggersadapter.h
    src/simulation/components/cabin.cpp
    src/simulation/components/cabin.h
    src/simulation/components/cooler.cpp
    src/simulation/components/cooler.h
    src/simulation/components/crew.cpp
    src/simulation/components/crew.h
    src/simulation/components/dehumidifier.cpp
    src/simulation/components/dehumidifier.h
    src/simulation/components/heater.cpp
    src/simulation/components/heater.h
    src/simulation/components/mixer.cpp
    src/simulation/components/mixer.h
    src/simulation/components/pipe.cpp
    src/simulation/components/pipe.h
    src/simulation/components/scrubber.cpp
    src/simulation/components/scrubber.h
    src/simulation/components/streamingcomponent.cpp
    src/simulation/components/streamingcomponent.h
    src/simulation/components/tank.cpp
    src/simulation/components/tank.h
    src/simulation/components/valve.cpp
    src/simulation/components/valve.h
    src/simulation/components/vent.cpp
    src/simulation/components/vent.h
    src/simulation/control/control.cpp
    src/simulation/control/control.h
    src/simulation/control/sensor.cpp
    src/simulation/control/sensor.h
    src/simulation/registry.cpp
    src/simulation/registry.h
    src/simulation/simulator.cpp
    src/simulation/simulator.h)

if(ENABLE_QT_TEST)
  list(APPEND PROJECT_SOURCES ${TEST_SOURCES})
endif()

set(NETWORK_CORE_SOURCES
    src/network/experimentserver.h
    src/network/experimentserver.cpp
    src/network/peer.h
    src/network/peer.cpp
    src/network/messagetransport.h
    src/network/messagetransport.cpp
    src/network/communicationmanager.h
    src/network/communicationmanager.cpp)

set(MONOCYPHER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libs/monocypher-4.0.3/src/monocypher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libs/monocypher-4.0.3/src/monocypher.c)

set(MQTTPROXY_SOURCES
    src/network/mqtt/mqtttransport.h
    src/network/mqtt/mqtttransport.cpp
    src/network/mqtt/mqttpeer.h
    src/network/mqtt/mqttpeer.cpp
    src/network/mqtt/mqtttopic.h
    src/network/mqtt/mqtttopic.cpp
    src/network/mqtt/mqttexperimentserver.h
    src/network/mqtt/mqttexperimentserver.cpp
    src/network/mqtt/mqttexperimentclient.h
    src/network/mqtt/mqttexperimentclient.cpp
    src/network/mqtt/jsonvocabularyencoder.h
    src/network/mqtt/jsonvocabularyencoder.cpp
    src/network/mqtt/mqttcommunicationmanager.h
    src/network/mqtt/mqttcommunicationmanager.cpp
    src/network/mqtt/mqttcrypto.h
    src/network/mqtt/mqttcrypto.cpp)

set(MQTTPROXY_CLIENT_SOURCES
    src/network/mqtt/mqtttransport.h src/network/mqtt/mqtttransport.cpp
    src/network/mqtt/mqtttopic.h src/network/mqtt/mqtttopic.cpp)

set(ICON_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/assets/app_icon.ico")
set(ICON_MACOS "${CMAKE_CURRENT_SOURCE_DIR}/assets/app_icon.icns")
set(ICON_LINUX "${CMAKE_CURRENT_SOURCE_DIR}/assets/app_icon.png")

if(WIN32)
  set(RES_FILE "${CMAKE_CURRENT_BINARY_DIR}/app_icon.rc")
  file(WRITE "${RES_FILE}" "IDI_ICON1 ICON DISCARDABLE \"${ICON_WINDOWS}\"\n")
  list(APPEND PROJECT_SOURCES "${RES_FILE}")
endif()

if(APPLE)
  list(APPEND PROJECT_SOURCES "${ICON_MACOS}")
  set_source_files_properties("${ICON_MACOS}" PROPERTIES MACOSX_PACKAGE_LOCATION
                                                         Resources)
endif()

qt_add_executable(
  TeamCAMSManager
  MANUAL_FINALIZATION
  ${PROJECT_SOURCES}
  ${MONOCYPHER_SOURCES}
  ${MQTTPROXY_SOURCES}
  ${NETWORK_CORE_SOURCES}
  src/resources.qrc)

target_compile_definitions(
  TeamCAMSManager PRIVATE $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
                          $<$<CONFIG:MinSizeRel>:QT_NO_DEBUG_OUTPUT>)

if(ENABLE_QT_TEST)
  target_compile_definitions(TeamCAMSManager PRIVATE QT_TEST_ENABLED)
else()
  target_compile_definitions(TeamCAMSManager PRIVATE)
endif()

# Enable if required target_compile_options( TeamCAMSManager PRIVATE
# -fsanitize=address -fno-omit-frame-pointer) target_link_options(
# TeamCAMSManager PRIVATE -fsanitize=address)

if(EMSCRIPTEN)
  target_compile_definitions(TeamCAMSManager PRIVATE NO_WEBSOCKET_SERVER)
  target_link_options(TeamCAMSManager PRIVATE "-lidbfs.js")
endif()

qt_add_translations(TeamCAMSManager TS_FILES ${TS_FILES})

if(WITH_GUI)
  target_compile_definitions(TeamCAMSManager PUBLIC MANAGERGUI=1)
endif()

add_subdirectory(src/libs)

set(MONOCYPHER_INCLUDE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/src/libs/monocypher-4.0.3/src")

target_include_directories(
  TeamCAMSManager
  PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/libs/qrcodegen/src/"
          ${MONOCYPHER_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(TEAM_CAMS_PRIVATE_LIBS
    Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Qml Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Svg QrCodeGenerator)

if(ENABLE_QT_TEST)
  list(APPEND TEAM_CAMS_PRIVATE_LIBS Qt${QT_VERSION_MAJOR}::Test)
endif()

if(EMSCRIPTEN)
  list(APPEND TEAM_CAMS_PRIVATE_LIBS Qt${QT_VERSION_MAJOR}::WebSockets)
endif()

target_link_libraries(TeamCAMSManager PRIVATE ${TEAM_CAMS_PRIVATE_LIBS})

set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER "ch.unifr.teamcams.manager")

set_target_properties(
  TeamCAMSManager
  PROPERTIES WIN32_EXECUTABLE TRUE
             MACOSX_BUNDLE TRUE
             ${BUNDLE_ID_OPTION} MACOSX_BUNDLE_BUNDLE_VERSION
             "${PROJECT_VERSION}" MACOSX_BUNDLE_SHORT_VERSION_STRING
             "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
             MACOSX_BUNDLE_ICON_FILE
             "app_icon.icns")

if(NOT APPLE AND NOT WIN32)
  install(
    FILES "${ICON_LINUX}"
    DESTINATION "share/icons/hicolor/512x512/apps"
    RENAME "teamcamsmanager.png")
  install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/TeamCAMSManager.desktop"
          DESTINATION "share/applications")
endif()

include(GNUInstallDirs)

install(
  TARGETS TeamCAMSManager
  BUNDLE DESTINATION .
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
  qt_finalize_executable(TeamCAMSManager)
endif()

if(QT_VERSION_MAJOR EQUAL 6
   AND NOT EMSCRIPTEN
   AND NOT APPLE
   AND NOT WIN32)
  install(TARGETS TeamCAMSManager RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
elseif(QT_VERSION_MAJOR EQUAL 6 AND NOT EMSCRIPTEN)
  include(QtCMakeMacros OPTIONAL)
  qt_generate_deploy_app_script(
    TARGET TeamCAMSManager OUTPUT_SCRIPT deploy_script.cmake
    NO_UNSUPPORTED_PLATFORM_ERROR)
  install(SCRIPT ${deploy_script.cmake})
endif()
