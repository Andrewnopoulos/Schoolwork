#include "Tutorial15.h"
#include <GLFW\glfw3.h>
#include <iostream>

void Tutorial15::Init()
{
	if (glfwInit() == false)
	{
		return;
	}

	for (auto& v : m_vectors)
	{
		v = vec4(1);
	}

	// get the first platform
	result = clGetPlatformIDs(1, &m_platform, nullptr);
	if (result != CL_SUCCESS)
		printf("clGetPlatformIDs failed: %i\n", result);	// get the first device on the platform (default is GPU)
	result = clGetDeviceIDs(m_platform, CL_DEVICE_TYPE_DEFAULT,
		1, &m_device, nullptr);
	if (result != CL_SUCCESS)
		printf("clGetDeviceIDs failed: %i\n", result);	// create a context for a device on the specified platform
	cl_context_properties contextProperties[] = {
		CL_CONTEXT_PLATFORM, (cl_context_properties)m_platform,
		0
	};
	m_context = clCreateContext(contextProperties, 1, &m_device,
		nullptr, nullptr, &result);
	if (result != CL_SUCCESS)
		printf("clCreateContext failed: %i\n", result);	// create a command queue to process commands
	m_queue = clCreateCommandQueue(m_context, m_device,
		CL_QUEUE_PROFILING_ENABLE, &result);
	if (result != CL_SUCCESS)
		printf("clCreateCommandQueue failed: %i\n", result);	LoadKernel("../data/kernels/tutorial15kernel.cl");	result = clBuildProgram(m_program, 1, &m_device,
		nullptr, nullptr, nullptr);
	if (result != CL_SUCCESS)
		printf("clBuildProgram failed: %i\n", result);	// extract the kernel
	m_kernel = clCreateKernel(m_program, "normalize_vec4", &result);
	if (result != CL_SUCCESS)
		printf("clCreateKernel failed: %i\n", result);	// create cl buffer for our data and copy it off the host (CPU)
	m_buffer = clCreateBuffer(m_context,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		sizeof(glm::vec4) * VECTOR_COUNT, m_vectors,
		&result);
	if (result != CL_SUCCESS)
		printf("clCreateBuffer failed: %i\n", result);
}

void Tutorial15::LoadKernel(char* filename)
{
	FILE* file = fopen(filename, "rb");
	if (file == nullptr)
		return;
	// read the program source
	fseek(file, 0, SEEK_END);
	unsigned int kernelSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	char* kernelSource = new char[kernelSize + 1];
	memset(kernelSource, 0, kernelSize + 1);
	fread(kernelSource, sizeof(char), kernelSize, file);
	fclose(file);

	// build program from our source using the device context
	m_program = clCreateProgramWithSource(m_context, 1,
		(const char**)&kernelSource, &kernelSize, &result);
	if (result != CL_SUCCESS)
		printf("clCreateProgramWithSource failed: %i\n", result);
}

void Tutorial15::Destroy()
{
	clReleaseMemObject(m_buffer);
	clReleaseKernel(m_kernel);
	clReleaseProgram(m_program);
	clReleaseCommandQueue(m_queue);
	clReleaseContext(m_context);
}

void Tutorial15::RunCPU()
{
	double startTime = glfwGetTime();

	for (auto& v : m_vectors)
		v = glm::normalize(v);
	double endTime = glfwGetTime();

	printf("CPU duration: %f\n", endTime - startTime);
}

void Tutorial15::RunGPU()
{

	// set the buffer as the first argument for our kernel
	result = clSetKernelArg(m_kernel, 0, sizeof(cl_mem), &m_buffer);
	if (result != CL_SUCCESS)
		printf("clSetKernelArg failed: %i\n", result);

	// process the kernel and give it an event ID
	cl_event eventID = 0;

	// we'll give it a size equal to the number of vec4's to process
	size_t globalWorkSize[] = { VECTOR_COUNT };

	result = clEnqueueNDRangeKernel(m_queue, m_kernel, 1,
		nullptr, globalWorkSize, nullptr,
		0, nullptr, &eventID);

	if (result != CL_SUCCESS)
		printf("clEnqueueNDRangeKernel failed: %i\n", result);

	// read back the processed data but wait for an event to complete
	result = clEnqueueReadBuffer(m_queue, m_buffer, CL_TRUE, 0,
		sizeof(glm::vec4) * VECTOR_COUNT, m_vectors,
		1, &eventID, nullptr);
	if (result != CL_SUCCESS)
		printf("clEnqueueReadBuffer failed: %i\n", result);
	// finish all opencl commands
	clFlush(m_queue);
	clFinish(m_queue);

	// how long did each event take?
	// get start / end profile data for the event
	cl_ulong clstartTime = 0;
	result = clGetEventProfilingInfo(eventID,
		CL_PROFILING_COMMAND_START,
		sizeof(cl_ulong), &clstartTime, nullptr);
	if (result != CL_SUCCESS)
		printf("clGetEventProfilingInfo failed: %i\n", result);
	cl_ulong clendTime = 0;
	result = clGetEventProfilingInfo(eventID,
		CL_PROFILING_COMMAND_END,
		sizeof(cl_ulong), &clendTime, nullptr);
	if (result != CL_SUCCESS)
		printf("clGetEventProfilingInfo failed: %i\n", result);
	// return time is in nanoseconds, so convert to seconds
	double clTime = (clendTime - clstartTime) * 1.0e-9;
	printf("GPU duration: %f\n", clTime);

}

void Tutorial15::Run()
{
	Init();
	RunCPU();
	RunGPU();
	Destroy();
}