#ifndef IVW_DENSITYMAPGENERATOR_H
#define IVW_DENSITYMAPGENERATOR_H

#include <inviwo/core/processors/processor.h>

#include <modules/pcclustering/pcclusteringmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/core/properties/buttonproperty.h>

#include <modules/pcclustering/datastructures/pcpdata.h>

#include <inviwo/core/ports/imageport.h>

#include <modules/opengl/shader/shader.h>


namespace inviwo {

class IVW_MODULE_PCCLUSTERING_API DensityMapGenerator : public Processor {
public:
    const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;
    DensityMapGenerator();
    ~DensityMapGenerator();

    void process() override;

private:
    void invalidateBuffers();

    PCPDataInport _inport;
    BinningDataOutport _outport;

    IntProperty _nBins;

    ButtonProperty _invalidate;

    Shader _densityMapGeneratorShader;
    Shader _densityMapCounterShader;

    std::shared_ptr<BinningData> _binningData;

};

}  // namespace

#endif  // IVW_DENSITYMAPGENERATOR_H
