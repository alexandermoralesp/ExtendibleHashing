#include <bits/stdc++.h>
#include "BucketNotLeaf.h"

using namespace std;
int main()
{
    try
    {
        /* code */
        BucketNotLeaf leaf;
        BucketNotLeaf leaf2;
        //leaf.add_element("Alexander", 0);
        //leaf.add_element("John", 1);
        //leaf.add_element("Morales", 2);
        // leaf.write_to_file("test.dat",0);
        leaf.read_from_file("test.dat",0);
        leaf2.read_from_file("test.dat",1);
        //leaf2.add_element("Andrea", 0);
        //leaf2.add_element("Panitz",1);
        //leaf2.add_element("Arroyo", 2);
        //leaf2.write_to_file("test.dat",1);
        //leaf.add_element("Morales", 0);
        //leaf.write_to_file("test.dat",0);
        leaf.show();
        leaf2.show();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}