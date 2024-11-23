#pragma once

// Based on https://github.com/diharaw/asset-core

#include "common/intermediate_types.h"

namespace Razix {
    namespace Tool {
        namespace AssetPacker {

            struct MeshExportOptions
            {
                std::string assetsOutputDirectory;
                bool        useCompression = true;
                bool        outputMetadata = false;
            };

            class MeshExporter
            {
            public:
                MeshExporter()  = default;
                ~MeshExporter() = default;

                bool exportMesh(const MeshImportResult& import_result, const MeshExportOptions& options);
                bool exportMaterial() {}
            };

        }    // namespace AssetPacker
    }        // namespace Tool
}    // namespace Razix