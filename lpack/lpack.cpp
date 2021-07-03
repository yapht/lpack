#include <iostream>
#include "compiler.h"
#include <Windows.h>
#include <filesystem>

namespace fs = std::filesystem;

#define IDR_CODE1                       101
bool pack_bytecode(std::string stub_path, void* bytecode, uint32_t size)
{
    HANDLE update_handle = BeginUpdateResourceA(stub_path.c_str(), false);
    if(update_handle == NULL)
    {
        std::cout << "Failed to open file for writing.\n";
        return false;
    }

    // Update the bytecode resource in the stub executable
    BOOL status =
    UpdateResource(update_handle,
        RT_RCDATA,
        MAKEINTRESOURCE(IDR_CODE1),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
        bytecode,
        size);

    if (status == FALSE)
    {
        std::cout << "Failed to update resource.\n";
        return false;
    }

    if (!EndUpdateResourceA(update_handle, FALSE))
    {
        std::cout << "Could not write changes to file.\n";
        return false;
    }

    return status;
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        std::cout << "Usage: lpack.exe in.lua out.exe\n";
        return 1;
    }

    if (!fs::exists("stub.exe"))
    {
        std::cout << "Failed to find stub.exe\n";
        return 1;
    }

    std::string input_path = argv[1];
    std::string output_path = argv[2];

    if (output_path == "stub.exe")
    {
        std::cout << "Cannot output as 'stub.exe'\n";
        return 1;
    }

    if (!fs::exists(input_path))
    {
        std::cout << "File doesn't exist: " << input_path << "\n";
        return 1;
    }

    if (fs::exists(output_path))
    {
        std::cout << "Output file already exists. Removing.\n";
        fs::remove(output_path);
    }

    std::cout << "Compiling...\n";

    std::unique_ptr<uint8_t> buffer(new uint8_t[65535]);

    compiler::bcbuffer bcbuffer;
    bcbuffer.buf = buffer.get();
    bcbuffer.capacity = 65535;
    bcbuffer.size = 0;

    if (!compiler::compile_bytecode(input_path, &bcbuffer, false))
    {
        std::cout << "Failed to compile bytecode.\n";
        return 1;
    }

    if (!fs::copy_file("stub.exe", output_path))
    {
        std::cout << "Failed to make stub copy.\n";
        return 1;
    }

    std::cout << "Packing...\n";

    if (!pack_bytecode(output_path, bcbuffer.buf, bcbuffer.size))
    {
        (void)fs::remove(output_path);
        std::cout << "Failed to pack output file!\n";
        return 1;
    }

    std::cout << "Done!\nOutput: " << output_path << "\n";
    return 0;
}