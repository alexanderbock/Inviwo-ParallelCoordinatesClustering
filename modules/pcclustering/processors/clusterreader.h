#ifndef IVW_CLUSTERREADER_H
#define IVW_CLUSTERREADER_H

#include <inviwo/core/processors/processor.h>

#include <modules/pcclustering/pcclusteringmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/core/properties/buttonproperty.h>

#include <modules/pcclustering/datastructures/pcpdata.h>

namespace inviwo {

class IVW_MODULE_PCCLUSTERING_API ClusterReader : public Processor {
public:

    virtual const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;
    ClusterReader();
    virtual ~ClusterReader() = default;

    virtual void process() override;

private:
    void load();

    ColoringDataOutport _coloringOutport;

    FileProperty _file;
    ButtonProperty _reload;

    bool isDeserializing_;
};

}  // namespace

#endif  // IVW_CLUSTERREADER_H
