#ifndef _FILE_PACKER_HPP
#define _FILE_PACKER_HPP 1

#include <vector>
#include <string>

namespace jaja
{

namespace fp
{

struct FileDescription
{
    FileDescription() : m_Full_Path(), m_Full_Path_Size(0), m_Start_Pos(0), m_Size(0) {}
    std::string m_Full_Path;
    uint16_t m_Full_Path_Size;
    uint64_t m_Start_Pos;
    uint64_t m_Size;
};

struct File
{
    File() : m_Buffer(nullptr), m_Size(0) {}
    File(const File& rhs) = delete;
    File& operator=(const File& rhs) = delete;
    ~File()
    {
        if (m_Buffer != nullptr)
        {
            delete [] m_Buffer;
            m_Buffer = nullptr;
        }
    }
    char* m_Buffer;
    uint64_t m_Size;
};

class FilePacker
{
public:

    bool mt_Get_File(const std::string& file, File& data);
    const std::vector<FileDescription>& mt_Get_Files_Description(void) const;
    bool mt_Parse(const std::string& file);
    bool mt_Pack(const std::string& pack_file, const std::vector<std::string>& files);

private:
    std::string m_Parsed_File;
    std::vector<FileDescription> m_File_Description;

    void mt_Pack_List_Files(const std::vector<std::string>& files, std::vector<FileDescription>& file_description);
    uint64_t mt_Pack_Write_Header(std::ofstream& output, const std::vector<FileDescription>& file_description);
    uint64_t mt_Pack_Write_Files(std::ofstream& output, const std::vector<FileDescription>& file_description);


};

}

}



#endif // _FILE_PACKER_HPP
