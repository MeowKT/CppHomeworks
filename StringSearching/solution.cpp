#include <cstdlib>
#include <iostream>
#include <iterator>
#include <cstring>

#include <fcntl.h>
#include <unistd.h>

void prefFunc(const char* arr, int* pref, size_t len)
{
    for (size_t i = 1; i < len; i++)
    {
        size_t j = pref[i - 1];
        while (j > 0 && arr[i] != arr[j])
            j = pref[j - 1];
        if (arr[i] == arr[j])
        {
            j++;
        }
        pref[i] = j;
    }
}

bool checkSubstr(const int* pref, size_t len, size_t exampleLen)
{
    for (size_t i = 0; i < len; i++)
    {
        if (pref[i] == exampleLen)
        {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char* argv[])
{

    if (argc != 3)
    {
        std::cerr << "usage: " << argv[0] << " <filename>\n";
        return EXIT_FAILURE;
    }

    FILE* fd = fopen(argv[2], "r");

    if (!fd)
    {
        perror("open failed");
        return EXIT_FAILURE;
    }

    size_t longestPrefix = 0;
    size_t len = std::strlen(argv[1]);
    for (;;)
    {
        size_t szBuf = 1; // for all
        char* buffer = new char[szBuf + len + 1 + longestPrefix];
        int* pref = new int[szBuf + len + 1 + longestPrefix];
        memset(pref, 0, (szBuf + len + 1 + longestPrefix) * sizeof(int));

        std::memcpy(buffer, argv[1], len * sizeof(char));
        buffer[len] = '$';

        std::memcpy(buffer + len + 1, argv[1], longestPrefix * sizeof(char));
        ssize_t bytes_read = fread(buffer + longestPrefix + len + 1, sizeof (char), szBuf, fd);

        if (bytes_read < 0)
        {
            perror("read failed");
            fclose(fd);
            return EXIT_FAILURE;
        }

        if (bytes_read == 0)
            break;

        prefFunc(buffer, pref, len + szBuf + longestPrefix + 1);

        if (checkSubstr(pref, len + szBuf + longestPrefix + 1, len))
        {
            std::cout << "TRUE";
            fclose(fd);
            return 0;
        }

        longestPrefix = pref[len + szBuf + longestPrefix];
    }
    std::cout << "FALSE";
    fclose(fd);
    return EXIT_SUCCESS;
}