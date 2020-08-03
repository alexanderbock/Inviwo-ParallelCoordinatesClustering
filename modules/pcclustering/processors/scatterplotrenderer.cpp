#include <modules/pcclustering/processors/scatterplotrenderer.h>

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

const ProcessorInfo ScatterPlotRenderer::processorInfo_{
    "pers.bock.ScatterPlotRenderer",  // Class identifier
    "ScatterPlot Renderer",            // Display name
    "Renderer",               // Category
    CodeState::Experimental,          // Code state
    Tags::CPU,                  // Tags
};

const ProcessorInfo ScatterPlotRenderer::getProcessorInfo() const {
    return processorInfo_;
}

ScatterPlotRenderer::ScatterPlotRenderer()
    : Processor()
    , _inport("data")
    , _coloringData("color")
    , _outport("image")
    , _xAxisSelection("_xAxisSelection", "X Axis")
    , _yAxisSelection("_yAxisSelection", "Y Axis")
    , _glyphSize("_glyphSize", "Glyph Size")
    , _excludeZero("_excludeZero", "Exclude Zero Cluster", false)
    , _transFunc("transferFunction", "Transfer Function")
    , _shader("scatterplotrenderer.vert", "scatterplotrenderer.frag")
{
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    float data[2] = { 0.f, 0.f };
    glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float), data, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glBindVertexArray(0);
    
    addPort(_inport);
    addPort(_coloringData);
    _coloringData.setOptional(true);
    _inport.onChange([this]() { updateOptionProperties();  });

    addPort(_outport);

    addProperty(_xAxisSelection);
    addProperty(_yAxisSelection);
    addProperty(_glyphSize);
    addProperty(_transFunc);
    addProperty(_excludeZero);

    _transFunc.get().clearPoints();
    _transFunc.get().addPoint(vec2(0, 1), vec4(0, 0, 0, 1));
    _transFunc.get().addPoint(vec2(1, 1), vec4(1, 1, 1, 1));

    _shader.onReload([this]() { invalidate(InvalidationLevel::InvalidOutput); });
}

ScatterPlotRenderer::~ScatterPlotRenderer() {
    glDeleteVertexArrays(1, &_vao);
    glDeleteBuffers(1, &_vbo);
}

void ScatterPlotRenderer::updateOptionProperties() {
    if (!_inport.hasData())
        return;

    _xAxisSelection.clearOptions();
    _yAxisSelection.clearOptions();
    for (int i = 0; i < _inport.getData()->nDimensions; ++i) {
        _xAxisSelection.addOption(std::to_string(i), std::to_string(i), i);
        _yAxisSelection.addOption(std::to_string(i), std::to_string(i), i);
    }
}

void ScatterPlotRenderer::process() {
    if (!_inport.hasData())
        return;
    std::shared_ptr<const ParallelCoordinatesPlotData> data = _inport.getData();

    utilgl::ClearColor colorState(glm::vec4(0.0));
    utilgl::activateAndClearTarget(_outport);

    utilgl::GlBoolState depthTest(GL_DEPTH_TEST, false);
    utilgl::BlendModeEquationState blendEquation(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_FUNC_ADD);

    _shader.activate();

    _shader.setUniform("_nDimensions", data->nDimensions);
    _shader.setUniform("_nData", data->nValues / data->nDimensions);
    _shader.setUniform("_xAxisSelection", _xAxisSelection.get());
    _shader.setUniform("_yAxisSelection", _yAxisSelection.get());
    _shader.setUniform("_excludeZero", _excludeZero);

    TextureUnit tfUnit;
    utilgl::bindTexture(_transFunc, tfUnit);
    _shader.setUniform("_transFunc", tfUnit.getUnitNumber());

    glBindVertexArray(_vao);

    bool hasColoringData = _coloringData.hasData() && _coloringData.getData()->hasData;
    _shader.setUniform("_hasColoringData", hasColoringData);
    if (hasColoringData) {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _coloringData.getData()->ssboColor);
    }

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, data->ssboData);

    glEnable(GL_POINT_SMOOTH);
    glPointSize(_glyphSize * 64.f);

    glDrawArraysInstanced(GL_POINTS, 0, 1, data->nValues);

    glBindVertexArray(0);
    glDisable(GL_POINT_SMOOTH);

    _shader.deactivate();
    utilgl::deactivateCurrentTarget();
}

}  // namespace

