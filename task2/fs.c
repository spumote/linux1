#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define c_maxStringSize 256
#define c_maxDepth 30
#define c_offsetNodesStart 4096
#define c_offsetSegmentsStart 524288
#define c_segmentSize 4096
#define c_nodeSize 1024

char curPwd[c_maxDepth][c_maxStringSize];
int pwdSize;
int curNode;

void pwd()
{
    for (size_t i = 0; i < pwdSize; i++)
    {
        printf("/");
        for (size_t j = 0; j < strlen(curPwd[i]); j++)
            printf("%c", curPwd[i][j]);
    }
    if (pwdSize == 0)
        printf("/");
    printf("\n");
}

long count_node_offset(const unsigned nodeNumber)
{
    return c_offsetNodesStart + (long)nodeNumber * c_nodeSize;
}

long count_segment_offset(const unsigned segmentNumber)
{
    return c_offsetSegmentStart + (long)segmentNumber * c_segmentSize;
}

void make_ls_seg(long offset)
{
    unsigned node = read_u(offset);
    char* buf = read_string(offset);
}

void make_ls()
{
    long curOffset = count_node_offset(curNode);
    unsigned number = read_u(curOffset);
    for (size_t i = 0; i < number; i++)
    {
        unsigned curSegment = read_u(curOffset);
        make_ls_seg(count_segment_offset(curSegment));
    }
}

int main()
{
    char* curLine = calloc(c_maxStringSize, sizeof(char));
    pwdSize = 0;
    curNode = 0;
    while (fgets(curLine, sizeof(char) * c_maxStringSize, stdin))
    {
        //delete \n
        if (strlen(curLine) > 0)
            curLine[strlen(curLine) - 1] = '\0';
        if (strcmp(curLine, "pwd") == 0)
        {
            pwd();
            continue;
        }
        if (strstr(curLine, "cd ") == curLine)
        {
            if (strstr(curLine + 3, "..") == curLine + 3)
            {
                if (pwdSize > 0)
                    pwdSize--;
            }
            else
            {
                strcpy(curPwd[pwdSize], curLine + 3);
                pwdSize++;
            }
            continue;
        }
        if (strcmp(curLine, "ls") == 0)
        {
            make_ls(); 
            continue;
        }
        printf("No command: \"%s\"\n", curLine);
    }
    free(curLine);
    return 0;
}
