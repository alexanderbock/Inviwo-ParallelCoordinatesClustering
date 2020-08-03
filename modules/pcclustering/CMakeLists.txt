#--------------------------------------------------------------------
# ABufferGL Module
ivw_module(PCClustering)

#--------------------------------------------------------------------
# Add header files
set(HEADER_FILES   
    ${CMAKE_CURRENT_SOURCE_DIR}/datastructures/pcpdata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/cpr.h
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/parallelcoordinates_axis_permutation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/support.h
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/clusterexport.h
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/clusterreader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/clusterrenderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/densitymapfiltering.h
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/densitymapgenerator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/densitymaprenderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/otherclustering.h
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/pcpfileconverter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/pcpfiltering.h
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/pcpgui.h
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/pcpreader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/pcprenderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/pcpupload.h
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/pcpuploadrenderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/radarplotrenderer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/scatterplotrenderer.h

    # ${CMAKE_CURRENT_SOURCE_DIR}/ext/dbscan/dbscan.h
    
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/kmeans/src/kmeans.h
)
ivw_group("Header Files" ${HEADER_FILES})

#--------------------------------------------------------------------
# Add source files
set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/datastructures/pcpdata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/parallelcoordinates_axis_permutation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/support.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/clusterexport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/clusterreader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/clusterrenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/densitymapfiltering.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/densitymapgenerator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/densitymaprenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/otherclustering.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/pcpfileconverter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/pcpfiltering.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/pcpgui.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/pcpreader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/pcprenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/pcpupload.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/pcpuploadrenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/radarplotrenderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/scatterplotrenderer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/ext/dbscan/dbscan.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/kmeans/src/kmeans.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ext/kmeans/src/point.cpp

)
ivw_group("Source Files" ${SOURCE_FILES})

#--------------------------------------------------------------------
# Add shaders
set(SHADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/clusterdetection.comp
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/clusterrenderer.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/densitymapcounter.comp
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/densitymapfiltering_percentage.comp
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/densitymapfiltering_topology.comp
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/densitymapgenerator.comp
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/densitymaprenderer.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/pcpfiltering_counting.comp
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/pcpfiltering_filtering.comp
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/pcprenderer.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/pcprenderer.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/radarplotlines.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/radarplotrenderer.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/radarplotrenderer.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/scatterplotrenderer.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/glsl/scatterplotrenderer.frag
)
ivw_group("Shader Files" ${SHADER_FILES})

#--------------------------------------------------------------------
# Add definition
ivw_add_definition(__CL_ENABLE_EXCEPTIONS)

#--------------------------------------------------------------------
# Create module
ivw_create_module(${SOURCE_FILES} ${HEADER_FILES} ${SHADER_FILES})

#--------------------------------------------------------------------
# Handle external resources
if(IVW_USE_GENERATED_RESOURCES)
#--------------------------------------------------------------------
# Generate shader resource (if GENERATE_SHADER_RESOURCES is checked)
ivw_generate_shader_resource(${CMAKE_CURRENT_SOURCE_DIR}/glsl ${SHADER_FILES})
else()
#--------------------------------------------------------------------
# Add shader directory to pack
ivw_add_to_module_pack(${CMAKE_CURRENT_SOURCE_DIR}/glsl)
endif()