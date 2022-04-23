#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
using namespace std;  //使用std命名空间 std::cout --> cout

void usage_exit(char *prog_name)
{
    char *short_name;
    char *extension;

    short_name = strrchr(prog_name, '\\');
    if (short_name == NULL)
        short_name = strrchr(prog_name, '/');
    if (short_name == NULL)
        short_name = strrchr(prog_name, ':');
    if (short_name != NULL)
        short_name++;
    else
        short_name = prog_name;
    extension = strrchr(short_name, '.');
    if (extension != NULL)
        *extension = '\0';
    printf("\nUsage:  %s in-file\n", short_name);
    exit(0);
}

#ifndef SEEK_END
#define SEEK_END 2
#endif

long file_size(char *name)
{
    long eof_ftell;
    FILE *file;

    file = fopen(name, "r");
    if (file == NULL)
        return (0L);
    fseek(file, 0L, SEEK_END);
    eof_ftell = ftell(file);
    fclose(file);
    return (eof_ftell);
}

int get_percent(char *input, char *output)
{
    long input_size;
    long output_size;

    input_size = file_size(input);
    if (input_size == 0)
        input_size = 1;
    output_size = file_size(output);
    return (100 - (int)(output_size * 100L / input_size));
}
/* 
 * 修饰的变量和函数是按照C语言方式进行编译和链接的
 */
extern "C"
{
    typedef struct bit_file
    {
        FILE *file;
        unsigned char mask;
        int rack;
        int pacifier_counter;
    } BIT_FILE;

    void fatal_error(char *fmt, ...);

    int CompressFile_adaptive_huffman(FILE *input, BIT_FILE *output);
    int ExpandFile_adaptive_huffman(BIT_FILE *input, FILE *output);

    int CompressFile_huffman(FILE *input, BIT_FILE *output);
    int ExpandFile_huffman(BIT_FILE *input, FILE *output);

    int CompressFile_fixed_order_0_arithmetic(FILE *input, BIT_FILE *output);
    int ExpandFile_fixed_order_0_arithmetic(BIT_FILE *input, FILE *output);

    int CompressFile_adaptive_order_1_arithmetic(FILE *input, BIT_FILE *output);
    int ExpandFile_adaptive_order_1_arithmetic(BIT_FILE *input, FILE *output);

    int CompressFile_lzss_encoding(FILE *input, BIT_FILE *output);
    int ExpandFile_lzss_encoding(BIT_FILE *input, FILE *output);

    int CompressFile_lzw_12_bit_encoder(FILE *input, BIT_FILE *output);
    int ExpandFile_lzw_12_bit_encoder(BIT_FILE *input, FILE *output);

    int CompressFile_lzw_15_bit_variable_rate_encoder(FILE *input, BIT_FILE *output);
    int ExpandFile_lzw_15_bit_variable_rate_encoder(BIT_FILE *input, FILE *output);

    BIT_FILE *OpenInputBitFile(char *name);
    BIT_FILE *OpenOutputBitFile(char *name);
    void CloseInputBitFile(BIT_FILE *bit_file);
    void CloseOutputBitFile(BIT_FILE *bit_file);
}

