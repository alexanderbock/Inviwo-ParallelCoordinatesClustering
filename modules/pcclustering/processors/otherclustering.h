#ifndef IVW_OTHERCLUSTERING_H
#define IVW_OTHERCLUSTERING_H

#include <inviwo/core/processors/processor.h>

#include <modules/pcclustering/pcclusteringmoduledefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/properties/fileproperty.h>
#include <inviwo/core/properties/buttonproperty.h>
#include <inviwo/core/properties/optionproperty.h>
#include <inviwo/core/properties/stringproperty.h>
#include <inviwo/core/properties/ordinalproperty.h>

#include <modules/pcclustering/datastructures/pcpdata.h>

#include <inviwo/core/ports/imageport.h>

#include <modules/opengl/shader/shader.h>


namespace inviwo {

class IVW_MODULE_PCCLUSTERING_API PCPOtherClustering : public Processor {
public:
    const ProcessorInfo getProcessorInfo() const override;
    static const ProcessorInfo processorInfo_;
    PCPOtherClustering();
    ~PCPOtherClustering();

    void process() override;

private:
    PCPRawDataInport _inport;
    std::shared_ptr<ParallelCoordinatesPlotData> _outportData;
    PCPDataOutport _outport;


    bool _dbScanDirty = true;
    std::shared_ptr<ColoringData> _dbScanColoringData;
    ColoringDataOutport _dbscanColoringOutport;

    bool _kmeansDirty = true;
    std::shared_ptr<ColoringData> _kmeansColoringData;
    ColoringDataOutport _kmeansColoringOutport;

    IntProperty _dbscanMinClusters;
    FloatProperty _dbscanEpsilon;
    IntProperty _dbscanMinPoints;

    IntProperty _kMeansk;

    BoolProperty _normalization;

    BoolProperty _disableKmeans;
    BoolProperty _disableDBScan;

    ButtonProperty _invalidate;

    //int countElements(const ParallelCoordinatesPlotData* inData,
    //    const BinningData* data);

    //void clusterDetection(const BinningData* data);

    //void filterData(const ParallelCoordinatesPlotData* inData, 
    //    const BinningData* data, ParallelCoordinatesPlotData* outData, int nElements);

    //BinningDataInport _binInport;
    //PCPDataInport _pcpInport;
    //
    //PCPDataOutport _pcpOutport;
    //ColoringDataOutport _coloringOutport;
    //ColoredBinDataOutport _coloredBinOutport;

    //IntProperty _coloringDimension;
    //StringProperty _dimensionMaskString;
    //uint32_t _dimensionMask;

    //IntProperty _parallelismSlider;

    //ButtonProperty _invalidate;

    //Shader _countingShader;
    //Shader _clusterDetectionShader;
    //Shader _filteringShader;

    //std::shared_ptr<ParallelCoordinatesPlotData> _pcpData;
    //std::shared_ptr<ColoringData> _coloringData;
    //std::shared_ptr<ColoredBinData> _coloredBinData;

    //GLuint _nValuesCounter;
    //GLuint _memoryAccess;
    ////GLuint _nClustersBuffer;
};

}  // namespace

#endif  // IVW_OTHERCLUSTERING_H
