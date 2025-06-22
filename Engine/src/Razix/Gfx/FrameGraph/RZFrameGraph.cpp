// clang-format off
#include "rzxpch.h"
// clang-format on
#include "RZFrameGraph.h"

#include "Razix/Core/OS/RZFileSystem.h"
#include "Razix/Core/OS/RZVirtualFileSystem.h"
#include "Razix/Core/RZEngine.h"

#include "Razix/Gfx/RZShaderLibrary.h"

#include "Razix/Gfx/FrameGraph/RZFrameGraphPass.h"

#include "Razix/Gfx/Resources/RZFrameGraphBuffer.h"
#include "Razix/Gfx/Resources/RZFrameGraphSampler.h"
#include "Razix/Gfx/Resources/RZFrameGraphTexture.h"

#include "Razix/Scene/RZScene.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;    // use other lib that said it was faster than this on github

namespace Razix {
    namespace Gfx {

        // TODO: Too much Code duplication clean this shit!

        //-----------------------------------------------------------------------------------
        // Frame Graph Class
        //-----------------------------------------------------------------------------------

        // First frame test
        bool RZFrameGraph::m_IsFirstFrame = true;

        bool RZFrameGraph::parse(const std::string& path)
        {
            std::string physicalPath;
            if (!RZVirtualFileSystem::Get().resolvePhysicalPath(path, physicalPath))
                return false;

            auto jsonStrData = RZFileSystem::ReadTextFile(physicalPath);

            json data = json::parse(jsonStrData);

            // Load imported resources
            auto& imports = data["imports"];
            for (auto& import_res: imports) {
                auto& type = import_res["type"];
                RAZIX_ASSERT(!type.empty(), "[Frame Graph] Missing import resource type!");

                RZFrameGraphResource resource{-1};

                auto& resourceName = import_res["name"];

                if (std::string(type) == "Texture") {
                    RZTextureDesc desc{};
                    desc.name = std::string(resourceName);

                    auto& format = import_res["format"];
                    RAZIX_ASSERT(!format.empty(), "[Frame Graph] Missing Texture Format!");
                    desc.format = RZTextureDesc::StringToFormat(format);

                    auto& texture_type = import_res["texture_type"];
                    RAZIX_ASSERT(!texture_type.empty(), "[Frame Graph] Missing Texture Type!");
                    desc.type  = RZTextureDesc::StringToType(texture_type);
                    auto width = import_res["width"];
                    if (!width.empty())
                        desc.width = width.get<int>();
                    auto height = import_res["height"];
                    if (!height.empty())
                        desc.height = height.get<int>();
                    auto depth = import_res["depth"];
                    if (!depth.empty())
                        desc.depth = depth.get<int>();
                    auto layers = import_res["layers"];
                    if (!layers.empty())
                        desc.layers = layers.get<int>();

                    auto& enableMips = import_res["enable_mips"];
                    if (!enableMips.empty())
                        desc.enableMips = enableMips.get<bool>();

                    auto& isHDR = import_res["hdr"];
                    if (!isHDR.empty())
                        desc.dataSize = sizeof(float);

                    // Create the resource
                    desc.filePath      = import_res["file_path"];
                    auto textureHandle = RZResourceManager::Get().createTexture(desc);
                    resource           = import <RZFrameGraphTexture>(desc.name, CAST_TO_FG_TEX_DESC desc, {textureHandle});

                } else if (std::string(type) == "Buffer") {
                    RZBufferDesc desc{};
                    desc.name = std::string(resourceName);

                    desc.size  = import_res["size"].get<int>();
                    auto usage = import_res["usage"];
                    if (!usage.empty())
                        desc.usage = StringToBufferUsage(usage);
                    // TODO: Load some data here, How?

                    // Create the buffer resource
                    auto bufferHandle = RZResourceManager::Get().createUniformBuffer(desc);
                    resource          = import <RZFrameGraphBuffer>(desc.name, CAST_TO_FG_BUF_DESC desc, {bufferHandle});
                } else if (std::string(type) == "Sampler") {
                    RZSamplerDesc desc{};
                    desc.name = std::string(resourceName);

                    auto& wrapping = import_res["wrapping"];
                    if (!wrapping.empty())
                        desc.wrapping = StringToWrapping(wrapping);

                    auto& filtering = import_res["filtering"];
                    if (!filtering.empty()) {
                        desc.filtering.minFilter = StringToFilteringMode(filtering["min"]);
                        desc.filtering.minFilter = StringToFilteringMode(filtering["mag"]);
                    }

                    // Create the Sampler resource
                    auto samplerHandle = RZResourceManager::Get().createSampler(desc);
                    resource           = import <RZFrameGraphSampler>(desc.name, CAST_TO_FG_SAMP_DESC desc, {samplerHandle});
                }
                // we skip else cause imported resources can't be a reference

                // Add each of them into the blackboard
                m_Blackboard.add(resourceName, resource);
            }

            auto& passes = data["passes"];
            for (auto& pass: passes) {
                auto& render_pass = pass["render_pass"];
                // Load this render pass from file
                RZPassNode& passNode = parsePass("//RazixFG/Passes/" + std::string(render_pass) + ".json");

                // Use the builder to build input/output resources
                RZPassResourceBuilder builder(*this, passNode);

                /**
                     * As mentioned in design docs, we will have 3 types
                     * 1. Texture -> RZFrameGraphTexture
                     * 2. Buffer -> RZFrameGraphBuffer
                     * 3. Reference -> points to existing Texture/Buffer
                     * 
                     * So identify and create them, as for Reference we need blackboard's extended functionality
                     */

                // Load the Input Resources
                auto& inputs = pass["inputs"];
                for (auto& input: inputs) {
                    auto& type = input["type"];
                    RAZIX_ASSERT(!type.empty(), "[Frame Graph] Missing input resource type!");

                    // Binding Info
                    DescriptorBindingInfo bindingInfo{};
                    bool                  hasBindingInfo = false;
                    auto&                 binding_info   = input["binding_info"];
                    if (!binding_info.empty()) {
                        hasBindingInfo = true;

                        auto& type = binding_info["type"];
                        if (!type.empty())
                            bindingInfo.type = std::string(type) == "ImageSamplerCombined" ? DescriptorType::kImageSamplerCombined : DescriptorType::kUniformBuffer;

                        auto& stage = binding_info["stage"];
                        if (!stage.empty())
                            bindingInfo.stage = StringToShaderStage(stage);

                        auto& location = binding_info["location"];
                        if (!location.empty()) {
                            auto& set                = location["set"];
                            bindingInfo.location.set = set.get<int>();

                            auto& binding                = location["binding"];
                            bindingInfo.location.binding = binding.get<int>();
                        }

                        auto& count = binding_info["count"];
                        if (!count.empty())
                            bindingInfo.count = count.get<int>();
                    }

                    RZFrameGraphResource resource{-1};

                    auto& resourceName = input["name"];

                    if (std::string(type) == "Texture") {
                        RZTextureDesc desc{};
                        desc.name = std::string(resourceName);

                        auto& format = input["format"];
                        RAZIX_ASSERT(!format.empty(), "[Frame Graph] Missing Texture Format!");
                        desc.format = RZTextureDesc::StringToFormat(format);

                        auto& texture_type = input["texture_type"];
                        RAZIX_ASSERT(!texture_type.empty(), "[Frame Graph] Missing Texture Type!");
                        desc.type = RZTextureDesc::StringToType(texture_type);

                        auto width = input["width"];
                        if (!width.empty())
                            desc.width = width.get<int>();
                        auto height = input["height"];
                        if (!height.empty())
                            desc.height = height.get<int>();
                        auto depth = input["depth"];
                        if (!depth.empty())
                            desc.depth = depth.get<int>();
                        auto layers = input["layers"];
                        if (!layers.empty())
                            desc.layers = layers.get<int>();

                        auto& enableMips = input["enable_mips"];
                        if (!enableMips.empty())
                            desc.enableMips = enableMips.get<bool>();

                        auto& isHDR = input["hdr"];
                        if (!isHDR.empty())
                            desc.dataSize = sizeof(float);

                        // Create the resource
                        resource = builder.create<RZFrameGraphTexture>(desc.name, CAST_TO_FG_TEX_DESC desc);
                        // Mark the input resource as read for the current pass and pass the attachment info
                        builder.read(resource, hasBindingInfo ? EncodeDescriptorBindingInfo(bindingInfo) : kFlagsNone);

                    } else if (std::string(type) == "Buffer") {
                        RZBufferDesc desc{};
                        desc.name = std::string(resourceName);

                        desc.size = input["size"].get<int>();

                        auto usage = input["usage"];
                        if (!usage.empty())
                            desc.usage = StringToBufferUsage(usage);

                        // Create the buffer resource
                        resource = builder.create<RZFrameGraphBuffer>(desc.name, CAST_TO_FG_BUF_DESC desc);
                        // Mark the input resource as read for the current pass and pass the attachment info
                        builder.read(resource, hasBindingInfo ? EncodeDescriptorBindingInfo(bindingInfo) : kFlagsNone);

                    } else if (std::string(type) == "Sampler") {
                        RZSamplerDesc desc{};
                        desc.name = std::string(resourceName);

                        auto& wrapping = input["wrapping"];
                        if (!wrapping.empty())
                            desc.wrapping = StringToWrapping(wrapping);

                        auto& filtering = input["filtering"];
                        if (!filtering.empty()) {
                            desc.filtering.minFilter = StringToFilteringMode(filtering["min"]);
                            desc.filtering.minFilter = StringToFilteringMode(filtering["mag"]);
                        }

                        // Create the Sampler resource
                        auto samplerHandle = RZResourceManager::Get().createSampler(desc);
                        resource           = import <RZFrameGraphSampler>(desc.name, CAST_TO_FG_SAMP_DESC desc, {samplerHandle});
                        // Mark the input resource as read for the current pass and pass the attachment info
                        builder.read(resource, hasBindingInfo ? EncodeDescriptorBindingInfo(bindingInfo) : kFlagsNone);

                    } else {
                        // This is a reference which means some pass node has this, in code driven we search blackboard which stores a struct of resources it uses, without actually touching RZPassNode
                        // But here we have access to RZPassNode and data driven can't use types for access hence we search each RZPassNode for resource using the string ID
                        // We can instead have a map in blackboard for this which will cache and speed thing up a little instead of having to search through each pass node without a string identfier for resourceID

                        resource = m_Blackboard.getID(resourceName);
                        builder.read(resource, hasBindingInfo ? EncodeDescriptorBindingInfo(bindingInfo) : kFlagsNone);
                    }

                    // Add each of them into the blackboard
                    m_Blackboard.add(resourceName, resource);
                }

                // Load the Output Resources
                auto& outputs = pass["outputs"];
                for (auto& output: outputs) {
                    auto& type = output["type"];
                    RAZIX_ASSERT(!type.empty(), "[Frame Graph] Missing output resource type!");

                    auto& attachment_info = output["attachment_info"];

                    RenderTargetAttachmentInfo attachInfo{};
                    bool                       hasAttachmentInfo = false;
                    if (!attachment_info.empty()) {
                        hasAttachmentInfo = true;
                        auto& clear       = attachment_info["clear"];
                        if (!clear.empty())
                            attachInfo.clear = clear.get<bool>();

                        auto& clear_color = attachment_info["clear_color"];
                        if (!clear_color.empty())
                            attachInfo.clearColor = StringToColorPreset[clear_color];

                        auto& binding_idx = attachment_info["binding_idx"];
                        if (!binding_idx.empty())
                            attachInfo.bindingIdx = binding_idx.get<int>();

                        auto& mip = attachment_info["mip"];
                        if (!mip.empty())
                            attachInfo.mip = mip.get<int>();

                        auto& layer = attachment_info["layer"];
                        if (!layer.empty())
                            attachInfo.layer = layer.get<int>();
                    }

                    RZFrameGraphResource resource{-1};

                    auto& resourceName = output["name"];

                    if (std::string(type) == "Texture") {
                        RZTextureDesc desc{};
                        desc.name = std::string(resourceName);

                        auto& format = output["format"];
                        RAZIX_ASSERT(!format.empty(), "[Frame Graph] Missing Texture Format!");
                        desc.format = RZTextureDesc::StringToFormat(format);

                        auto& texture_type = output["texture_type"];
                        RAZIX_ASSERT(!texture_type.empty(), "[Frame Graph] Missing Texture Type!");
                        desc.type = RZTextureDesc::StringToType(texture_type);

                        auto width = output["width"];
                        if (!width.empty())
                            desc.width = width.get<int>();
                        auto height = output["height"];
                        if (!height.empty())
                            desc.height = height.get<int>();
                        auto depth = output["depth"];
                        if (!depth.empty())
                            desc.depth = depth.get<int>();
                        auto layers = output["layers"];
                        if (!layers.empty())
                            desc.layers = layers.get<int>();

                        auto& enableMips = output["enable_mips"];
                        if (!enableMips.empty())
                            desc.enableMips = enableMips.get<bool>();

                        auto& isHDR = output["hdr"];
                        if (!isHDR.empty())
                            desc.dataSize = sizeof(float);

                        // TODO: Support rest of the RZTextureDesc members

                        // Create the resource
                        resource = builder.create<RZFrameGraphTexture>(desc.name, CAST_TO_FG_TEX_DESC desc);
                        // Mark the input resource as read for the current pass and pass the attachment info
                        resource = builder.write(resource, hasAttachmentInfo ? EncodeAttachmentInfo(attachInfo) : kFlagsNone);

                    } else if (std::string(type) == "Buffer") {
                        RZBufferDesc desc{};
                        desc.name = std::string(resourceName);

                        desc.size = output["size"].get<int>();

                        auto usage = output["usage"];
                        if (!usage.empty())
                            desc.usage = StringToBufferUsage(usage);

                        // Create the buffer resource
                        resource = builder.create<RZFrameGraphBuffer>(desc.name, CAST_TO_FG_BUF_DESC desc);
                        // Mark the input resource as read for the current pass and pass the attachment info
                        resource = builder.write(resource, hasAttachmentInfo ? EncodeAttachmentInfo(attachInfo) : kFlagsNone);

                    } else {
                        resource = m_Blackboard.getID(resourceName);
                        resource = builder.write(resource, hasAttachmentInfo ? EncodeAttachmentInfo(attachInfo) : kFlagsNone);
                    }

                    // Add each of them into the blackboard
                    m_Blackboard.add(resourceName, resource);
                }
            }

            // Final Output RT name (pass this to FinalCompositionPass via Blackboard)
            auto finalOutput = data["final_output"];
            m_Blackboard.setFinalOutputName(finalOutput.empty() ? "SceneHDR" : finalOutput);

            return true;
        }

