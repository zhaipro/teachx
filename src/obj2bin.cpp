#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <windows.h>
#include <winnt.h>

int main(int argc, const char *args[])
{
    assert(argc >= 4 && "obj2bin <ifn> <ofn> <ttext>");
    FILE *ifp = fopen(args[1], "rb");
    FILE *ofp = fopen(args[2], "wb");
    int ttext = strtol(args[3], NULL, 16);

    // https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-image_file_header
    IMAGE_FILE_HEADER header;
    fread(&header, sizeof(header), 1, ifp);
    assert(header.Machine == IMAGE_FILE_MACHINE_I386 && "x86?");
    fseek(ifp, header.SizeOfOptionalHeader, SEEK_CUR);

    // https://docs.microsoft.com/en-us/windows/win32/api/winnt/ns-winnt-image_section_header
    size_t size = sizeof(IMAGE_SECTION_HEADER) * header.NumberOfSections;
    IMAGE_SECTION_HEADER *sections = (IMAGE_SECTION_HEADER*)malloc(size);
    fread(sections, size, 1, ifp);
    unsigned long address = 0;
    for(int i=0; i<header.NumberOfSections; i++)
    {
        assert(address % 4 == 0);
        sections[i].VirtualAddress = address;
        address += sections[i].SizeOfRawData;
    }

    // https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#coff-symbol-table
    size = sizeof(IMAGE_SYMBOL) * header.NumberOfSymbols;
    IMAGE_SYMBOL *symbols = (IMAGE_SYMBOL*)malloc(size);
    fseek(ifp, header.PointerToSymbolTable, SEEK_SET);
    fread(symbols, size, 1, ifp);
    for(int i=0; i<header.NumberOfSymbols; i++)
    {
        if(symbols[i].SectionNumber <= header.NumberOfSections) {
            symbols[i].Value += sections[symbols[i].SectionNumber - 1].VirtualAddress;
        }
    }

    // sections
    char *buf = (char*)malloc(4 * 1024);
    for(int i=0; i<header.NumberOfSections; i++)
    {
        assert(sections[i].SizeOfRawData <= 4 * 1024);
        fseek(ifp, sections[i].PointerToRawData, SEEK_SET);
        fread(buf, sections[i].SizeOfRawData, 1, ifp);
        // https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#coff-relocations-object-only
        IMAGE_RELOCATION relocation;
        fseek(ifp, sections[i].PointerToRelocations, SEEK_SET);
        for(int j=0; j<sections[i].NumberOfRelocations; j++)
        {
            fread(&relocation, sizeof(relocation), 1, ifp);
            if(relocation.Type == 0x14) {   // 函数调用
                *(long*)(&buf[relocation.VirtualAddress]) = symbols[relocation.SymbolTableIndex].Value - relocation.VirtualAddress - 4;
            } else if(relocation.Type == 6) {
                *(long*)(&buf[relocation.VirtualAddress]) += ttext + symbols[relocation.SymbolTableIndex].Value;
            } else {
                assert(!"unknown type");
            }
        }
        fwrite(buf, 1, sections[i].SizeOfRawData, ofp);
    }

    // 释放资源的活就交给操作系统了
    fclose(ofp);
    return 0;
}
