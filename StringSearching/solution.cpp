#include <cstdlib>
#include <iostream>
#include <iterator>

#include <fcntl.h>
#include <unistd.h>

void prefFunc(const char* arr, int* z, size_t len)
{
    for (size_t i = 1; i < len; i++)
    {
        size_t j = z[i - 1];
        while (j > 0 && arr[i] != arr[j])
            j = z[j - 1];
        if (arr[i] == arr[j])
        {
            j++;
        }
        z[i] = j;
    }
}

bool checkSubstr(const int* z, size_t len, int exampleLen)
{
    for (size_t i = 0; i < len; i++)
    {
        if (z[i] == exampleLen)
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

    int fd = open(argv[2], O_RDONLY | O_CLOEXEC);
    if (fd < 0)
    {
        perror("open failed");
        return EXIT_FAILURE;
    }

    __int32_t longestPrefix = 0;
    __int32_t len = std::strlen(argv[1]);
    for (;;)
    {
        __int32_t szBuf = 1; // for all
        char* buffer = new char[szBuf + len + 1 + longestPrefix];
        int* z = new int[szBuf + len + 1 + longestPrefix];
        memset(z, 0, (szBuf + len + 1 + longestPrefix) * sizeof(int));

        std::memcpy(buffer, argv[1], len * sizeof(char));
        buffer[len] = '$';

        std::memcpy(buffer + len + 1, argv[1], longestPrefix * sizeof(char));
        ssize_t bytes_read = read(fd, buffer + longestPrefix + len + 1, szBuf);

        if (bytes_read < 0)
        {
            perror("read failed");
            close(fd);
            return EXIT_FAILURE;
        }

        if (bytes_read == 0)
            break;

        prefFunc(buffer, z, len + szBuf + longestPrefix + 1);

        if (checkSubstr(z, len + szBuf + longestPrefix + 1, len))
        {
            std::cout << "TRUE";
            close(fd);
            return 0;
        }

        longestPrefix = z[len + szBuf + longestPrefix];
    }
    std::cout << "FALSE";
    close(fd);
    return EXIT_SUCCESS;
}