        RZPassNode& RZFrameGraph::parsePass(const std::string& passPath)
        {
            std::string physicalPath;
            RAZIX_ASSERT(RZVirtualFileSystem::Get().resolvePhysicalPath(passPath, physicalPath), "Invalid Pass, please check again!");

            auto jsonStrData = RZFileSystem::ReadTextFile(physicalPath);

            json data = json::parse(jsonStrData);

            // Get the pass name
            auto&       passName    = data["name"];
            std::string passNameStr = "DefaultPass";
            if (!passName.empty())
                passNameStr = passName.template get<std::string>();

            RAZIX_CORE_TRACE("pass name : {0}", passName);

            // parse the shader and load into/from Shader Library
            // TODO: Support loading user land shaders and re-verification of Builtin.Shaders
            // Since as of now we only deal with Built-in passes and shaders we can go ahead fine
            auto& shaderFileName = data["shader"];
            auto  shader         = Gfx::RZShaderLibrary::Get().getBuiltInShader(std::string(shaderFileName));

            RZPipelineDesc pipelineDesc{};
            pipelineDesc.name = std::string(passName) + ".Pipeline";

            // Set the shader to the pipeline
            if (!shaderFileName.empty()) {
                pipelineDesc.shader = shader;
            } else
                RAZIX_ASSERT(false, "[Frame Graph] No shader in pass description!");

            // parse the pipeline info
            auto& pipelineInfo = data["pipeline_info"];
            if (!pipelineInfo.empty()) {
                auto& depth = pipelineInfo["depth"];
                if (!depth.empty()) {
                    auto& depthWrite     = depth["write"];
                    auto& depthTest      = depth["test"];
                    auto& depthoperation = depth["op"];
                    auto& depthBias      = depth["bias"];

                    if (!depthTest.empty())
                        pipelineDesc.depthTestEnabled = depthTest.get<bool>();
                    if (!depthWrite.empty())
                        pipelineDesc.depthWriteEnabled = depthWrite.get<bool>();
                    if (!depthBias.empty())
                        pipelineDesc.depthBiasEnabled = depthBias.get<bool>();

                    if (!depthoperation.empty())
                        pipelineDesc.depthOp = StringToCompareOp(depthoperation);
                }
                auto& cullMode = pipelineInfo["cull_mode"];
                if (!cullMode.empty())
                    pipelineDesc.cullMode = StringToCullMode(cullMode);

                auto& polygonMode = pipelineInfo["polygon_mode"];
                if (!polygonMode.empty())
                    pipelineDesc.polygonMode = StringToPolygonMode(polygonMode);

                auto& drawType = pipelineInfo["draw_type"];
                if (!drawType.empty())
                    pipelineDesc.drawType = StringToDrawType(drawType);

                auto& depthFormat = pipelineInfo["depth_format"];
                if (!depthFormat.empty())
                    pipelineDesc.depthFormat = StringToTextureFormat(depthFormat);

                auto& colorFormats = pipelineInfo["color_formats"];
                for (auto& format: colorFormats) {
                    pipelineDesc.colorAttachmentFormats.push_back(StringToTextureFormat(format));
                }

                auto& colorBlendInfo = pipelineInfo["color_blend"];
                if (!colorBlendInfo.empty()) {
                    auto& src = colorBlendInfo["src"];
                    auto& dst = colorBlendInfo["dst"];
                    auto& op  = colorBlendInfo["op"];

                    if (!dst.empty())
                        pipelineDesc.colorDst = StringToBlendFactor(dst);
                    if (!src.empty())
                        pipelineDesc.colorSrc = StringToBlendFactor(src);
                    if (!op.empty())
                        pipelineDesc.colorOp = StringToBlendOp(op);
                }

                auto& alphaBlendInfo = pipelineInfo["alpha_blend"];
                if (!alphaBlendInfo.empty()) {
                    auto& src = alphaBlendInfo["src"];
                    auto& dst = alphaBlendInfo["dst"];
                    auto& op  = alphaBlendInfo["op"];

                    if (!dst.empty())
                        pipelineDesc.alphaDst = StringToBlendFactor(dst);
                    if (!src.empty())
                        pipelineDesc.alphaSrc = StringToBlendFactor(src);
                    if (!op.empty())
                        pipelineDesc.alphaOp = StringToBlendOp(op);
                }

                auto& transparency = pipelineInfo["transparency"];
                if (!transparency.empty())
                    pipelineDesc.transparencyEnabled = transparency.get<bool>();
            }

            // Create the pipeline object
            RZPipelineHandle pipeline = RZResourceManager::Get().createPipeline(pipelineDesc);

            // TODO: support parsing enableFrameData and enableBindless from JSON file

            SceneDrawGeometryMode geomMode{};
            // parse the scene params
            auto& geometry = data["geometry_mode"];
            if (!geometry.empty())
                geomMode = SceneGeometryModeStringMap[geometry];

            auto& renderInfo = data["rendering_info"];
            RAZIX_ASSERT(!renderInfo.empty(), "[Frame Graph] Missing Rendering info in pass description!");
            Resolution resolution = StringToResolutionsMap[renderInfo["resolution"]];
            // TODO: Remove this from JSON files as well, reminder!
            //                bool       resize     = renderInfo["resize"].get<bool>();
            auto&  extents = renderInfo["extents"];
            float2 extent  = float2(0.0f);
            if (!extents.empty()) {
                extent.x = extents["x"].get<float>();
                extent.y = extents["y"].get<float>();
            }
            auto& layersCount = renderInfo["layers"];
            u32   layers      = 1;
            if (!layersCount.empty())
                layers = layersCount.get<int>();

            // First create the FrameGraphPass (Data) and create a pass node
            // Now that the checks are done, let's create the pass and PassNode
            FrameGraphDataPassDesc desc = {};
            desc.shader                 = shader;
            desc.pipeline               = pipeline;
            desc.geometryMode           = geomMode;
            desc.resolution             = resolution;
            desc.extent                 = extent;
            desc.layers                 = layers;
            auto* pass                  = new RZFrameGraphDataPass(desc);
            // Create the PassNode in the graph
            RZPassNode& passNode = createPassNodeRef(std::string_view(passNameStr), std::unique_ptr<RZFrameGraphDataPass>(pass));
            // Mark as data driven
            passNode.m_IsDataDriven = true;
            auto isStandAlonePass   = data["is_standalone"];
            if (!isStandAlonePass.empty())
                passNode.m_IsStandAlone = isStandAlonePass;

            return passNode;
        }

