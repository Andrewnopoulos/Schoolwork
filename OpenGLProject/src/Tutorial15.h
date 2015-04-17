#pragma once

#include <glm\glm.hpp>
#include <CL\cl.h>

using glm::vec4;

static const int VECTOR_COUNT = 5000000;

class Tutorial15
{
private:
	vec4 m_vectors[VECTOR_COUNT];

	cl_platform_id m_platform;
	cl_device_id m_device;
	cl_context m_context;
	cl_command_queue m_queue;
	cl_program m_program;
	cl_kernel m_kernel;
	cl_mem m_buffer;

	cl_int result;

public:
	void Run();
	void Init();
	void Destroy();
	void RunCPU();
	void RunGPU();
	void LoadKernel(char* filename);
};