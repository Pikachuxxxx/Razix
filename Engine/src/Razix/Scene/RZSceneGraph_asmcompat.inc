/* AUTO GENERATED - DO NOT EDIT */

/* enum rz_compression_type */
#define SIZEOF_rz_compression_type 4
#define RZ_COMPRESSION_NONE 0
#define RZ_COMPRESSION_ZLIB 1
#define RZ_COMPRESSION_LZ4 2
#define RZ_COMPRESSION_BCn 4
#define RZ_COMPRESSION_ASTC 8
#define RZ_COMPRESSION_MESHOPT 16
#define RZ_COMPRESSION_FLAC 32
#define RZ_COMPRESSION_COUNT 7

/* enum rz_compression_type */
#define SIZEOF_rz_compression_type 4
#define RZ_COMPRESSION_NONE 0
#define RZ_COMPRESSION_ZLIB 1
#define RZ_COMPRESSION_LZ4 2
#define RZ_COMPRESSION_BCn 4
#define RZ_COMPRESSION_ASTC 8
#define RZ_COMPRESSION_MESHOPT 16
#define RZ_COMPRESSION_FLAC 32
#define RZ_COMPRESSION_COUNT 7

/* enum rz_compression_result */
#define SIZEOF_rz_compression_result 4
#define RZ_COMPRESSION_OK 0
#define RZ_COMPRESSION_ERROR 1
#define RZ_COMPRESSION_INVALID_ARGUMENT 2
#define RZ_COMPRESSION_BUFFER_TOO_SMALL 3

/* enum rz_compression_result */
#define SIZEOF_rz_compression_result 4
#define RZ_COMPRESSION_OK 0
#define RZ_COMPRESSION_ERROR 1
#define RZ_COMPRESSION_INVALID_ARGUMENT 2
#define RZ_COMPRESSION_BUFFER_TOO_SMALL 3

/* struct rz_uuid */
#define SIZEOF_rz_uuid 16
#define OFFSETOF_rz_uuid_data 0

/* struct rz_uuid */
#define SIZEOF_rz_uuid 16
#define OFFSETOF_rz_uuid_data 0

/* struct RZDate */
#define SIZEOF_RZDate 4
#define OFFSETOF_RZDate_year 0
#define OFFSETOF_RZDate_month 2
#define OFFSETOF_RZDate_day 3

/* struct RZDate */
#define SIZEOF_RZDate 4
#define OFFSETOF_RZDate_year 0
#define OFFSETOF_RZDate_month 2
#define OFFSETOF_RZDate_day 3

/* struct RZDateTime */
#define SIZEOF_RZDateTime 8
#define OFFSETOF_RZDateTime_year 0
#define OFFSETOF_RZDateTime_month 2
#define OFFSETOF_RZDateTime_day 3
#define OFFSETOF_RZDateTime_hour 4
#define OFFSETOF_RZDateTime_minute 5
#define OFFSETOF_RZDateTime_second 6

/* struct RZDateTime */
#define SIZEOF_RZDateTime 8
#define OFFSETOF_RZDateTime_year 0
#define OFFSETOF_RZDateTime_month 2
#define OFFSETOF_RZDateTime_day 3
#define OFFSETOF_RZDateTime_hour 4
#define OFFSETOF_RZDateTime_minute 5
#define OFFSETOF_RZDateTime_second 6

/* enum EventType */
#define SIZEOF_EventType 4
#define kNone 0
#define kWindowClose 1
#define kWindowResize 2
#define kWindowFocus 3
#define kWindowLostFocus 4
#define kWindowMoved 5
#define kAppTick 6
#define kAppUpdate 7
#define kAppRender 8
#define kKeyPressed 9
#define kKeyReleased 10
#define kMouseButtonPressed 11
#define kMouseButtonReleased 12
#define kMouseMoved 13
#define kMouseScrolled 14
#define kAssetCreated 15
#define kAssetModified 16
#define kAssetMemoryMoved 17
#define kAssetCopied 18
#define kAssetDeleted 19
#define kAssetBroken 20
#define kAssetSerialized 21
#define kAssetDeserialzed 22

