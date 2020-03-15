#include "FilePacker.hpp"

#include <iomanip>
#include <algorithm>
#include <fstream>
#include <iostream>

namespace jaja
{

namespace fp
{

bool FilePacker::mt_Get_File(const std::string& file, File& data)
{
    bool l_Ret(false);
    std::ifstream l_Input;

    l_Input.open(m_Parsed_File, std::ios::binary | std::ios::in);

    if (l_Input.is_open())
    {
        auto l_it = std::find_if(m_File_Description.begin(), m_File_Description.end(), [&](const FileDescription& fd){return fd.m_Full_Path == file;});

        if (l_it != m_File_Description.end())
        {
            l_Ret = true;

            l_Input.seekg(l_it->m_Start_Pos, std::ios::beg);
            data.m_Size = l_it->m_Size;
            data.m_Buffer = new char[data.m_Size];
            l_Input.read(data.m_Buffer, data.m_Size);
        }
        else
        {
            std::cerr << "No file \"" <<file << "\" recorded\n";
        }
    }
    else
    {
        std::cerr << "Can't open \"" << m_Parsed_File << "\"\n";
    }

    return l_Ret;
}

const std::vector<FileDescription>& FilePacker::mt_Get_Files_Description(void) const
{
    return m_File_Description;
}

bool FilePacker::mt_Parse(const std::string& file)
{
    bool l_b_Ret;
    std::ifstream l_Input;

    l_Input.open(file, std::ios::binary | std::ios::in);

    if (l_Input.is_open())
    {
        char l_Header[4];

        m_Parsed_File = file;

        l_Input.read(l_Header, 4);

        if ((l_Header[0] == 'J') && (l_Header[1] == 'a') && (l_Header[2] == 'J') && (l_Header[3] == 'a'))
        {
            uint16_t l_File_Count;

            l_Input.read((char*)&l_File_Count, sizeof(l_File_Count));
            std::cerr << "File count: " << l_File_Count << '\n';

            m_File_Description.resize(l_File_Count);

            for (uint16_t ii = 0; ii < l_File_Count; ii++)
            {
                uint16_t l_File_Name_Size;

                l_Input.read((char*)&l_File_Name_Size, sizeof(l_File_Name_Size));

                m_File_Description[ii].m_Full_Path.resize(l_File_Name_Size);
                l_Input.read(&m_File_Description[ii].m_Full_Path[0], l_File_Name_Size);

                l_Input.read((char*)&m_File_Description[ii].m_Size, sizeof(m_File_Description[ii].m_Size));
            }

            for (std::size_t ii = 0; ii < m_File_Description.size(); ii++)
            {
                m_File_Description[ii].m_Start_Pos = l_Input.tellg();
                l_Input.seekg(m_File_Description[ii].m_Size, std::ios::cur);
            }
            std::cerr << "Parsed File: \"" << m_Parsed_File << "\"\n";
        }

        l_Input.close();

        l_b_Ret = true;
    }
    else
    {
        l_b_Ret = false;
    }

    return l_b_Ret;
}

bool FilePacker::mt_Pack(const std::string& pack_file, const std::vector<std::string>& files)
{
    bool l_b_Ret(false);
    std::ofstream l_Output;

    l_Output.open(pack_file);

    mt_Pack_List_Files(files, m_File_Description);

    mt_Pack_Write_Header(l_Output, m_File_Description);

    mt_Pack_Write_Files(l_Output, m_File_Description);

    //mt_Pack_Write_Compute_Checksum()

    l_Output.close();

    l_b_Ret = true;

    return l_b_Ret;
}





void FilePacker::mt_Pack_List_Files(const std::vector<std::string>& files, std::vector<FileDescription>& file_description)
{
    std::ifstream l_Input;

    file_description.resize(files.size());

    for (std::size_t ii = 0; ii < files.size(); ii++)
    {
        file_description[ii].m_Full_Path = files[ii];
        file_description[ii].m_Full_Path_Size = file_description[ii].m_Full_Path.size();

        l_Input.open(files[ii]);

        l_Input.seekg(0, std::ios::end);
        file_description[ii].m_Size = l_Input.tellg();

        l_Input.close();

        std::cerr << file_description[ii].m_Full_Path << " : " << file_description[ii].m_Size << '\n';
    }
}

uint64_t FilePacker::mt_Pack_Write_Header(std::ofstream& output, const std::vector<FileDescription>& file_description)
{
    uint16_t l_File_Count = file_description.size();
    uint64_t l_Current_Pos = 0;

    output.write("JaJa", 4);
    output.write((char*)&l_File_Count, sizeof(l_File_Count));
    l_Current_Pos = 4 + l_File_Count;

    for (std::size_t ii = 0; ii < file_description.size(); ii++)
    {
        uint16_t l_File_Name_Size = file_description[ii].m_Full_Path.size();

        output.write((char*)&l_File_Name_Size, sizeof(uint16_t));
        output.write(file_description[ii].m_Full_Path.c_str(), l_File_Name_Size);
        output.write((char*)&file_description[ii].m_Size, sizeof(uint64_t));
        l_Current_Pos += sizeof(uint64_t) + l_File_Name_Size + sizeof(uint64_t);
    }

    return l_Current_Pos;
}

uint64_t FilePacker::mt_Pack_Write_Files(std::ofstream& output, const std::vector<FileDescription>& file_description)
{
    uint64_t l_Written_Bytes;
    std::ifstream l_Input;

    l_Written_Bytes = 0;

    for (std::size_t ii = 0; ii < file_description.size(); ii++)
    {
        l_Input.open(file_description[ii].m_Full_Path);

        l_Written_Bytes += file_description[ii].m_Size;
        output << l_Input.rdbuf();

        l_Input.close();
    }

    return l_Written_Bytes;
}

}

}
