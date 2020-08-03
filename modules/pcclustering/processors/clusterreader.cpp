#include <modules/pcclustering/processors/clusterreader.h>

#include <inviwo/core/util/filesystem.h>
#include <set>

#pragma optimize ("", off)

namespace inviwo {

const ProcessorInfo ClusterReader::processorInfo_{
    "pers.bock.ClusterReader",  // Class identifier
    "Cluster Data Reader",            // Display name
    "Data Input",               // Category
    CodeState::Experimental,          // Code state
    Tags::CPU,                  // Tags
};

const ProcessorInfo ClusterReader::getProcessorInfo() const {
    return processorInfo_;
}

ClusterReader::ClusterReader()
    : Processor()
    , _coloringOutport("data")
    , _file("filename", "File")
    , _reload("reload", "Reload data")
{
    auto l = [this]() { load(); };
    _file.onChange(l);
    _reload.onChange(l);

    addPort(_coloringOutport);

    addProperty(_file);
    addProperty(_reload);
}

void ClusterReader::load() {
    std::string fileName = _file;
    
    if (!filesystem::fileExists(fileName)) {
        LogError("Error opening file '" << fileName << "'");
        return;
    }

    std::shared_ptr<ColoringData> data = std::make_shared<ColoringData>();
    glGenBuffers(1, &data->ssboColor);

    std::ifstream file(fileName);
    std::vector<int> clusters;
    clusters.push_back(-1);
    std::set<int> set;
    int v;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        if (file.bad()) {
            LogError("Error loading file");
            break;
        }
        std::stringstream s(line);
        s >> v;
        clusters.push_back(v);
        set.insert(v);
    }

    LogInfo("Loaded number of clusters: " << set.size());
    //while (file >> v) {
    //    clusters.push_back(v);
    //    set.insert(v);
    //}
    data->hasData = true;
    data->nValues = clusters.size();
    data->nClusters = set.size();
    clusters[0] = data->nClusters;

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, data->ssboColor);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        clusters.size() * sizeof(int),
        clusters.data(),
        GL_STATIC_DRAW
    );
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    _coloringOutport.setData(data);

    //ParallelCoordinatesPlotRawData* data = new ParallelCoordinatesPlotRawData;
    //LogInfo("Dataset information");

    //int8_t dimensions;
    //file.read(reinterpret_cast<char*>(&dimensions), sizeof(int8_t));
    //LogInfo("Dimensions: " << int(dimensions));

    //std::vector<float> minMaxValues(dimensions * 2);
    //file.read(reinterpret_cast<char*>(minMaxValues.data()), dimensions * 2 * sizeof(float));

    //LogInfo("Min/Max Values:");
    //for (int i = 0; i < dimensions; ++i) {
    //    data->minMax.push_back(std::make_pair(minMaxValues[i * 2], minMaxValues[i * 2 + 1]));
    //    LogInfo(i << ": " << data->minMax[i].first << " / " << data->minMax[i].second);
    //}

    //int64_t nValues;
    //file.read(reinterpret_cast<char*>(&nValues), sizeof(int64_t));
    //LogInfo("Number of total values: " << nValues);
    //LogInfo("Number of data values: " << nValues / dimensions);
    //data->data.resize(nValues);
    //file.read(reinterpret_cast<char*>(data->data.data()), nValues * sizeof(float));

    //for (int i = 0; i < nValues; ++i) {
    //    const int dim = i % dimensions;
    //    const float minValue = data->minMax[dim].first;
    //    const float maxValue = data->minMax[dim].second;

    //    if (minValue == maxValue)
    //        data->data[i] = 0.5f;
    //    else {
    //        float& v = data->data[i];
    //        v = (v - minValue) / (maxValue - minValue);
    //        v = (v - 0.5f) * 2.f;
    //    }
    //}

    //_coloringOutport.setData(data);
}

void ClusterReader::process() {}

}  // namespace

