#include <cstdlib>
#include <iostream>
#include <iterator>
#include <cstring>

void prefFunc(const char* arr, int32_t* pref, size_t len)
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

void addToPref(const char* arr, int32_t& cur, const int32_t *pref, char last)
{
    while (cur > 0 && last != arr[cur])
    {
        cur = pref[cur - 1];
    }
    if (last == arr[cur])
    {
        cur++;
    }
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

    size_t len = strlen(argv[1]);

    int32_t* pref = new int32_t[len];
    memset(pref, 0, len * sizeof(int32_t));

    prefFunc(argv[1], pref, len);
    int32_t cur = 0;

    char c;
    while (c = getc(fd), c != EOF) {
        addToPref(argv[1], cur, pref, c);
        if (cur == len) {
            std::cout << "TRUE";
            fclose(fd);
            return EXIT_SUCCESS;
        }
    }

    std::cout << "FALSE";
    fclose(fd);
    return EXIT_SUCCESS;
}