        void RZFrameGraph::compile()
        {
            m_CompiledPassIndices.clear();
            m_CompiledResourceIndices.clear();

            // Set the read and write passes
            // Build ref counts and producer links
            for (auto& pass: m_PassNodes) {
                pass.m_RefCount = static_cast<u32>(pass.m_Writes.size());
                for (auto& [id, flags]: pass.m_Reads) {
                    auto& consumed = m_ResourceNodes[id];
                    consumed.m_RefCount++;
                }
                for (auto& [id, flags]: pass.m_Writes) {
                    auto& written      = m_ResourceNodes[id];
                    written.m_Producer = &pass;
                }
            }

            //  Cull unused resources and their producer passes
            std::stack<RZResourceNode*> unreferencedResources;
            for (auto& node: m_ResourceNodes)
                if (node.m_RefCount == 0)
                    unreferencedResources.push(&node);

            while (!unreferencedResources.empty()) {
                auto* unreferencedResource = unreferencedResources.top();
                unreferencedResources.pop();
                RZPassNode* producer{unreferencedResource->m_Producer};

                if (producer == nullptr || producer->isStandAlone())
                    continue;

                /**
                  * If a resource is unreferenced (i.e., no passes read from it), then we check its producer pass.
                  * If that pass is not standalone (like a presentation pass), and all its output resources are dead,
                  * then that pass itself becomes unreferenced and can be culled.
                  *
                  * When we cull a pass, we must also decrement the ref count of all the resources it reads.
                  * This may trigger recursive culling of earlier resources and passes in the graph.
                  */

                // Ensure this pass was still considered "alive" when we reached here
                RAZIX_CORE_ASSERT(producer->m_RefCount >= 1, "The producer of the resource being culled must have at least one reference remaining, this indicates a bug with ref counting");

                // Decrement the reference count of the producer pass.
                // If this was the last live output of the pass, then the pass is no longer used by anyone.
                if (--producer->m_RefCount == 0) {
                    // This pass has become unreferenced and can now be culled.
                    // As a result, we must update the resources it read as well,
                    // because those inputs are no longer needed if this pass is gone.

                    for (auto& [readResID, _]: producer->m_Reads) {
                        auto& readNode = m_ResourceNodes[readResID];

                        // Decrement the reference count of the input resource.
                        // If this was the last pass reading from it, then the resource is now dead too.
                        if (--readNode.m_RefCount == 0) {
                            // Add this resource to the stack of unreferenced resources to be processed.
                            // This triggers recursive culling of its producer pass if needed.
                            unreferencedResources.push(&readNode);
                        }
                    }
                }
            }

            // Cache compiled resource + pass indices
            for (u32 resID = 0; resID < m_ResourceNodes.size(); ++resID) {
                if (m_ResourceNodes[resID].m_RefCount > 0) {
                    m_CompiledResourceIndices.push_back(resID);

                    // Cache compiled resource nodes entries uniquely
                    const RZResourceEntry& entry   = getResourceEntry(resID);
                    u32                    version = m_ResourceNodes[resID].m_Version;
                    if (version == 1)
                        m_CompiledResourceEntries.push_back(entry.m_ID);
                }
            }

#ifdef FG_USE_FINE_GRAINED_LIFETIMES

            //struct Interval
            //{
            //    u32          lo, hi;
            //    u32          entryID;
            //    LifeTimeMode mode;
            //};
            //std::map<u32, std::vector<Interval>> intervals;
            // track last seen pass for each (entryID, mode)
            std::map<u32, u32> lastSeen;

            for (u32 passID = 0; passID < m_PassNodes.size(); ++passID) {
                const auto& pass = m_PassNodes[passID];
                if (pass.m_RefCount == 0) continue;

                // Store final culled passes in a array
                m_CompiledPassIndices.push_back(passID);

                auto touch = [&](const u32& resId, LifeTimeMode mode) {
                    u32              entryId  = getResourceNodeRef(resId).getResourceEntryId();
                    RZResourceEntry& resEntry = getResourceEntryRef(entryId);
                    //u32              key      = entryId;
                    u32 key = (entryId << 2) | static_cast<u32>(mode);

                    auto& realints = resEntry.getLifetimesRef();
                    //auto &ints     = intervals[key];
                    auto itLast = lastSeen.find(key);

                    if (itLast != lastSeen.end() && passID == itLast->second + 1) {
                        // extend existing interval
                        realints.back().EndPassID = passID;
                        //ints.back().hi            = passID;
                    } else {
                        // start new interval
                        realints.push_back(RZResourceLifetime{entryId, passID, passID, mode});
                        //ints.push_back(Interval{passID, passID, entryId, mode});
                    }
                    lastSeen[key] = passID;
                };

                for (auto& [resId, flags]: pass.m_Writes) touch(resId, LifeTimeMode::kWrite);
                for (auto& [resId, flags]: pass.m_Reads) touch(resId, LifeTimeMode::kRead);
            }

            //for (auto &kv: intervals) {
            //    u32   entryID = kv.first >> 2;
            //    auto &vec     = kv.second;

            // FIXME: enryID doesn't match with resourceName IDs because of multiple versions, we need to use compiledEntryIds
            // As they are stored per V1 resources and represent entryIDs more accurately, not sure how to solve that problem here
            //    RAZIX_CORE_INFO("ResourceEntryID: {} | Name: {}", entryID, getResourceEntryName(entryID));
            //    for (auto &intv: vec)
            //        RAZIX_CORE_WARN("    Interval [{} ... {}], nodeID={}, mode={}",
            //            intv.lo,
            //            intv.hi,
            //            intv.entryID,
            //            intv.mode);
            //}
            //RAZIX_CORE_INFO("");
#else

            for (u32 passID = 0; passID < m_PassNodes.size(); ++passID) {
                auto& pass = m_PassNodes[passID];
                if (pass.m_RefCount == 0) continue;

                // Store final culled passes in a array
                m_CompiledPassIndices.push_back(passID);

                // Create coarse lifetimes
                for (auto id: pass.m_Creates)
                    getResourceEntryRef(id).m_Producer = &pass;
                for (auto& [id, flags]: pass.m_Writes)
                    getResourceEntryRef(id).m_Last = &pass;
                for (auto& [id, flags]: pass.m_Reads) {
                    auto& entryRef  = getResourceEntryRef(id);
                    entryRef.m_Last = &pass;
                }
            }
#endif

            // Once lifetimes are determined we can create aliasing groups
            for (const auto& entry: m_CompiledResourceEntries) {
                m_TransientAllocator.registerLifetime(m_ResourceRegistry[entry].getCoarseLifetime());
            }

        }