/* enum RZAssetType */
#define SIZEOF_RZAssetType 1
#define kTransform 0
#define kCamera 1
#define kLight 2
#define kMaterial 3
#define kPhysicsMaterial 4
#define kMesh 5
#define kTexture 6
#define kAnimation 7
#define kAudio 8
#define kLuaScript 9
#define kAssetRef 10
#define kVignerePuzzle 11
#define kCloth 12
#define kGameData 13

/* enum RZAssetStorageType */
#define SIZEOF_RZAssetStorageType 4
#define kMemoryBacked 0
#define kGPUBacked 1
#define kRuntimeGenBacked 2
#define kNonMemoryBacked 3
#define kLazyMemoryBacked 4
#define kSceneGraphBacked 5

/* struct RZAssetVersion */
#define SIZEOF_RZAssetVersion 32
#define OFFSETOF_RZAssetVersion_revisionID 0
#define OFFSETOF_RZAssetVersion_major 16
#define OFFSETOF_RZAssetVersion__pad0 24

/* struct RZAssetVersion */
#define SIZEOF_RZAssetVersion 32
#define OFFSETOF_RZAssetVersion_revisionID 0
#define OFFSETOF_RZAssetVersion_major 16
#define OFFSETOF_RZAssetVersion__pad0 24

/* enum RZAssetFlags */
#define SIZEOF_RZAssetFlags 1
#define RZ_ASSET_FLAG_NONE 0
#define RZ_ASSET_FLAG_READY 0
#define RZ_ASSET_FLAG_COMPRESSED 1
#define RZ_ASSET_FLAG_ENCRYPTED 2
#define RZ_ASSET_FLAG_READONLY 4
#define RZ_ASSET_FLAG_DIRTY 8
#define RZ_ASSET_FLAG_STREAMING 32
#define RZ_ASSET_FLAG_PLACEHOLDER 64
#define RZ_ASSET_FLAG_COUNT 7

/* enum RZAssetFlags */
#define SIZEOF_RZAssetFlags 1
#define RZ_ASSET_FLAG_NONE 0
#define RZ_ASSET_FLAG_READY 0
#define RZ_ASSET_FLAG_COMPRESSED 1
#define RZ_ASSET_FLAG_ENCRYPTED 2
#define RZ_ASSET_FLAG_READONLY 4
#define RZ_ASSET_FLAG_DIRTY 8
#define RZ_ASSET_FLAG_STREAMING 32
#define RZ_ASSET_FLAG_PLACEHOLDER 64
#define RZ_ASSET_FLAG_COUNT 7

/* struct RZAssetMetadata */
#define SIZEOF_RZAssetMetadata 448
#define OFFSETOF_RZAssetMetadata_name 0
#define OFFSETOF_RZAssetMetadata_author 96
#define OFFSETOF_RZAssetMetadata_description 192
#define OFFSETOF_RZAssetMetadata_commitHash 288
#define OFFSETOF_RZAssetMetadata_version 384
#define OFFSETOF_RZAssetMetadata_packlastModified 416
#define OFFSETOF_RZAssetMetadata_createdDate 420
#define OFFSETOF_RZAssetMetadata_department 422
#define OFFSETOF_RZAssetMetadata__pad0 423

/* struct RZAssetMetadata_TypeRegistration */
#define SIZEOF_RZAssetMetadata_TypeRegistration 1

/* struct RZAssetDependecy */
#define SIZEOF_RZAssetDependecy 32
#define OFFSETOF_RZAssetDependecy_assetID 0
#define OFFSETOF_RZAssetDependecy_type 16
#define OFFSETOF_RZAssetDependecy__pad0 17

/* struct RZAssetDependecy_TypeRegistration */
#define SIZEOF_RZAssetDependecy_TypeRegistration 1

