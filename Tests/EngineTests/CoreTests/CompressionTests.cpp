// RZCompressionTests.cpp
// Integration tests for RZCompression using TEST_F and fixtures, with container checks and C++ RZLog API

#include <gtest/gtest.h>
#include <chrono>
#include <vector>
#include <cstdint>
#include <cstring>
#include <string>

#include "Razix/Core/Compression/RZCompression.h"
#include "Razix/Core/Log/RZLog.h"
#include "Razix/Core/RZCore.h"
#include "Razix/Core/Containers/string.h"
#include "Razix/Core/Containers/string_utils.h"

namespace Razix {
    class RZCompressionFixture : public ::testing::Test {
    protected:
        void SetUp() override {
            Razix::Debug::RZLog::StartUp();
        }
        void TearDown() override {
            Razix::Debug::RZLog::Shutdown();
        }
        // Helper for test data
        static void GenerateTestData(std::vector<uint8_t>& out, size_t sizeBytes) {
            out.resize(sizeBytes);
            constexpr size_t BlockSize = 256;
            uint32_t seed = 0x12345678;
            for (size_t i = 0; i < sizeBytes; i += BlockSize) {
                seed = seed * 1664525u + 1013904223u;
                for (size_t j = 0; j < BlockSize && i + j < sizeBytes; ++j) {
                    out[i + j] = static_cast<uint8_t>((seed >> (j % 24)) & 0xFF);
                }
            }
        }
        // Helper for compression test
        void RunCompressionTest(size_t sizeBytes) {
            std::vector<uint8_t> input;
            GenerateTestData(input, sizeBytes);

            size_t inputSize = input.size();
            u32 minOut = rz_min_compressed_size(RZ_COMPRESSION_LZ4, inputSize);
            std::vector<uint8_t> compressed(minOut);

            auto t0 = std::chrono::high_resolution_clock::now();
            
            size_t compressedSize = 0;
            rz_compression_result cres = rz_compress(RZ_COMPRESSION_LZ4, input.data(), inputSize, compressed.data(), &compressedSize);
            auto t1 = std::chrono::high_resolution_clock::now();
                
            double elapsedSecs = std::chrono::duration<double>(t1 - t0).count();
            double speedMBs = (double)inputSize / (1024.0 * 1024.0) / (elapsedSecs > 0.0 ? elapsedSecs : 1e-9);
            double ratio =(double)inputSize / (double)compressedSize;
            RAZIX_CORE_TRACE("Compression: input={:.2f} MB, compressed={:.2f} MB, ratio={:.4f}, time={:.6f} s, speed={:.2f} MB/s", inputSize / (1024.0 * 1024.0), compressedSize / (1024.0 * 1024.0), ratio, elapsedSecs, speedMBs);
            RAZIX_CORE_TRACE("Compressed Size: {0} MB and minOut: {1} MB, ratio of minSize/FinalCompression size : {2}", in_Mib(compressedSize), in_Mib(minOut), (float)minOut / (float)compressedSize);
            
            ASSERT_EQ(cres, RZ_COMPRESSION_OK);
            ASSERT_GT(compressedSize, 0u);

            std::vector<uint8_t> decompressed(inputSize);
            size_t decomprSize = 0;
            
            auto d0 = std::chrono::high_resolution_clock::now();
            
            rz_compression_result dres = rz_decompress(RZ_COMPRESSION_LZ4, compressed.data(), compressedSize, decompressed.data(), inputSize, &decomprSize);
            
            auto d1 = std::chrono::high_resolution_clock::now();
            double delapsed = std::chrono::duration<double>(d1 - d0).count();
            double dspeed = (double)decomprSize / (1024.0 * 1024.0) / (delapsed > 0.0 ? delapsed : 1e-9);
            RAZIX_CORE_TRACE("Decompression: output={:.2f} MB, time={:.6f} s, speed={:.2f} MB/s", decomprSize / (1024.0 * 1024.0), delapsed, dspeed);

            ASSERT_EQ(dres, RZ_COMPRESSION_OK);
            ASSERT_EQ(decomprSize, inputSize);
            ASSERT_EQ(0, std::memcmp(input.data(), decompressed.data(), inputSize));        
        }
    };

    TEST_F(RZCompressionFixture, SmallSizeCompression) {
        RunCompressionTest(Mib(1));
    }

    TEST_F(RZCompressionFixture, MediumSizeCompression) {
        RunCompressionTest(Mib(16));
    }

    TEST_F(RZCompressionFixture, DISABLED_LargeSizeCompression) {
        RunCompressionTest(Mib(256));
    }

    TEST_F(RZCompressionFixture, DISABLED_GiantSizeCompression) {
        RunCompressionTest(Gib(1));
    }

    TEST_F(RZCompressionFixture, DISABLED_BigRZStringCompression) {
        size_t strSize = 128 * 1024 * 1024;
        Razix::RZString bigStr;
        bigStr.resize(strSize);
        for (size_t i = 0; i < strSize * 1.8; ++i) {
            bigStr += (char)('A' + (i % 26));
        }
        // Compress
        u32 minOut = rz_min_compressed_size(RZ_COMPRESSION_LZ4, bigStr.size());
        std::vector<uint8_t> compressed(minOut);
        size_t compressedSize = 0;
        auto t0 = std::chrono::high_resolution_clock::now();
        rz_compression_result cres = rz_compress(RZ_COMPRESSION_LZ4, bigStr.data(), bigStr.size(), compressed.data(), &compressedSize);
        auto t1 = std::chrono::high_resolution_clock::now();
        double elapsedSecs = std::chrono::duration<double>(t1 - t0).count();
        double ratio = (double)bigStr.size() / (double)compressedSize;
        RAZIX_CORE_TRACE("BigRZString Compression: input={} MB, compressed={} MB, ratio={:.4f}, time={:.6f} s", bigStr.size() / (1024.0 * 1024.0), compressedSize / (1024.0 * 1024.0), ratio, elapsedSecs);
        ASSERT_EQ(cres, RZ_COMPRESSION_OK);
        ASSERT_GT(compressedSize, 0u);
        // Decompress
        Razix::RZString decompressed;
        decompressed.reserve(bigStr.size());
        decompressed.resize(bigStr.size());
        size_t decomprSize = 0;
        rz_compression_result dres = rz_decompress(RZ_COMPRESSION_LZ4, compressed.data(), compressedSize, decompressed.data(), bigStr.size(), &decomprSize);
        ASSERT_EQ(dres, RZ_COMPRESSION_OK);
        ASSERT_EQ(decomprSize, bigStr.size());
        ASSERT_EQ(0, std::memcmp(bigStr.data(), decompressed.data(), bigStr.size()));
    }
}