        void RZFrameGraph::execute(void* transientAllocator)
        {
#ifndef RAZIX_GOLD_MASTER
            if (RZEngine::Get().getGlobalEngineSettings().EnableBarrierLogging)
                RAZIX_CORE_INFO("***************Frame Graph EXEC START***************");
#endif

            // Iterate though all passes and call their ExecuteFunc
            for (auto& pass: m_PassNodes) {
                // Only it it's executable and not culled
                if (!pass.canExecute()) continue;

                if (RZEngine::Get().getGlobalEngineSettings().EnableBarrierLogging) {
                    RAZIX_CORE_INFO("===============PASS START===============");
                    RAZIX_CORE_INFO("[Pass] executing pass: {0}", pass.m_Name);
                }

                // Call create for all the resources created by this node : Lazy Allocation --> helps with memory aliasing (pass transient resources)
                // Even for Data Driven passes this works be cause we create the RZFrameGraphTexture/Buffer while parsing the JSON graph and we have a pseudo SetupFunc
                for (const auto& id: pass.m_Creates)
                    getResourceEntryRef(id).getConcept()->create(transientAllocator);

                // TODO: To reduce unnecessary barries use version boundary changes to skip over some
                // Call pre-read and pre-write functions on the resource before the execute function
                // Safety of existence is taken care in the ResourceEntry class
                // Skip if they are imported resource, since imported resources are always Read only data!
                for (auto&& [id, flags]: pass.m_Reads) {
                    if (getResourceEntryRef(id).isTransient())
                        getResourceEntryRef(id).getConcept()->preRead(flags);
                }
                for (auto&& [id, flags]: pass.m_Writes) {
                    if (getResourceEntryRef(id).isTransient())
                        getResourceEntryRef(id).getConcept()->preWrite(flags);
                }

                // call the ExecuteFunc (same for Code and DataDriven passes)
                RZPassResourceDirectory resources{*this, pass};
                // https://stackoverflow.com/questions/43680182/what-is-stdinvoke-in-c
                std::invoke(*pass.m_Exec, pass, resources);

                // TODO: enable this when transient resources backend is done
                /**
                     * Current nodes resources can still be used by other nodes so we check
                     * for the EntryPoints and see at all the resources to check which are done with this
                     * the m_Last will keep track of which node will require this resource, if this pass is done
                     * then all the resources in the framegraph that depend on this can be deleted safely
                     */
                //for (auto &entry: m_ResourceRegistry)
                //    if (entry.m_Last == &pass && entry.isTransient())
                //        entry.getConcept()->destroy(transientAllocator);

#ifndef RAZIX_GOLD_MASTER
                if (RZEngine::Get().getGlobalEngineSettings().EnableBarrierLogging)
                    RAZIX_CORE_INFO("=============PASS END===================");
#endif
            }

#ifndef RAZIX_GOLD_MASTER
            if (RZEngine::Get().getGlobalEngineSettings().EnableBarrierLogging)
                RAZIX_CORE_INFO("***************Frame Graph EXEC END***************");
#endif

            // End first frame identifier
            RZFrameGraph::m_IsFirstFrame = false;
        }