/* struct RZAssetHotData */
#define SIZEOF_RZAssetHotData 56
#define OFFSETOF_RZAssetHotData_UUID 0
#define OFFSETOF_RZAssetHotData_handle 16
#define OFFSETOF_RZAssetHotData_referenceCount 24
#define OFFSETOF_RZAssetHotData_type 32
#define OFFSETOF_RZAssetHotData_storagePreference 36
#define OFFSETOF_RZAssetHotData_flags 40
#define OFFSETOF_RZAssetHotData_reserved 41

/* struct RZAssetHotData_TypeRegistration */
#define SIZEOF_RZAssetHotData_TypeRegistration 1

/* struct RZAssetColdData */
#define SIZEOF_RZAssetColdData 1
#define OFFSETOF_RZAssetColdData_dependencies -1
#define OFFSETOF_RZAssetColdData_eventDispatcher -1
#define OFFSETOF_RZAssetColdData_metadata -1
#define OFFSETOF_RZAssetColdData__pad0 -1
#define OFFSETOF_RZAssetColdData_CS -1

/* struct RZAssetColdData_TypeRegistration */
#define SIZEOF_RZAssetColdData_TypeRegistration 1

/* struct RZAsset_TypeRegistration */
#define SIZEOF_RZAsset_TypeRegistration 1

/* struct RZAsset_TypeRegistration */
#define SIZEOF_RZAsset_TypeRegistration 1

/* struct rz_aabb */
#define SIZEOF_rz_aabb 1
#define OFFSETOF_rz_aabb_min -1
#define OFFSETOF_rz_aabb_max -1

/* struct rz_aabb */
#define SIZEOF_rz_aabb 1
#define OFFSETOF_rz_aabb_min -1
#define OFFSETOF_rz_aabb_max -1

/* enum rz_node_flags */
#define SIZEOF_rz_node_flags 4
#define RZ_NODE_FLAG_NONE 0
#define RZ_NODE_FLAG_DIRTY 1
#define RZ_NODE_FLAG_VISIBLE 2
#define RZ_NODE_FLAG_STATIC 4
#define RZ_NODE_FLAG_PLACEHOLDER 8

/* enum rz_node_flags */
#define SIZEOF_rz_node_flags 4
#define RZ_NODE_FLAG_NONE 0
#define RZ_NODE_FLAG_DIRTY 1
#define RZ_NODE_FLAG_VISIBLE 2
#define RZ_NODE_FLAG_STATIC 4
#define RZ_NODE_FLAG_PLACEHOLDER 8

/* struct rz_gather_list */
#define SIZEOF_rz_gather_list 16
#define OFFSETOF_rz_gather_list_pIndices 0
#define OFFSETOF_rz_gather_list_count 8
#define OFFSETOF_rz_gather_list_capacity 12

/* struct rz_gather_list */
#define SIZEOF_rz_gather_list 16
#define OFFSETOF_rz_gather_list_pIndices 0
#define OFFSETOF_rz_gather_list_count 8
#define OFFSETOF_rz_gather_list_capacity 12

/* struct rz_dirty_entry */
#define SIZEOF_rz_dirty_entry 8
#define OFFSETOF_rz_dirty_entry_nodeIdx 0
#define OFFSETOF_rz_dirty_entry_depth 4
#define OFFSETOF_rz_dirty_entry__pad0 6

/* struct rz_dirty_entry */
#define SIZEOF_rz_dirty_entry 8
#define OFFSETOF_rz_dirty_entry_nodeIdx 0
#define OFFSETOF_rz_dirty_entry_depth 4
#define OFFSETOF_rz_dirty_entry__pad0 6

/* struct rz_dirty_list */
#define SIZEOF_rz_dirty_list 24
#define OFFSETOF_rz_dirty_list_pEntries 0
#define OFFSETOF_rz_dirty_list_pTempEntries 8
#define OFFSETOF_rz_dirty_list_count 16
#define OFFSETOF_rz_dirty_list_capacity 20

/* struct rz_dirty_list */
#define SIZEOF_rz_dirty_list 24
#define OFFSETOF_rz_dirty_list_pEntries 0
#define OFFSETOF_rz_dirty_list_pTempEntries 8
#define OFFSETOF_rz_dirty_list_count 16
#define OFFSETOF_rz_dirty_list_capacity 20

