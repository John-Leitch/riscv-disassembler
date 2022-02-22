#include <stdio.h>

#include "riscv-disas.h"

#define array_size(arr) (sizeof(arr) / sizeof(arr[0]))

uint64_t knownPCs[0x10000];
size_t knownPCCount = 0;

bool addPC(uint64_t pc)
{
    for (int i = 0; i < knownPCCount; i++)
    {
        if (knownPCs[i] == pc)
        {
            return false;
        }
    }

    if (knownPCCount == 0x10000)
    {
        knownPCCount = 0;
    }

    knownPCs[knownPCCount++] = pc;
    return true;
}

uint64_t findNextPC(uint64_t pc)
{
    uint64_t nextPC = UINT64_MAX;

    for (int i = 0; i < knownPCCount; i++)
    {
        if (knownPCs[i] > pc && knownPCs[i] < nextPC)
        {
            nextPC = knownPCs[i];
        }
    }

    return nextPC == UINT64_MAX ? 0 : nextPC;
}

void dissassemble(uint64_t pc, const uint8_t *data, size_t data_len)
{
    char buf[128] = { 0 };
    size_t offset = 0, inst_len;
    rv_inst inst;
    uint64_t nextPC = 0;
    while (offset < data_len)
    {
        uint64_t tmpPC = 0;
        uint64_t curPC = pc + offset;
        inst_fetch(data + offset, &inst, &inst_len);
        disasm_inst_rec(buf, sizeof(buf), rv64, curPC, inst, &tmpPC);

        if (tmpPC != 0)
        {
            addPC(tmpPC);
        }


        printf("0x%" PRIx64 ":  %s\n", pc + offset, buf);
        offset += inst_len;

        if (inst_len == 0)
        {
            nextPC = findNextPC(curPC);
            if (nextPC == 0)
            {
                break;
            }
            printf("################################################################\n");
            printf("# Continuing disassembly at known PC 0x%" PRIx64 "\n", nextPC);
            printf("################################################################\n");
            /*if (nextPC <= curPC)
            {
                break;
            }
            else
            {*/
                offset = nextPC - pc;
                /*nextPC = 0;
            }*/
        }
    }
}

void t1()
{
    static const uint8_t inst_arr[] = {
        //0x00, 0x00, // illegal
        //0x00, 0x01, // nop
        //0x00, 0x0d, // addi zero,zero,3
        //0x01, 0x04, // mv s0,s0
        //0x04, 0x04, // addi s1,sp,512
        //0x05, 0x40, // addi  s0,s0,1
        //0x73, 0x25, 0x40, 0xf1, // csrrs a0,mhartid,zero
        //0x97, 0x05, 0x00, 0x00, // auipc a1,0
        //0xb7, 0x02, 0x40, 0x20, // lui t0,541065216
        //0x13, 0x00, 0x00, 0x00, // nop

        0x21, 0xa8, 0xef, 0xbe, 0xad, 0xde, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x50, 0x30, 0x30, 0x73, 0x50, 0x20, 0x30, 0x73, 0x50, 0x40, 0x30, 0x73, 0x50, 0x40, 0x34, 0x97, 0x02, 0x00, 0x00, 0x93, 0x82, 0x82, 0x0f, 0x73, 0x90, 0x52, 0x30, 0x81, 0x40, 0x01, 0x41, 0x81, 0x41, 0x01, 0x42, 0x81, 0x42, 0x01, 0x43, 0x81, 0x43, 0x01, 0x44, 0x81, 0x44, 0x01, 0x45, 0x81, 0x45, 0x01, 0x46, 0x81, 0x46, 0x01, 0x47, 0x81, 0x47, 0x01, 0x48, 0x81, 0x48, 0x01, 0x49, 0x81, 0x49, 0x01, 0x4a, 0x81, 0x4a, 0x01, 0x4b, 0x81, 0x4b, 0x01, 0x4c, 0x81, 0x4c, 0x01, 0x4d, 0x81, 0x4d, 0x01, 0x4e, 0x81, 0x4e, 0x01, 0x4f, 0x81, 0x4f, 0x99, 0x62, 0x73, 0xa0, 0x02, 0x30, 0x73, 0x10, 0x30, 0x00, 0x53, 0x00, 0x00, 0xf0
    };

    FILE *binFile = fopen(
        //"C:\\source\\kendryte-flash-windows\\src\\Canaan.Kendryte.Flash\\Resources\\isp_flash.bin",
        "C:\\downloads\\LCD-display-code\\Program source code\\build\\hello_world.bin",
        "rb");

    fseek(binFile, 0, SEEK_END);          // Jump to the end of the file
    long filelen = ftell(binFile);             // Get the current byte offset in the file
    rewind(binFile);                      // Jump back to the beginning of the file

    char* buffer = (char*)malloc(filelen * sizeof(char)); // Enough memory for the file
    fread(buffer, filelen, 1, binFile); // Read in the entire file
    fclose(binFile); // Close the file
    dissassemble(
        0x80000000
        , buffer, filelen);

    //dissassemble(
    //    //0x10078
    //    0x80000000
    //    , inst_arr, array_size(inst_arr));
}

int main()
{
    t1();
}
