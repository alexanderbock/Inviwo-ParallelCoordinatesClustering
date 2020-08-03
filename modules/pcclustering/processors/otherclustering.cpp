#include <modules/pcclustering/processors/otherclustering.h>

#include <modules/opengl/texture/textureutils.h>
#include <inviwo/core/util/clock.h>
#include <bitset>
#include <set>

//#include <modules/pcclustering/ext/dbscan/dbscan.h>
#include <modules/pcclustering/ext/dbscan2/clustering.cpp>
#include <modules/pcclustering/ext/kmeans/src/kmeans.h>

namespace inviwo {

const ProcessorInfo PCPOtherClustering::processorInfo_{
    "pers.bock.PCPOtherClustering",  // Class identifier
    "Other Clustering Methods",            // Display name
    "Data Filtering",               // Category
    CodeState::Experimental,          // Code state
    Tags::CPU,                  // Tags
};

const ProcessorInfo PCPOtherClustering::getProcessorInfo() const {
    return processorInfo_;
}

PCPOtherClustering::PCPOtherClustering()
    : Processor()
    , _inport("in_data")
    , _outport("out_data")
    , _dbscanColoringOutport("out_dbscan_coloring")
    , _kmeansColoringOutport("out_kmeans_coloring")
    , _dbscanMinClusters("dbscan_minclusters", "DBScan Minimum Clusters", 2, 0, 30)
    , _dbscanEpsilon("dbscan_epsilon", "DBScan Epsilon", 0.2f, 0.f, 1.f)\
    , _dbscanMinPoints("dbscan_minpoints", "DBScan Min Points", 10, 0, 10000)
    , _kMeansk("kmeans_k", "K-Means K", 2, 0, 30)
    , _normalization("normalization", "Normalization", true)
    , _invalidate("_invalidate", "Invalidate")
    , _disableKmeans("_disableKmeans", "Disable KMeans", false)
    , _disableDBScan("_disableDBScan", "Disable DBScan", true)
{
    addPort(_inport);

    addPort(_outport);
    addPort(_dbscanColoringOutport);
    addPort(_kmeansColoringOutport);

    _dbscanMinClusters.onChange([this]() { _dbScanDirty = true; });
    addProperty(_dbscanMinClusters);
    _dbscanEpsilon.onChange([this]() { _dbScanDirty = true; });
    addProperty(_dbscanEpsilon);
    _dbscanMinPoints.onChange([this]() { _dbScanDirty = true; });
    addProperty(_dbscanMinPoints);

    _kMeansk.onChange([this]() { _kmeansDirty = true; });
    addProperty(_kMeansk);

    _invalidate.onChange([this]() {
        _dbScanDirty = true;
        _kmeansDirty = true;
    });
    addProperty(_normalization);

    addProperty(_disableKmeans);
    addProperty(_disableDBScan);

    addProperty(_invalidate);
    _invalidate.onChange([this]() {
        _dbScanDirty = true;
        _kmeansDirty = true;
        invalidate(InvalidationLevel::InvalidOutput);
    });

    _outportData = std::make_shared<ParallelCoordinatesPlotData>();
    glGenBuffers(1, &(_outportData->ssboData));

    _dbScanColoringData = std::make_shared<ColoringData>();
    glGenBuffers(1, &_dbScanColoringData->ssboColor);
    _kmeansColoringData = std::make_shared<ColoringData>();
    glGenBuffers(1, &_kmeansColoringData->ssboColor);
}

PCPOtherClustering::~PCPOtherClustering() {}

#pragma optimize ( "", off)

void PCPOtherClustering::process() {
    std::shared_ptr<const ParallelCoordinatesPlotRawData> inData = _inport.getData();
    const int nValues = inData->data.size();
    const int nDimensions = inData->minMax.size();
    const int nItems = nValues / nDimensions;

    _outportData->nValues = nValues;
    _outportData->nDimensions = nDimensions;
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _outportData->ssboData);
    glBufferData(
        GL_SHADER_STORAGE_BUFFER,
        inData->data.size() * sizeof(float),
        inData->data.data(),
        GL_STATIC_DRAW
    );
    _outport.setData(_outportData);

