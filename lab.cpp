#include <bits/stdc++.h>
#include "CopyBucketNotLeaf.h"

using namespace std;
int main()
{
    try
    {
        BucketNotLeaf<string> leaf;
        //leaf.add_element("Alexander", 1);
        //leaf.add_element("Bob", 2);
        leaf.read_from_file("test2.dat", 0);
        leaf.show();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}