/* struct rz_scene_node */
#define SIZEOF_rz_scene_node 64
#define OFFSETOF_rz_scene_node_uuid 0
#define OFFSETOF_rz_scene_node_handle 16
#define OFFSETOF_rz_scene_node_flags 24
#define OFFSETOF_rz_scene_node_depth 28
#define OFFSETOF_rz_scene_node__pad0 30
#define OFFSETOF_rz_scene_node_parent 32
#define OFFSETOF_rz_scene_node_firstChild 36
#define OFFSETOF_rz_scene_node_nextSibling 40
#define OFFSETOF_rz_scene_node__pad1 44

/* struct rz_scene_node */
#define SIZEOF_rz_scene_node 64
#define OFFSETOF_rz_scene_node_uuid 0
#define OFFSETOF_rz_scene_node_handle 16
#define OFFSETOF_rz_scene_node_flags 24
#define OFFSETOF_rz_scene_node_depth 28
#define OFFSETOF_rz_scene_node__pad0 30
#define OFFSETOF_rz_scene_node_parent 32
#define OFFSETOF_rz_scene_node_firstChild 36
#define OFFSETOF_rz_scene_node_nextSibling 40
#define OFFSETOF_rz_scene_node__pad1 44

/* enum rz_zone_state */
#define SIZEOF_rz_zone_state 4
#define RZ_ZONE_INVALID 0
#define RZ_ZONE_UNLOADED 1
#define RZ_ZONE_LOADING 2
#define RZ_ZONE_LOADED 3

/* enum rz_zone_state */
#define SIZEOF_rz_zone_state 4
#define RZ_ZONE_INVALID 0
#define RZ_ZONE_UNLOADED 1
#define RZ_ZONE_LOADING 2
#define RZ_ZONE_LOADED 3

/* struct rz_zone */
#define SIZEOF_rz_zone 1
#define OFFSETOF_rz_zone_name -1
#define OFFSETOF_rz_zone_gridPos -1
#define OFFSETOF_rz_zone_bounds -1
#define OFFSETOF_rz_zone_state -1
#define OFFSETOF_rz_zone_pNodes -1
#define OFFSETOF_rz_zone_nodeCount -1
#define OFFSETOF_rz_zone_nodeCapacity -1
#define OFFSETOF_rz_zone_rootNodeIndex -1
#define OFFSETOF_rz_zone_pNodeFreelist -1
#define OFFSETOF_rz_zone_nodeFreelistTop -1
#define OFFSETOF_rz_zone_gather -1
#define OFFSETOF_rz_zone_dirtyTransforms -1

/* struct rz_zone */
#define SIZEOF_rz_zone 1
#define OFFSETOF_rz_zone_name -1
#define OFFSETOF_rz_zone_gridPos -1
#define OFFSETOF_rz_zone_bounds -1
#define OFFSETOF_rz_zone_state -1
#define OFFSETOF_rz_zone_pNodes -1
#define OFFSETOF_rz_zone_nodeCount -1
#define OFFSETOF_rz_zone_nodeCapacity -1
#define OFFSETOF_rz_zone_rootNodeIndex -1
#define OFFSETOF_rz_zone_pNodeFreelist -1
#define OFFSETOF_rz_zone_nodeFreelistTop -1
#define OFFSETOF_rz_zone_gather -1
#define OFFSETOF_rz_zone_dirtyTransforms -1

/* struct rz_global_trigger */
#define SIZEOF_rz_global_trigger 1
#define OFFSETOF_rz_global_trigger_name -1
#define OFFSETOF_rz_global_trigger_bounds -1
#define OFFSETOF_rz_global_trigger_eventHash -1

/* struct rz_global_trigger */
#define SIZEOF_rz_global_trigger 1
#define OFFSETOF_rz_global_trigger_name -1
#define OFFSETOF_rz_global_trigger_bounds -1
#define OFFSETOF_rz_global_trigger_eventHash -1

