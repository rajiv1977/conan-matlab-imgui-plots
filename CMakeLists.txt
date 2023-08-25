cmake_minimum_required(VERSION 3.27.0)

project(matlab-imgui-plot-conan LANGUAGES CXX C VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

find_package(glfw3 REQUIRED)
find_package(glew REQUIRED)
find_package(glu REQUIRED)
find_package(imgui REQUIRED)
find_package(implot REQUIRED)

if(WIN32) 
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:100000000")
find_package(opengl REQUIRED)
endif(WIN32) 

# find Matlab's matrix and mex libraries
find_package(Matlab COMPONENTS MAT_LIBRARY MX_LIBRARY)
if (NOT Matlab_FOUND)
    message(FATAL_ERROR "Matlab dependencies not found. Is the MATLAB_PATH environment variable set?")
endif()

set(CPACK_NSIS_CONTACT "rajiv.sithiravel@gmail.com")
	
# build the mex
matlab_add_mex(
    NAME imGuiPlotMex
    SHARED
    SRC bindings/imgui_impl_glfw.cpp
        bindings/imgui_impl_glfw.h
        bindings/imgui_impl_opengl3.cpp
        bindings/imgui_impl_opengl3.h
        bindings/imgui_impl_opengl3_loader.h 
		include/MatlabImGuiPlot.h
		source/MatlabImGuiPlot.cpp
		source/imGuiPlotMex.cpp 
    LINK_TO imgui::imgui glfw opengl::opengl GLEW::GLEW glu::glu implot::implot
)
target_include_directories(imGuiPlotMex PRIVATE ${PROJECT_SOURCE_DIR}/bindings ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/source )

include(CTest) 
# CTest sets the BUILD_TESTING variable to ON
if (BUILD_TESTING)

add_executable( ${PROJECT_NAME}
                bindings/imgui_impl_glfw.cpp
                bindings/imgui_impl_glfw.h
                bindings/imgui_impl_opengl3.cpp
                bindings/imgui_impl_opengl3.h
                bindings/imgui_impl_opengl3_loader.h
				include/MatlabImGuiPlot.h
				source/MatlabImGuiPlot.cpp
				Test/CorePlots.h
                Test/main.cpp)

if(WIN32) 				
target_compile_options(${PROJECT_NAME} PRIVATE "/Od")   
endif(WIN32)

target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} imgui::imgui glfw opengl::opengl GLEW::GLEW glu::glu implot::implot)

endif()
