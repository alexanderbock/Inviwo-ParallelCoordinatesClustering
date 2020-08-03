#include <modules/pcclustering/pcclusteringmodule.h>

#include <modules/opengl/shader/shadermanager.h>

#include <modules/pcclustering/processors/clusterexport.h>
#include <modules/pcclustering/processors/clusterrenderer.h>
#include <modules/pcclustering/processors/densitymapfiltering.h>
#include <modules/pcclustering/processors/densitymapgenerator.h>
#include <modules/pcclustering/processors/densitymaprenderer.h>
#include <modules/pcclustering/processors/pcpfileconverter.h>
#include <modules/pcclustering/processors/pcpfiltering.h>
#include <modules/pcclustering/processors/pcpgui.h>
#include <modules/pcclustering/processors/pcpreader.h>
#include <modules/pcclustering/processors/pcprenderer.h>
#include <modules/pcclustering/processors/pcpupload.h>
#include <modules/pcclustering/processors/pcpuploadrenderer.h>
#include <modules/pcclustering/processors/radarplotrenderer.h>
#include <modules/pcclustering/processors/scatterplotrenderer.h>
#include <modules/pcclustering/processors/otherclustering.h>
#include <modules/pcclustering/processors/clusterreader.h>

namespace inviwo {

PCClusteringModule::PCClusteringModule(InviwoApplication* app)
    : InviwoModule(app, "PCClustering")
{
    ShaderManager::getPtr()->addShaderSearchPath(getPath(ModulePath::GLSL));

    registerProcessor<ClusterExport>();
    registerProcessor<ClusterRenderer>();
    registerProcessor<DensityMapFiltering>();
    registerProcessor<DensityMapGenerator>();
    registerProcessor<DensityMapRenderer>();
    registerProcessor<PCPFileConverter>();
    registerProcessor<PCPFiltering>();
    registerProcessor<PCPGui>();
    registerProcessor<PCPReader>();
    registerProcessor<PCPRenderer>();
    registerProcessor<PCPUpload>();
    registerProcessor<PCPUploadRenderer>();
    registerProcessor<RadarPlotRenderer>();
    registerProcessor<ScatterPlotRenderer>();
    registerProcessor<PCPOtherClustering>();
    registerProcessor<ClusterReader>();
}

}  // namespace