        void RZFrameGraph::resize(u32 width, u32 height)
        {
            m_IsFirstFrame = true;

            // Iterate though all passes and call their ExecuteFunc
            for (auto& pass: m_PassNodes) {
                // Only it it's executable and not culled
                if (!pass.canExecute()) continue;

                for (const auto& id: pass.m_Creates)
                    if (getResourceEntryRef(id).isTransient())
                        getResourceEntryRef(id).getConcept()->resize(width, height);

                // call the ResizeFunc
                RZPassResourceDirectory resources{*this, pass};
                pass.m_Exec->resize(resources, width, height);
            }
        }

        struct StyleSheet
        {
            bool        useClusters{true};
            const char* rankDir{"TB"};    // TB, LR, BT, RL

            struct
            {
                const char* name{"helvetica"};
                int32_t     size{10};
            } font;
            struct
            {
                // https://graphviz.org/doc/info/colors.html
                struct
                {
                    const char* executed{"orange"};
                    const char* datadriven{"firebrick2"};
                    const char* culled{"lightgray"};
                } pass;
                struct
                {
                    const char* imported{"lightsteelblue"};
                    const char* transient{"skyblue"};
                } resource;
                struct
                {
                    const char* read{"olivedrab3"};
                    const char* write{"orangered"};
                } edge;
            } color;
        };

