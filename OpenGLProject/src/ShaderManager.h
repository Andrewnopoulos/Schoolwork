
#include <string>
#include <map>
#include <glm\glm.hpp>

class ShaderManager
{
public:
	ShaderManager();
	~ShaderManager();

	unsigned int GetShader(std::string& a_ShaderName);

	unsigned int LoadShader(std::string a_ShaderName,
		const char* a_VertexShader,
		const char* a_PixelShader,
		const char* a_GeometryShader = nullptr,
		const char* a_TesselationControlShader = nullptr,
		const char* a_TesselationEvalShader = nullptr);

	unsigned int GetErrorShader();

	void SetProjectionMatrix(glm::mat4 a_ProjMat);
	void SetViewMatrix(glm::mat4 a_ViewMat);
	void SetEyePosition(glm::vec4 a_EyePos);

private:

	bool DoesShaderExist(std::string& a_MaterialName);
	unsigned int LoadShader(const char* a_Name, unsigned int a_Type);

	//default shaders
	void CreateErrorShader();
	unsigned char* FileToBuffer(const char* a_Name);
	unsigned int CreateProgram(unsigned int, unsigned int tcs, unsigned int tes, unsigned int gs, unsigned int fs);
	std::map<std::string, unsigned int> m_shaderMap;

	unsigned int m_ErrorShader;
};