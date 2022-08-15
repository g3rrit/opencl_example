#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#define CL_HPP_TARGET_OPENCL_VERSION 200
#include <CL/opencl.hpp>

#define XSTR(s) STR(s)
#define STR(s) #s


std::string read_file(char const * const path) {

    std::ifstream t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();

    return buffer.str();
}

int main() {

    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    if (all_platforms.size() == 0) {
        std::cout << "No OpenCL platforms found.\n";
        exit(1);
    }

    cl::Platform default_platform = all_platforms[0];
    std::cout << "Using platform: " <<default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

    std::vector<cl::Device> all_devices;
    default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
    if (all_devices.size() == 0) {
        std::cout << "No devices found.\n";
        exit(1);
    }

    cl::Device default_device = all_devices[0];
    std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";

    cl::Context context({default_device});

    size_t const vector_size = 10;

    int const A_h[vector_size] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    int const B_h[vector_size] = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };

    cl::Buffer A_d(context, CL_MEM_READ_ONLY, sizeof(int) * vector_size);
    cl::Buffer B_d(context, CL_MEM_READ_ONLY, sizeof(int) * vector_size);
    cl::Buffer C_d(context, CL_MEM_WRITE_ONLY, sizeof(int) * vector_size);

    cl::CommandQueue queue(context, default_device);

    queue.enqueueWriteBuffer(A_d, CL_TRUE, 0, sizeof(int) * vector_size, A_h);
    queue.enqueueWriteBuffer(B_d, CL_TRUE, 0, sizeof(int) * vector_size, B_h);

    std::string kernel_code = read_file(XSTR(KERNEL_FILE_PATH));

    cl::Program::Sources sources;
    sources.push_back({ kernel_code.c_str(),kernel_code.length() });

    cl::Program program(context, sources);

    if (program.build({ default_device }) != CL_SUCCESS) {
        std::cout << "Error building: " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
        exit(1);
    }

    cl::compatibility::make_kernel<cl::Buffer, cl::Buffer, cl::Buffer> simple_add(cl::Kernel(program, "simple_add"));
    cl::NDRange global(vector_size);
    simple_add(cl::EnqueueArgs(queue, global), A_d, B_d, C_d).wait();

    int C_h[vector_size];
    queue.enqueueReadBuffer(C_d, CL_TRUE, 0, sizeof(int) * vector_size, C_h);

    std::cout << "Result: \n";
    for (int i = 0; i<10; i++) {
        std::cout << C_h[i] << " ";
    }

    return 0;
}
