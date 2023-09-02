#pragma once

/**
* FrameGraph is an alias for Render Graph which controls the entire frame and it's rendering process
* Based on : Copyright (c) Dawid Kurek, GitHub : skaarj1989 [https://github.com/skaarj1989/FrameGraph] MIT license. 
* With Additional Changes Copyright (c) by Phani Srikar (Pikachuxxxx) MIT license.
* Inspired from EA's Frostbite engine : https://www.gdcvault.com/play/1024612/FrameGraph-Extensible-Rendering-Architecture-in
*/

#include <string_view>
#include <type_traits>

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            // [SFINAE] Source 1 :  https://www.cppstories.com/2016/02/notes-on-c-sfinae/
            // [SFINAE] Source 2 :  https://jguegant.github.io/blogs/tech/sfinae-introduction.html

            // This is basically same implementation effect as what Dawid Kurek, GitHub : skaarj1989 wrote, I just renamed stuff and made them more readable
            // Also the implementation is very much same as Dawid Kurek's and has minor changes from what I understood from reading the blogs online

            // Checks for Functions - Create, Destroy, toString

            /**
             * Note: T is still undefined and is specified by frame graph users to define custom types for wrapping Graphics API handles,
             * But, Hey! we at least know what to check for! but just not on what so we'll at least specify that
             */

            RAZIX_CHECK_TYPE_HAS_FUNCTION(T, create);
            RAZIX_CHECK_TYPE_HAS_FUNCTION(T, destroy);
            RAZIX_CHECK_TYPE_HAS_FUNCTION(T, toString);

            // Checks for Sub types - Desc

            RAZIX_CHECK_TYPE_HAS_SUBTYPE(T, Desc);

            /**
             * How to know if it's a valid resource (the one that is being passes as universal reference to the concept class)
             * 
             * These are the conditions it must satisfy, if not it will fail and we will not generate overload sets!
             * 
             * Ofc we can also throw compile time errors for that type and not wait until instantiation time
             */
            template<typename T>
            constexpr bool is_acceptible_frame_graph_resource()
            {
                return std::is_default_constructible_v<T> &&
                       std::is_move_constructible_v<T> && RAZIX_TYPE_HAS_SUB_TYPE_V(T, Desc) && RAZIX_TYPE_HAS_FUNCTION_V(T, create) &&
                       RAZIX_TYPE_HAS_FUNCTION_V(T, destroy());
            }

            /**
             * Okay using type erasure we will register wrapper frame graph resource types using template Type T
             * now on this T we can use the above checks to report any missing functions - Solution for fake pure virtual func checks
             * 
             * Now when we call methods like create/destroy etc. on T, it's done from the FrameGraph class, and what we do is using SFINAE 
             * we reject overload sets that is not a resource, we enforce the Concept on it like pure virtual and generate only those template 
             * specializations that we want!, we use the failure to our advantage to reject the overload sets that don't conform the concept 
             * 
             */

            /**
             * overload sets will be generated if T when evaluated by is_resource is true
             * using SFINAE for conditionally removing functions from the candidate set 
             * 
             * if it fails we can use SFINAE to trigger errors on why it happened or use that failure 
             * path to rectify the stuff and dispatch to alternate path
             */
#define ENFORCE_RESOURCE_ENTRY_CONCEPT_ON_TYPE template<typename T, typename = std::enable_if_t<is_acceptible_frame_graph_resource<T>()>>

        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix