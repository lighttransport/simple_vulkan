#pragma once
#include<cstdio>
#include<vector>
#include<string>

bool readBinaryFile(const std::string& fileName,std::vector<uint8_t>& result)
{
    long int size;
    size_t retval;

    std::FILE *fp = std::fopen(fileName.c_str(), "rb");
    if (!fp) return false;

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);

    result.resize(size);
    retval = std::fread(result.data(), size, 1,fp);

    return true;
}
