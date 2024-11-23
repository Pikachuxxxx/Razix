#include <iostream>

#include "exporter/MeshExporter.h"
#include "importer/MeshImporter.h"

int main(int argc, char* argv[])
{
    // TODO: Use command line args

    // Results of the mesh import
    Razix::Tool::AssetPacker::MeshImportResult import_result;
    // Mesh import options
    Razix::Tool::AssetPacker::MeshImportOptions import_options{};

    // Importer
    Razix::Tool::AssetPacker::MeshImporter* importer = new Razix::Tool::AssetPacker::MeshImporter;
    bool                                    result   = importer->importMesh("X:/GameEngines/Razix/Sandbox/Assets/Models/Sponza/Sponza.gltf", import_result, import_options);

    if (!result) {
        std::cout << "[ERROR!] Mesh Importing Failed" << std::endl;
        return EXIT_FAILURE;
    }

    delete importer;

    // Export Options
    Razix::Tool::AssetPacker::MeshExportOptions export_options{};
    export_options.assetsOutputDirectory = "X:/GameEngines/Razix/Sandbox/Assets/";
    // Exporter
    Razix::Tool::AssetPacker::MeshExporter exporter;
    result = exporter.exportMesh(import_result, export_options);

    if (!result) {
        std::cout << "[ERROR!] Mesh Export Failed" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}