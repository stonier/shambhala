##############################################################################
# Macros
##############################################################################

macro(shambhala_add_demo name)
  add_executable(demo_${name} ${name}.cpp)
  target_link_libraries(demo_${name} PUBLIC drake::drake)
  set_target_properties(demo_${name} PROPERTIES OUTPUT_NAME demo_${name})
  install(TARGETS demo_${name} RUNTIME DESTINATION lib/${PROJECT_NAME})
endmacro()

##############################################################################
# Executables
##############################################################################

shambhala_add_demo(external_update)