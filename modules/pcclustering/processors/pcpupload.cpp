#include <modules/pcclustering/processors/pcpupload.h>

#include <modules/opengl/openglutils.h>

//#include <inviwo/core/resources/resourcemanager.h>
//#include <inviwo/core/resources/templateresource.h>
//#include <inviwo/core/common/inviwoapplication.h>
//#include <inviwo/core/util/filesystem.h>
//#include <inviwo/core/io/datareaderfactory.h>
//#include <inviwo/core/io/rawvolumereader.h>
//#include <inviwo/core/network/processornetwork.h>
//#include <inviwo/core/datastructures/volume/volumeram.h>
//#include <inviwo/core/common/inviwoapplication.h>
//#include <inviwo/core/io/datareaderexception.h>
//
//#include <math.h>

namespace inviwo {

const ProcessorInfo PCPUpload::processorInfo_{
    "pers.bock.PCPUpload",  // Class identifier
    "PCP Upload",            // Display name
    "Data Input",               // Category
    CodeState::Experimental,          // Code state
    Tags::CPU,                  // Tags
};

const ProcessorInfo PCPUpload::getProcessorInfo() const {
    return processorInfo_;
}

PCPUpload::PCPUpload()
    : Processor()
    , _inport("inData")
    , _outport("outdata")
{
    addPort(_inport);
    addPort(_outport);

    _data = std::make_shared<ParallelCoordinatesPlotData>();
}

void PCPUpload::process() {
    std::shared_ptr<const ParallelCoordinatesPlotRawData> inData = _inport.getData();
    if (inData->data.empty())
        return;

    glGenBuffers(1, &(_data->ssboData));
    _data->nValues = static_cast<int>(inData->data.size());
    _data->nDimensions = static_cast<int>(inData->minMax.size());

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _data->ssboData);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        _data->nValues * sizeof(float),
        inData->data.data(),
        GL_STATIC_DRAW
    );

    _outport.setData(_data);
}

}  // namespace
