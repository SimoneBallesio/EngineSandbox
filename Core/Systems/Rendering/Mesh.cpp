#include "Mesh.h"

#include "Renderer/Shader.h"
#include "Vertex.h"

Mesh::Mesh()
	: m_VAO(0)
	, m_VBO(0)
	, m_EBO(0)
	, m_materialIndex(0)
	, m_isReady(false)
{
}

Mesh::Mesh(const Mesh& copy)
	: m_vertices(copy.m_vertices)
	, m_indices(copy.m_indices)
	, m_VAO(copy.m_VAO)
	, m_VBO(copy.m_VBO)
	, m_EBO(copy.m_EBO)
	, m_isReady(copy.m_isReady)
	, m_name(copy.m_name)
	, m_materialIndex(copy.m_materialIndex)
{
}

Mesh::Mesh(Mesh&& move)
	: m_vertices(move.m_vertices)
	, m_indices(move.m_indices)
	, m_VAO(move.m_VAO)
	, m_VBO(move.m_VBO)
	, m_EBO(move.m_EBO)
	, m_isReady(move.m_isReady)
	, m_name(move.m_name)
	, m_materialIndex(move.m_materialIndex)
{
}

Mesh& Mesh::operator=(const Mesh & assign)
{
	m_vertices = assign.m_vertices;
	m_indices = assign.m_indices;
	m_VAO = assign.m_VAO;
	m_VBO = assign.m_VBO;
	m_EBO = assign.m_EBO;
	m_isReady = assign.m_isReady;
	m_name = assign.m_name;
	m_materialIndex = assign.m_materialIndex;
	return *this;
}

Mesh::~Mesh()
{
	// Cleanup();
}

void Mesh::SetVertices(const std::vector<VertexInfo>& vertices)
{
	this->m_vertices = vertices;
}

void Mesh::SetIndices(const std::vector<unsigned int>& indices)
{
	this->m_indices = indices;
}

void Mesh::CreateBuffers()
{
	if (!m_isReady) 
	{
		// create buffers/arrays
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);
	}

	glBindVertexArray(m_VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

	// A great thing about structs is that their memory layout is sequential for all its items.
	// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
	// again translates to 3/2 floats which translates to a byte array.
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(VertexInfo), &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0); // layout (location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1); // layout (location = 1)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), (void*)offsetof(VertexInfo, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2); // layout (location = 2)
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), (void*)offsetof(VertexInfo, TexCoords));
	// vertex tangent
	glEnableVertexAttribArray(3); // layout (location = 3)
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), (void*)offsetof(VertexInfo, Tangent));
	// vertex bitangent
	glEnableVertexAttribArray(4); // layout (location = 4)
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexInfo), (void*)offsetof(VertexInfo, Bitangent));

	glBindVertexArray(0);

	m_isReady = m_VAO != 0;
}

void Mesh::Cleanup()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
}

void Mesh::UpdateBounds()
{
	glm::vec3 min = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	glm::vec3 max = { FLT_MAX, FLT_MAX, FLT_MAX };

	for (VertexInfo vi : m_vertices)
	{
		if ( vi.Position.x < min.x && vi.Position.y < min.y && vi.Position.z < min.z )
		{
			min = vi.Position;
		}

		if (vi.Position.x > max.x && vi.Position.y > max.y && vi.Position.z > max.z)
		{
			max = vi.Position;
		}
	}

	m_aabb.SetBounds(min, max);
}

void Mesh::Draw(unsigned int instanceCount /*= 1*/) const
{
	// draw mesh
	glBindVertexArray(m_VAO);

	if (instanceCount > 1)
	{
		glDrawElementsInstanced(GL_TRIANGLES,
			static_cast<GLsizei>(m_indices.size()),
			GL_UNSIGNED_INT, 0, instanceCount);
	}
	else
	{
		glDrawElements(GL_TRIANGLES,
			static_cast<GLsizei>(m_indices.size()),
			GL_UNSIGNED_INT, 0);
	}
	glBindVertexArray(0);

	// always good practice to set everything back to defaults once configured.
	glActiveTexture(GL_TEXTURE0);
}
