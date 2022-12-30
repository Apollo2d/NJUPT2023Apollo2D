# trainer
aux_source_directory(. HFO)

add_executable(hfo_trainer
  ${HFO}
  main_trainer.cpp
  )

target_include_directories(hfo_trainer
  PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}
  PUBLIC
  ${Boost_INCLUDE_DIRS}
  ${LIBRCSC_INCLUDE_DIR}
  )

target_link_libraries(hfo_trainer
  PUBLIC
  ${LIBRCSC_LIB}
  Boost::system
  ZLIB::ZLIB
  )

set_target_properties(hfo_trainer
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${OUTPUT_NAME}
  RUNTIME_OUTPUT_NAME ${CMAKE_PROJECT_NAME}_hfo_trainer
  )