int main(int argc, char *argv[])
{
    BIT_FILE *output, *input2;
    FILE *input, *output2;

    static char szTemp1[MAX_PATH + 1], szTemp2[MAX_PATH + 1];

    /* setbuf(stdout, buf) 所有写入到stdout的输出都应该使用buf作为输出缓冲区 */
    setbuf(stdout, NULL);  //NULL为无缓冲
    if (argc < 2)
        usage_exit(argv[0]);

    /* 
     * 为临时文件创建名称 如果生成了唯一的文件名 则创建一个空文件并释放该文件的句柄；否则只生成文件名 
     * 文件名的目录路径 前缀字符串 创建临时文件名的无符号整数 接受临时文件名的缓冲区指针
     */
    GetTempFileNameA(".", "bit", 0, szTemp1);
    GetTempFileNameA(".", "out", 0, szTemp2);

    //setw()空格补全
    cout << setw(33) << "algorithm" << setw(9) << "compress" << setw(6) << "ratio" << setw(11) << "uncompress"
         << "\r\n-----------------------------------------------------------\r\n";

    // Adaptive Huffman coding
    input = fopen(argv[1], "rb");
    if (input == NULL)
        fatal_error("Error opening %s for input\n", argv[1]);
    output = OpenOutputBitFile(szTemp1);
    if (output == NULL)
        fatal_error("Error opening %s for output\n", szTemp1);
    cout << setw(33) << "Adaptive Huffman";
    cout << setw(7) << CompressFile_adaptive_huffman(input, output) << "ms";
    CloseOutputBitFile(output);
    fclose(input);
    cout << setw(5) << get_percent(argv[1], szTemp1) << "%";
    input2 = OpenInputBitFile(szTemp1);
    output2 = fopen(szTemp2, "wb");
    cout << setw(9) << ExpandFile_adaptive_huffman(input2, output2) << "ms" << endl;
    fclose(output2);
    CloseInputBitFile(input2);
    DeleteFileA(szTemp1);
    DeleteFileA(szTemp2);

    // Huffman coding
    input = fopen(argv[1], "rb");
    if (input == NULL)
        fatal_error("Error opening %s for input\n", argv[1]);
    output = OpenOutputBitFile(szTemp1);
    if (output == NULL)
        fatal_error("Error opening %s for output\n", szTemp1);
    cout << setw(33) << "Huffman";
    cout << setw(7) << CompressFile_huffman(input, output) << "ms";
    CloseOutputBitFile(output);
    fclose(input);
    cout << setw(5) << get_percent(argv[1], szTemp1) << "%";
    input2 = OpenInputBitFile(szTemp1);
    output2 = fopen(szTemp2, "wb");
    cout << setw(9) << ExpandFile_huffman(input2, output2) << "ms" << endl;
    fclose(output2);
    CloseInputBitFile(input2);
    DeleteFileA(szTemp1);
    DeleteFileA(szTemp2);

    // LZSS Encoding
    input = fopen(argv[1], "rb");
    if (input == NULL)
        fatal_error("Error opening %s for input\n", argv[1]);
    output = OpenOutputBitFile(szTemp1);
    if (output == NULL)
        fatal_error("Error opening %s for output\n", szTemp1);
    cout << setw(33) << "LZSS Encoding";
    cout << setw(7) << CompressFile_lzss_encoding(input, output) << "ms";
    CloseOutputBitFile(output);
    fclose(input);
    cout << setw(5) << get_percent(argv[1], szTemp1) << "%";
    input2 = OpenInputBitFile(szTemp1);
    output2 = fopen(szTemp2, "wb");
    cout << setw(9) << ExpandFile_lzss_encoding(input2, output2) << "ms" << endl;
    fclose(output2);
    CloseInputBitFile(input2);
    DeleteFileA(szTemp1);
    DeleteFileA(szTemp2);

    // LZW 12-bit Encoder
    input = fopen(argv[1], "rb");
    if (input == NULL)
        fatal_error("Error opening %s for input\n", argv[1]);
    output = OpenOutputBitFile(szTemp1);
    if (output == NULL)
        fatal_error("Error opening %s for output\n", szTemp1);
    cout << setw(33) << "LZW 12-bit Encoder";
    cout << setw(7) << CompressFile_lzw_12_bit_encoder(input, output) << "ms";
    CloseOutputBitFile(output);
    fclose(input);
    cout << setw(5) << get_percent(argv[1], szTemp1) << "%";
    input2 = OpenInputBitFile(szTemp1);
    output2 = fopen(szTemp2, "wb");
    cout << setw(9) << ExpandFile_lzw_12_bit_encoder(input2, output2) << "ms" << endl;
    fclose(output2);
    CloseInputBitFile(input2);
    DeleteFileA(szTemp1);
    DeleteFileA(szTemp2);

    // LZW 15-bit Variable Rate Encoding
    input = fopen(argv[1], "rb");
    if (input == NULL)
        fatal_error("Error opening %s for input\n", argv[1]);
    output = OpenOutputBitFile(szTemp1);
    if (output == NULL)
        fatal_error("Error opening %s for output\n", szTemp1);
    cout << setw(33) << "LZW 15-bit Variable Rate Encoding";
    cout << setw(7) << CompressFile_lzw_15_bit_variable_rate_encoder(input, output) << "ms";
    CloseOutputBitFile(output);
    fclose(input);
    cout << setw(5) << get_percent(argv[1], szTemp1) << "%";
    input2 = OpenInputBitFile(szTemp1);
    output2 = fopen(szTemp2, "wb");
    cout << setw(9) << ExpandFile_lzw_15_bit_variable_rate_encoder(input2, output2) << "ms" << endl;
    fclose(output2);
    CloseInputBitFile(input2);
    DeleteFileA(szTemp1);
    DeleteFileA(szTemp2);

    // Fixed Order 0 Arithmetic
    input = fopen(argv[1], "rb");
    if (input == NULL)
        fatal_error("Error opening %s for input\n", argv[1]);
    output = OpenOutputBitFile(szTemp1);
    if (output == NULL)
        fatal_error("Error opening %s for output\n", szTemp1);
    cout << setw(33) << "Fixed Order 0 Arithmetic";
    cout << setw(7) << CompressFile_fixed_order_0_arithmetic(input, output) << "ms";
    CloseOutputBitFile(output);
    fclose(input);
    cout << setw(5) << get_percent(argv[1], szTemp1) << "%";
    input2 = OpenInputBitFile(szTemp1);
    output2 = fopen(szTemp2, "wb");
    cout << setw(9) << ExpandFile_fixed_order_0_arithmetic(input2, output2) << "ms" << endl;
    fclose(output2);
    CloseInputBitFile(input2);
    DeleteFileA(szTemp1);
    DeleteFileA(szTemp2);

    // Adaptive Order 1 Arithmetic
    input = fopen(argv[1], "rb");
    if (input == NULL)
        fatal_error("Error opening %s for input\n", argv[1]);
    output = OpenOutputBitFile(szTemp1);
    if (output == NULL)
        fatal_error("Error opening %s for output\n", szTemp1);
    cout << setw(33) << "Adaptive Order 1 Arithmetic";
    cout << setw(7) << CompressFile_adaptive_order_1_arithmetic(input, output) << "ms";
    CloseOutputBitFile(output);
    fclose(input);
    cout << setw(5) << get_percent(argv[1], szTemp1) << "%";
    input2 = OpenInputBitFile(szTemp1);
    output2 = fopen(szTemp2, "wb");
    cout << setw(9) << ExpandFile_adaptive_order_1_arithmetic(input2, output2) << "ms" << endl;
    fclose(output2);
    CloseInputBitFile(input2);
    DeleteFileA(szTemp1);
    DeleteFileA(szTemp2);

    return (0);
}