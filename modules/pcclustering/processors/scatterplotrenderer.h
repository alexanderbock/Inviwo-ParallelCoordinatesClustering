#ifndef IVW_SCATTERPLOTRENDERER_H
#define IVW_SCATTERPLOTRENDERER_H

#include <inviwo/core/processors/processor.h>

#include <modules/pcclustering/pcclusteringmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/optionproperty.h>

#include <modules/pcclustering/datastructures/pcpdata.h>

#include <inviwo/core/ports/imageport.h>

#include <modules/opengl/shader/shader.h>
#include <inviwo/core/properties/transferfunctionproperty.h>


namespace inviwo {

class IVW_MODULE_PCCLUSTERING_API ScatterPlotRenderer : public Processor {
public:
    const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;
    ScatterPlotRenderer();
    ~ScatterPlotRenderer();

    void process() override;

private:
    void updateOptionProperties();

    PCPDataInport _inport;
    ColoringDataInport _coloringData;

    ImageOutport _outport;

    OptionPropertyInt _xAxisSelection;
    OptionPropertyInt _yAxisSelection;
    FloatProperty _glyphSize;

    BoolProperty _excludeZero;

    TransferFunctionProperty _transFunc;

    Shader _shader;
    GLuint _vbo;
    GLuint _vao;
};

}  // namespace

#endif  // IVW_SCATTERPLOTRENDERER_H
