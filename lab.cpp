#include <bits/stdc++.h>
#include "ExtendibleHashing2.h"

using namespace std;
int main()
{
    try
    {
        ExtendibleHashing<int, 3> eh("test");
        eh.insert(1, 1);
        eh.insert(2, 2);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}