    if (_dbScanDirty && !_disableDBScan) {
#if 0
        // Run DBScan
        std::vector<dbscan::Point> points;
        points.reserve(nItems);

        // Fill points
        for (int i = 0; i < nItems; ++i) {
            dbscan::Point p;
            p.clusterID = UNCLASSIFIED;
            for (int j = 0; j < nDimensions; ++j) {
                float v = inData->data[i * nDimensions + j];
                if (_normalization) {
                    v = (v - inData->minMax[j].first) / (inData->minMax[j].second - inData->minMax[j].first);
                }
                p.data.push_back(v);
            }
            points.push_back(p);
        }

        dbscan::DBSCAN dbscan(_dbscanMinClusters, _dbscanEpsilon, std::move(points));
        dbscan.run();

        // Count clusters
        std::set<int> clusters;
        for (const dbscan::Point& p : dbscan.m_points) {
            clusters.insert(p.clusterID);
        }
        const int nClusters = clusters.size();
        LogInfo("Number of clusters: " << clusters.size());


        _dbScanColoringData->nValues = nValues;
        _dbScanColoringData->hasData = true;
        _dbScanColoringData->nClusters = nClusters;

        std::vector<int> clusterData;
        clusterData.reserve(nValues + 1);
        clusterData.push_back(nClusters);
        for (const dbscan::Point& p : dbscan.m_points) {
            clusterData.push_back(p.clusterID);
        }
#else
        std::vector<dbscan2::Point> points;
        points.reserve(nItems);

        // Fill points
        for (int i = 0; i < nItems; ++i) {
            dbscan2::Point p;
            p.ptsCnt = 0;
            p.cluster = dbscan2::NOT_CLASSIFIED;
            for (int j = 0; j < nDimensions; ++j) {
                float v = inData->data[i * nDimensions + j];
                if (_normalization) {
                    v = (v - inData->minMax[j].first) / (inData->minMax[j].second - inData->minMax[j].first);
                }
                p.data.push_back(v);
            }
            points.push_back(p);
        }

        dbscan2::DBCAN dbScan(_dbscanMinClusters, _dbscanEpsilon, _dbscanMinPoints, points);
        dbScan.run();

        std::vector<std::vector<int>> clusters = dbScan.getCluster();
        std::vector<int> clusterData(nItems + 1);
        clusterData[0] = clusters.size();

        // Invert the association of the array
        for (int i = 0; i < clusters.size(); ++i) {
            const std::vector<int> elems = clusters[i];
            for (int j : elems) {
                clusterData[j + 1] = i; // +1 for the extra cluster size in the beginning
            }
        }

#endif

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _dbScanColoringData->ssboColor);
        glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            clusterData.size() * sizeof(int),
            clusterData.data(),
            GL_STATIC_DRAW
        );
        _dbscanColoringOutport.setData(_dbScanColoringData);

        _dbScanDirty = false;
    }

    if (_kmeansDirty && !_disableKmeans) {
        // Run K-Means
        KMeans kmeans(_kMeansk);
        // Fill points
        std::vector<Point> points;
        points.reserve(nItems);
        for (int i = 0; i < nItems; ++i) {
            std::vector<double> values;
            for (int j = 0; j < nDimensions; ++j) {
                float v = inData->data[i * nDimensions + j];
                if (_normalization) {
                    v = (v - inData->minMax[j].first) / (inData->minMax[j].second - inData->minMax[j].first);
                }
                values.push_back(v);
            }

            Point p(values);
            points.push_back(p);
        }

        kmeans.init(points);
        kmeans.run();



        _kmeansColoringData->nClusters = _kMeansk;
        _kmeansColoringData->hasData = true;
        _kmeansColoringData->nValues = nValues;

        std::vector<int> clusterData;
        clusterData.reserve(nValues + 1);
        clusterData.push_back(_kMeansk);
        for (const Point& p : kmeans.getPoints()) {
            clusterData.push_back(p.cluster_);
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, _kmeansColoringData->ssboColor);
        glBufferData(
            GL_SHADER_STORAGE_BUFFER,
            clusterData.size() * sizeof(int),
            clusterData.data(),
            GL_STATIC_DRAW
        );
        _kmeansColoringOutport.setData(_kmeansColoringData);

        _kmeansDirty = false;


        std::ofstream f("export_kmeans.txt");
        for (int i = 1; i < clusterData.size(); ++i) {
            f << clusterData[i] << '\n';
        }
    }
}

}  // namespace

