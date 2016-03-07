#include <modules/pcclustering/processors/pcpfiltering.h>

#include <modules/opengl/texture/textureutils.h>

//#include "volumesource.h"
//#include <inviwo/core/resources/resourcemanager.h>
//#include <inviwo/core/resources/templateresource.h>
//#include <inviwo/core/common/inviwoapplication.h>
//#include <inviwo/core/util/filesystem.h>
//#include <inviwo/core/util/raiiutils.h>
//#include <inviwo/core/io/datareaderfactory.h>
//#include <inviwo/core/io/rawvolumereader.h>
//#include <inviwo/core/network/processornetwork.h>
//#include <inviwo/core/datastructures/volume/volumeram.h>
//#include <inviwo/core/common/inviwoapplication.h>
//#include <inviwo/core/io/datareaderexception.h>
//
//#include <math.h>

namespace inviwo {

const ProcessorInfo PCPFiltering::processorInfo_{
    "pers.bock.PCPFiltering",  // Class identifier
    "PCP Filtering",            // Display name
    "Data Filtering",               // Category
    CodeState::Experimental,          // Code state
    Tags::CPU,                  // Tags
};

const ProcessorInfo PCPFiltering::getProcessorInfo() const {
    return processorInfo_;
}

PCPFiltering::PCPFiltering()
    : Processor()
    , _binInport("in.bins")
    , _pcpInport("in.pcp")
    , _pcpOutport("out.pcp")
    , _filteringMethod("_filteringMethod", "Filtering Method")
    , _countingShader({{ShaderType::Compute, "pcpfiltering_counting.comp" }}, Shader::Build::No)
    , _filteringShader({ { ShaderType::Compute, "pcpfiltering_filtering.comp" } }, Shader::Build::No)
{
    addPort(_binInport);
    addPort(_pcpInport);

    addPort(_pcpOutport);

    _countingShader.getShaderObject(ShaderType::Compute)->addShaderExtension(
        "GL_ARB_compute_variable_group_size",
        true
    );

    _filteringShader.getShaderObject(ShaderType::Compute)->addShaderExtension(
        "GL_ARB_compute_variable_group_size",
        true
    );


    _countingShader.build();
    _filteringShader.build();

    _countingShader.onReload([this]() {invalidate(InvalidationLevel::InvalidOutput); });
    _filteringShader.onReload([this]() {invalidate(InvalidationLevel::InvalidOutput); });
}

PCPFiltering::~PCPFiltering() {
}


void PCPFiltering::process() {
    if (!_binInport.hasData())
        return;

    if (!_pcpInport.hasData())
        return;

    std::shared_ptr<const BinningData> binInData = _binInport.getData();
    std::shared_ptr<const ParallelCoordinatesPlotData> pcpInData = _pcpInport.getData();

    ParallelCoordinatesPlotData* pcpOutData = new ParallelCoordinatesPlotData;
    pcpOutData->nDimensions = pcpInData->nDimensions;
    //pcpOutData->ssboMinMax = pcpInData->ssboMinMax;
    LGL_ERROR;
    glGenBuffers(1, &pcpOutData->ssboData);
    glGenBuffers(1, &pcpOutData->ssboMinMax);

    glBindBuffer(GL_COPY_WRITE_BUFFER, pcpOutData->ssboMinMax);
    glBufferData(
        GL_COPY_WRITE_BUFFER,
        pcpInData->nDimensions * 2 * sizeof(float),
        nullptr,
        GL_DYNAMIC_DRAW
    );

    glBindBuffer(GL_COPY_READ_BUFFER, pcpInData->ssboMinMax);
    glCopyBufferSubData(
        GL_COPY_READ_BUFFER,
        GL_COPY_WRITE_BUFFER,
        0,
        0,
        pcpInData->nDimensions * 2 * sizeof(float)
        );
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    LGL_ERROR;

    filterData(pcpInData.get(), binInData.get(), pcpOutData);
    LGL_ERROR;


    _pcpOutport.setData(pcpOutData);
    LGL_ERROR;

}

void PCPFiltering::filterData(const ParallelCoordinatesPlotData* inData,
                               const BinningData* binData, ParallelCoordinatesPlotData* outData)
{
    // First count the number of data values that will be written
    _countingShader.activate();
    _countingShader.setUniform("_nDimensions", inData->nDimensions);
    _countingShader.setUniform("_nBins", binData->nBins);

    // Storage for number of data values (not one value per dimension)
    GLuint nValuesCounter = 0;
    glGenBuffers(1, &nValuesCounter);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, nValuesCounter);
    glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    
    glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 0, nValuesCounter);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, inData->ssboMinMax);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, inData->ssboData);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, binData->ssboBins);

    LGL_ERROR;
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glDispatchComputeGroupSizeARB(
        inData->nValues / inData->nDimensions, 1, 1,
        1, 1, 1
        );
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    LGL_ERROR;

    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, nValuesCounter);
    int* ptr = (int*)glMapBuffer(GL_ATOMIC_COUNTER_BUFFER, GL_READ_WRITE);
    int nDataValues = ptr[0];
    glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
    glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
    LGL_ERROR;

    glDeleteBuffers(1, &nValuesCounter);
    LGL_ERROR;

    //LogInfo(nDataValues);

    outData->nValues = nDataValues * outData->nDimensions;
    _countingShader.deactivate();


    // Then actually write out the number of values
    _filteringShader.activate();
    _filteringShader.setUniform("_nDimensions", inData->nDimensions);
    _filteringShader.setUniform("_nBins", binData->nBins);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outData->ssboData);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        outData->nValues * sizeof(float),
        nullptr,
        GL_DYNAMIC_DRAW
    );
    LGL_ERROR;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    LGL_ERROR;

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, inData->ssboMinMax);
    LGL_ERROR;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, inData->ssboData);
    LGL_ERROR;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, binData->ssboBins);
    LGL_ERROR;
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, outData->ssboData);
    LGL_ERROR;
    
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    glDispatchComputeGroupSizeARB(
        inData->nValues / inData->nDimensions, 1, 1,
        1, 1, 1
        );
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
    LGL_ERROR;

    _filteringShader.deactivate();
    LGL_ERROR;
}

}  // namespace
