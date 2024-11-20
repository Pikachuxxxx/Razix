#pragma once

namespace Razix {
	namespace Gfx {

		struct RZDecal
		{
			std::string name;
			std::string diffuseMapPath;
			std::string normalMapPath;
			std::string specularMapPath;
			std::string emissiveMapPath;
			glm::vec4   color;
			f32         opacity;
			u32         normalThreshold;
			u32         layerPriority;

			// Runtime instance of the textures
			Gfx::RZTextureHandle diffuseMap;
			Gfx::RZTextureHandle normalMap;
			Gfx::RZTextureHandle specularMap;
			Gfx::RZTextureHandle emissiveMap;

			template<class Archive>
			void load(Archive& archive)
			{
				archive(cereal::make_nvp("name", name));
				archive(cereal::make_nvp("diffuseMap", diffuseMapPath));
				archive(cereal::make_nvp("normalMap", normalMapPath));
				archive(cereal::make_nvp("specularMap", specularMapPath));
				archive(cereal::make_nvp("emissiveMap", emissiveMapPath));
				archive(cereal::make_nvp("color", color));
				archive(cereal::make_nvp("opacity", opacity));
				archive(cereal::make_nvp("normalThreshold", normalThreshold));
				archive(cereal::make_nvp("layerPriority", layerPriority));

				// Create the textures
				// FIXME Only diffuse and normal for now, they are mandatory

			}

			template<class Archive>
			void save(Archive& archive) const
			{
				archive(cereal::make_nvp("name", name));
				archive(cereal::make_nvp("diffuseMap", diffuseMapPath));
				archive(cereal::make_nvp("normalMap", normalMapPath));
				archive(cereal::make_nvp("specularMap", specularMapPath));
				archive(cereal::make_nvp("emissiveMap", emissiveMapPath));
				archive(cereal::make_nvp("color", color));
				archive(cereal::make_nvp("opacity", opacity));
				archive(cereal::make_nvp("normalThreshold", normalThreshold));
				archive(cereal::make_nvp("layerPriority", layerPriority));
			}
		};
	}
}