        void RZFrameGraph::exportToGraphViz(std::ostream& os) const
        {
            // https://www.graphviz.org/pdf/dotguide.pdf

            static StyleSheet style;

            os << "digraph FrameGraph {" << std::endl;
            os << "graph [style=invis, rankdir=\"" << style.rankDir
               << "\" ordering=out, splines=spline]" << std::endl;
            os << "node [shape=record, fontname=\"" << style.font.name
               << "\", fontsize=" << style.font.size << ", margin=\"0.2,0.03\"]"
               << std::endl
               << std::endl;

            // -- Define pass nodes

            for (const RZPassNode& node: m_PassNodes) {
                os << "P" << node.m_ID << " [label=<{ {<B>" << node.m_Name << "</B>} | {"
                   << (node.isStandAlone() ? "&#x2605; " : "")
                   << "Refs: " << node.m_RefCount << "<BR/> Index: " << node.m_ID
                   << "} }> style=\"rounded,filled\", fillcolor="
                   << ((node.m_RefCount > 0 || node.isStandAlone())
                              ? (node.isDataDriven() ? style.color.pass.datadriven : style.color.pass.executed)
                              : style.color.pass.culled);

                os << "]" << std::endl;
            }
            os << std::endl;

            // -- Define resource nodes

            for (const RZResourceNode& node: m_ResourceNodes) {
                const auto& entry = m_ResourceRegistry[node.m_ResourceEntryID];
                os << "R" << entry.m_ID << "_" << node.m_Version << " [label=<{ {<B>"
                   << node.m_Name << "</B>";
                if (node.m_Version > kRESOURCE_INITIAL_VERSION) {
                    // FIXME: Bold text overlaps regular text
                    os << "   <FONT>v" + std::to_string(node.m_Version) + "</FONT>";
                }
                os << "<BR/>" << entry.getConcept()->toString() << "} | {Index: " << entry.m_ID << "<BR/>"
                   << "Refs : " << node.m_RefCount << "} }> style=filled, fillcolor="
                   << (entry.isImported() ? style.color.resource.imported
                                          : style.color.resource.transient);

                os << "]" << std::endl;
            }
            os << std::endl;

            // -- Each pass node points to resource that it writes

            for (const RZPassNode& node: m_PassNodes) {
                os << "P" << node.m_ID << " -> { ";
                for (auto [id, flags]: node.m_Writes) {
                    const auto& written = m_ResourceNodes[id];
                    os << "R" << written.m_ResourceEntryID << "_" << written.m_Version << " ";
                }
                os << "} [color=" << style.color.edge.write << "]" << std::endl;
            }

            // -- Each resource node points to pass where it's consumed

            os << std::endl;
            for (const RZResourceNode& node: m_ResourceNodes) {
                os << "R" << node.m_ResourceEntryID << "_" << node.m_Version << " -> { ";
                // find all readers of this resource node
                for (const RZPassNode& pass: m_PassNodes) {
                    for (const auto [id, flags]: pass.m_Reads)
                        if (id == node.m_ID) os << "P" << pass.m_ID << " ";
                }
                os << "} [color=" << style.color.edge.read << "]" << std::endl;
            }
            os << std::endl;

            // -- Clusters:

            if (style.useClusters) {
                for (const RZPassNode& node: m_PassNodes) {
                    os << "subgraph cluster_" << node.m_ID << " {" << std::endl;

                    os << "P" << node.m_ID << " ";
                    for (auto id: node.m_Creates) {
                        const auto& r = m_ResourceNodes[id];
                        os << "R" << r.m_ResourceEntryID << "_" << r.m_Version << " ";
                    }
                    os << std::endl
                       << "}" << std::endl;
                }
                os << std::endl;

                os << "subgraph cluster_imported_resources {" << std::endl;
                os << "graph [style=dotted, fontname=\"helvetica\", label=< "
                      "<B>Imported</B> >]"
                   << std::endl;

                for (const RZResourceEntry& entry: m_ResourceRegistry) {
                    if (entry.isImported()) os << "R" << entry.m_ID << "_1 ";
                }
                os << std::endl
                   << "}" << std::endl
                   << std::endl;
            }

            os << "}";
        }

        void RZFrameGraph::exportToGraphViz(const std::string& location) const
        {
            std::ofstream os(location, std::ofstream::out | std::ofstream::trunc);
            RAZIX_CORE_INFO("Exporting FrameGraph .... to ({0})", location);
            os << *this;
            os.close();
        }

        void RZFrameGraph::destroy()
        {
            for (auto& entry: m_ResourceRegistry)
                entry.getConcept()->destroy(nullptr);

            m_PassNodes.clear();
            m_ResourceNodes.clear();
            m_ResourceRegistry.clear();

            m_Blackboard.destroy();
        }

        bool RZFrameGraph::isValid(RZFrameGraphResource id)
        {
            const auto& node     = getResourceNodeRef(id);
            auto&       resource = m_ResourceRegistry[node.m_ResourceEntryID];
            return node.m_Version == resource.m_Version;
        }

        RZResourceNode& RZFrameGraph::getResourceNodeRef(RZFrameGraphResource id)
        {
            assert(id < m_ResourceNodes.size());
            return m_ResourceNodes[id];
        }

        RZResourceEntry& RZFrameGraph::getResourceEntryRef(RZFrameGraphResource id)
        {
            const auto& node = getResourceNodeRef(id);
            assert(node.m_ResourceEntryID < m_ResourceRegistry.size());
            return m_ResourceRegistry[node.m_ResourceEntryID];
        }

        std::ostream& operator<<(std::ostream& os, const RZFrameGraph& fg)
        {
            fg.exportToGraphViz(os);
            return os;
        }

        const std::string& RZFrameGraph::getResourceName(RZFrameGraphResource id) const
        {
            assert(id < m_ResourceNodes.size());
            auto& resNode = m_ResourceNodes[id];
            return resNode.getName();
        }

        static std::string s_RES_NOT_FUND_STR("RESOURCE_ENTRY_NOT_FOUND");

        const std::string& RZFrameGraph::getResourceEntryName(RZFrameGraphResource id) const
        {
            assert(id < m_ResourceNodes.size());
            for (u32 i = 0; i < m_ResourceNodes.size(); i++) {
                auto& resNode = m_ResourceNodes[id];
                if (resNode.getResourceEntryId() == id)
                    return resNode.getName();
            }
            return s_RES_NOT_FUND_STR;
        }

        const RZResourceNode& RZFrameGraph::getResourceNode(RZFrameGraphResource id) const
        {
            assert(id < m_ResourceNodes.size());
            return m_ResourceNodes[id];
        }

        const RZResourceEntry& RZFrameGraph::getResourceEntry(RZFrameGraphResource id) const
        {
            const auto& node = getResourceNode(id);
            assert(node.m_ResourceEntryID < m_ResourceRegistry.size());
            return m_ResourceRegistry[node.m_ResourceEntryID];
        }

        RZPassNode& RZFrameGraph::createPassNodeRef(const std::string_view name, std::unique_ptr<IRZFrameGraphPass>&& base)
        {
            const auto id = static_cast<u32>(m_PassNodes.size());
            return m_PassNodes.emplace_back(RZPassNode(name, id, std::move(base)));
        }

