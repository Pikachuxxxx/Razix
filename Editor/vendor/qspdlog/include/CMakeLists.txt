add_library(
  qspdlog_interface INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/qspdlog/qspdlog.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/qspdlog/qabstract_spdlog_toolbar.hpp)
add_library(qspdlog::interface ALIAS qspdlog_interface)

target_include_directories(qspdlog_interface
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
