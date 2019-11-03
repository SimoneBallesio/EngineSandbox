#include "Material.h"

Material::Material()
{
}

void Material::SetShader(const Shader& shader)
{
	m_shader = shader;
}

void Material::SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4 & projection)
{
	m_shader.Use();
	m_shader.SetMat4("view", view);
	m_shader.SetMat4("projection", projection);
	m_shader.SetMat4("model", model);
}

void Material::SetMaterialProperty(const std::string& name, const std::variant<int, float, std::string, bool>& value)
{
	unsigned int hash = static_cast<unsigned int>(std::hash<std::string>{}(name));

	auto findIt = m_materialMapToVectorIndex.find(hash);
	if (findIt == m_materialMapToVectorIndex.end())
	{
		Property mp;
		mp.name = name;
		mp.value = value;
		unsigned int index = static_cast<unsigned int>(m_materialProperties.size());
		m_materialProperties.push_back(mp);
		m_materialMapToVectorIndex.emplace(hash, index);
	}
	else
	{
		unsigned int index = findIt->second;
		if (index < static_cast<unsigned int>(m_materialProperties.size()))
		{
			m_materialProperties[index].value = value;
		}
	}
}

/*
	Support for only one texture per texture type
*/
void Material::BindTextures()
{
	m_shader.Use();
	const unsigned int textureSize = static_cast<unsigned int>(m_textures.size());
	for (unsigned int i = 0; i < textureSize; i++)
	{
		switch (m_textures[i].GetType())
		{
		case TextureType::DiffuseMap:
			glActiveTexture(GL_TEXTURE3);
			m_shader.SetInt("albedoMap", 3);
			break;
		case TextureType::NormalMap:
			glActiveTexture(GL_TEXTURE4);
			m_shader.SetInt("normalMap", 4);
			break;
		case TextureType::MetallicMap:
			glActiveTexture(GL_TEXTURE5);
			m_shader.SetInt("metallicMap", 5);
			break;
		case TextureType::RoughnessMap:
			glActiveTexture(GL_TEXTURE6);
			m_shader.SetInt("roughnessMap", 6);
			break;
		case TextureType::AOMap:
			glActiveTexture(GL_TEXTURE7);
			m_shader.SetInt("aoMap", 7);
			break;
		case TextureType::HeightMap:
			glActiveTexture(GL_TEXTURE8);
			m_shader.SetInt("heightMap", 8);
			break;
		case TextureType::SpecularMap:
			glActiveTexture(GL_TEXTURE9);
			m_shader.SetInt("specularMap", 9);
			break;
		default: break;
		}

		glBindTexture(GL_TEXTURE_2D, m_textures[i].GetId());
	}
}

void Material::AddTexture(TextureInfo texture)
{
	m_textures.push_back(texture);
}

void Material::AddTexturePath(TextureType type, const std::string& path)
{
	m_texturePathPerType[type].push_back(path);
}

void Material::AddTexturePaths(TextureType type, const std::vector<std::string>& paths)
{
	m_texturePathPerType[type] = paths;
}

