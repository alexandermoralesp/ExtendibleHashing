#ifndef BUCKETLEAF_H
#define BUCKETLEAF_H

#include <iostream>
#include <fstream>

struct BucketIndex {
  int bit;
  long zero;
  long one;
  bool isLeaf;
  BucketIndex() {
    this->bit = -1;
    this->isLeaf = false;
    this->zero = -1;
    this->one = -1;
  }
  BucketIndex(int bit, bool isLeaf) {
    this->bit = bit;
    this->isLeaf = isLeaf;
    this->zero = -1;
    this->one = -1;
  }
  ~BucketIndex() = default;
  void show() {
    std::cout << "Bucket Index ->";
    std::cout << "Bit: " << bit << " IsLeaf: " << isLeaf << std::endl;
    std::cout << "Zero: " << zero << " One: " << one << std::endl;
  }
  size_t get_size() {
    return sizeof(bit) + sizeof(zero) + sizeof(one) + sizeof(isLeaf);
  }
  void read_from_file(long pos, std::string filename) {
    BucketIndex bi(-1, false);
    std::ifstream fin(filename, std::ios::in | std::ios::binary);
    if (!fin.is_open()) throw ("File not open");
    fin.seekg(pos*this->get_size(), std::ios::beg);
    fin.read((char*)&this->bit, sizeof(int));
    fin.read((char*)&this->zero, sizeof(long));
    fin.read((char*)&this->one, sizeof(long));
    fin.read((char*)&this->isLeaf, sizeof(bool));
    fin.close();
    return;
  }
  void write_to_file(long pos, std::string filename) {
    std::ofstream fout(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!fout.is_open()) throw ("File not open");
    fout.seekp(pos*this->get_size(), std::ios::beg);
    fout.write((char*)&bit, sizeof(int));
    fout.write((char*)&zero, sizeof(long));
    fout.write((char*)&one, sizeof(long));
    fout.write((char*)&isLeaf, sizeof(bool));
    fout.close();
  }
};

#endif