/* struct rz_scene_graph */
#define SIZEOF_rz_scene_graph 1
#define OFFSETOF_rz_scene_graph_version -1
#define OFFSETOF_rz_scene_graph_sceneBounds -1
#define OFFSETOF_rz_scene_graph_gridDims -1
#define OFFSETOF_rz_scene_graph_zonePhysicalSize -1
#define OFFSETOF_rz_scene_graph_zones -1
#define OFFSETOF_rz_scene_graph_zoneCount -1
#define OFFSETOF_rz_scene_graph_activeZoneIndex -1
#define OFFSETOF_rz_scene_graph_pTriggers -1
#define OFFSETOF_rz_scene_graph_triggerCount -1

/* struct rz_scene_graph */
#define SIZEOF_rz_scene_graph 1
#define OFFSETOF_rz_scene_graph_version -1
#define OFFSETOF_rz_scene_graph_sceneBounds -1
#define OFFSETOF_rz_scene_graph_gridDims -1
#define OFFSETOF_rz_scene_graph_zonePhysicalSize -1
#define OFFSETOF_rz_scene_graph_zones -1
#define OFFSETOF_rz_scene_graph_zoneCount -1
#define OFFSETOF_rz_scene_graph_activeZoneIndex -1
#define OFFSETOF_rz_scene_graph_pTriggers -1
#define OFFSETOF_rz_scene_graph_triggerCount -1

/* struct rz_scene_graph_manager */
#define SIZEOF_rz_scene_graph_manager 80
#define OFFSETOF_rz_scene_graph_manager_pActiveScene 0
#define OFFSETOF_rz_scene_graph_manager_pQueuedScenes 8
#define OFFSETOF_rz_scene_graph_manager_queuedCount 72

/* struct rz_scene_graph_manager */
#define SIZEOF_rz_scene_graph_manager 80
#define OFFSETOF_rz_scene_graph_manager_pActiveScene 0
#define OFFSETOF_rz_scene_graph_manager_pQueuedScenes 8
#define OFFSETOF_rz_scene_graph_manager_queuedCount 72

/* struct rz_asset_db */
#define SIZEOF_rz_asset_db -2

/* enum rz_sexp_token_type */
#define SIZEOF_rz_sexp_token_type 4
#define RZ_SEXP_LPAREN 0
#define RZ_SEXP_RPAREN 1
#define RZ_SEXP_ATOM 2
#define RZ_SEXP_STRING 3

/* enum rz_sexp_token_type */
#define SIZEOF_rz_sexp_token_type 4
#define RZ_SEXP_LPAREN 0
#define RZ_SEXP_RPAREN 1
#define RZ_SEXP_ATOM 2
#define RZ_SEXP_STRING 3

/* struct rz_sexp_token */
#define SIZEOF_rz_sexp_token 12
#define OFFSETOF_rz_sexp_token_offset 0
#define OFFSETOF_rz_sexp_token_length 4
#define OFFSETOF_rz_sexp_token_type 8

/* struct rz_sexp_token */
#define SIZEOF_rz_sexp_token 12
#define OFFSETOF_rz_sexp_token_offset 0
#define OFFSETOF_rz_sexp_token_length 4
#define OFFSETOF_rz_sexp_token_type 8

/* struct rz_sexp_tokenizer_result */
#define SIZEOF_rz_sexp_tokenizer_result 16
#define OFFSETOF_rz_sexp_tokenizer_result_pTokens 0
#define OFFSETOF_rz_sexp_tokenizer_result_count 8
#define OFFSETOF_rz_sexp_tokenizer_result_capacity 12

/* struct rz_sexp_tokenizer_result */
#define SIZEOF_rz_sexp_tokenizer_result 16
#define OFFSETOF_rz_sexp_tokenizer_result_pTokens 0
#define OFFSETOF_rz_sexp_tokenizer_result_count 8
#define OFFSETOF_rz_sexp_tokenizer_result_capacity 12
