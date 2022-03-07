##########################################################################################
### Change the next line for making new plugins
##########################################################################################
set (PLUGINPROJECT Main)

include (../CMakePluginCommon.txt)

##########################################################################################
### Add Custom CMake Code after here
##########################################################################################


##########################################################################################
### Add dependencies for drivers below
###########################################################################################
if (TARGET libmcdriver_marlin) 
	add_dependencies (${PLUGINNAME} libmcdriver_marlin)
endif()
if (TARGET libmcdriver_camera) 
	add_dependencies (${PLUGINNAME} libmcdriver_camera)
endif()
