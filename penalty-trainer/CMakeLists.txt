# trainer

aux_source_directory(. TRAINER)

add_executable(penalty_trainer
  ${TRAINER}
  main_trainer.cpp
  )

target_include_directories(penalty_trainer
  PRIVATE
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_BINARY_DIR}
  PUBLIC
  ${Boost_INCLUDE_DIRS}
  ${LIBRCSC_INCLUDE_DIR}
  )

target_link_libraries(penalty_trainer
  PUBLIC
  ${LIBRCSC_LIB}
  Boost::system
  ZLIB::ZLIB
  )

set_target_properties(penalty_trainer
  PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../bin
  RUNTIME_OUTPUT_NAME ${CMAKE_PROJECT_NAME}_penalty_trainer
  )