        RZResourceNode& RZFrameGraph::createResourceNodeRef(const std::string_view name, u32 resourceID)
        {
            const auto id = static_cast<u32>(m_ResourceNodes.size());
            return m_ResourceNodes.emplace_back(RZResourceNode(name, id, resourceID, kRESOURCE_INITIAL_VERSION));
        }

        RZFrameGraphResource RZFrameGraph::cloneResource(RZFrameGraphResource id)
        {
            // Get the OG resource and increase it's version
            const auto& node = getResourceNodeRef(id);
            assert(node.m_ResourceEntryID < m_ResourceRegistry.size());
            auto& entry = m_ResourceRegistry[node.m_ResourceEntryID];
            entry.m_Version++;

            // Now add it to the m_ResourceNodes array, see here we have more resources than the entry point
            // Despite being uses differently in the graph these read/write/clones have same entry point so we have a separate array for them
            const auto cloneId = static_cast<u32>(m_ResourceNodes.size());
            m_ResourceNodes.emplace_back(RZResourceNode(node.m_Name, cloneId, node.m_ResourceEntryID, entry.getVersion()));
            return cloneId;
        }

        RZPassResourceBuilder* RZFrameGraph::CreateBuilder(RZFrameGraph& fg, RZPassNode& passNode)
        {
            return new RZPassResourceBuilder(fg, passNode);
        }

        //-----------------------------------------------------------------------------------
        // RZPassResourceBuilder Class
        //-----------------------------------------------------------------------------------

        RZPassResourceBuilder::RZPassResourceBuilder(RZFrameGraph& frameGraph, RZPassNode& passNode)
            : m_FrameGraph(frameGraph), m_PassNode{passNode}
        {
        }

        RZFrameGraphResource RZPassResourceBuilder::read(RZFrameGraphResource id, u32 flags /*= kFlagsNone*/)
        {
            RAZIX_ASSERT(m_FrameGraph.isValid(id), "Invalid resource");
            auto readID = m_PassNode.registerResourceForRead(id, flags);

            // Register the name, this makes code based frame graph pass resources compatible with data driven passes
            m_FrameGraph.m_Blackboard.add(m_FrameGraph.getResourceName(readID), readID);

            return readID;
        }

        RZFrameGraphResource RZPassResourceBuilder::write(RZFrameGraphResource id, u32 flags /*= kFlagsNone*/)
        {
            RAZIX_ASSERT(m_FrameGraph.isValid(id), "Invalid resource");

            // If it writes to an imported resource mark this pass as stand alone pass
            if (m_FrameGraph.getResourceEntryRef(id).isImported())
                setAsStandAlonePass();

            RZFrameGraphResource writeID{id};

            // If the pass creates a resources it means its writing to it
            if (m_PassNode.canCreateResouce(id))
                writeID = m_PassNode.registerResourceForWrite(id, flags);
            else {
                /**
                  * Old Usage:
                  * If it's writing to a resource not created by this PassNode
                  * then, it must first be able to read from it and then write to 
                  * a clone of the same resource 
                  */
                /**
                 * [2025-06-15]
                 * Registering the same id for a read immediately before cloning
                 * and writing to it doesn't make sense:
                 *  - You mark the original resource "read" in this pass,
                 *  - Immediately clone it and write to the clone,
                 *  - You never actually consume that "read" lifetime on the original.
                 *
                 * This leads to back‑to‑back touches on the original entry in the
                 * same pass, producing zero length or duplicate lifetime segments
                 * which serve no runtime purpose. Instead, we skip the read registration
                 * and directly clone + write:
                 */
                m_PassNode.registerResourceForRead(id, flags);
                // we're writing to the same existing external resource so clone it before writing to it
                writeID = m_PassNode.registerResourceForWrite(m_FrameGraph.cloneResource(id), flags);
            }

            // Register the name, this makes code based frame graph pass resources compatible with data driven passes
            m_FrameGraph.m_Blackboard.add(m_FrameGraph.getResourceName(writeID), writeID);

            return writeID;
        }

        RZPassResourceBuilder& RZPassResourceBuilder::setAsStandAlonePass()
        {
            m_PassNode.m_IsStandAlone = true;
            return *this;
        }

        RZPassResourceBuilder& RZPassResourceBuilder::setDepartment(Department dept)
        {
            m_PassNode.m_Department = dept;
            return *this;
        }

        RZPassResourceBuilder& RZPassResourceBuilder::setCPUTime(f32 time)
        {
            m_PassNode.m_CurrentPassBudget.CPUframeBudget = time;
            return *this;
        }

        Department RZPassResourceBuilder::getDepartment()
        {
            return m_PassNode.m_Department;
        }

        Memory::BudgetInfo RZPassResourceBuilder::getPassCurrentBudget()
        {
            return m_PassNode.m_CurrentPassBudget;
        }

        f32 RZPassResourceBuilder::getPassCurrentCPUTimeBudget()
        {
            return m_PassNode.m_CurrentPassBudget.CPUframeBudget;
        }

        u32 RZPassResourceBuilder::getPassCurrentMemoryBudget()
        {
            return m_PassNode.m_CurrentPassBudget.MemoryBudget;
        }

        //-----------------------------------------------------------------------------------
        // RZPassResourcesDirectory Class
        //-----------------------------------------------------------------------------------

        RZPassResourceDirectory::RZPassResourceDirectory(RZFrameGraph& frameGraph, RZPassNode& passNode)
            : m_FrameGraph(frameGraph), m_PassNode{passNode}
        {
        }
    }    // namespace Gfx
}    // namespace Razix
