#ifndef BUCKETNOTLEAF_H
#define BUCKETNOTLEAF_H

/* Dependencies */
#include <iostream>
#include <fstream>
#include <cstring>
#include <exception>
#include <map>

/* Constants */
const int MAX_LENGTH_VARCHAR = 30;
const int FACTOR_BLOCK = 3;

class BucketNotLeaf
{
private:
    std::map<std::string, long> *elements_;
    int size_;
    long next_;

public:
    BucketNotLeaf();
    ~BucketNotLeaf();
    void set_next(long next);
    void add_element(std::string key, long value);
    void remove_element(std::string key);
    long get_position(std::string key);
    int get_size();
    long get_next();
    std::map<std::string, long> *get_elements();
    void read_from_file(std::string filename, long position);
    void write_to_file(std::string filename, long position);
    void show();

private:
    int get_sizeof();
};

int BucketNotLeaf::get_sizeof()
{
    return sizeof(long) + sizeof(int) + (sizeof(long) + MAX_LENGTH_VARCHAR) * MAX_LENGTH_VARCHAR;
}

BucketNotLeaf::BucketNotLeaf()
{
    elements_ = new std::map<std::string, long>();
    size_ = 0;
    next_ = -1;
}

BucketNotLeaf::~BucketNotLeaf()
{
    delete elements_;
}
void BucketNotLeaf::set_next(long next)
{
    next_ = next;
}
void BucketNotLeaf::add_element(std::string key, long value)
{
    if (size_ == FACTOR_BLOCK)
        throw std::runtime_error("BucketNotLeaf is full");
    if (elements_->find(key) == elements_->end())
    {
        elements_->insert(std::pair<std::string, long>(key, value));
        size_++;
    }
}

void BucketNotLeaf::remove_element(std::string key)
{
    if (size_ == 0) throw std::runtime_error("BucketNotLeaf is empty");
    if (elements_->find(key) != elements_->end())
    {
        elements_->erase(key);
        size_--;
    }
}

long BucketNotLeaf::get_position(std::string key)
{
    return (*elements_)[key];
}

int BucketNotLeaf::get_size()
{
    return size_;
}

std::map<std::string, long> *BucketNotLeaf::get_elements()
{
    return elements_;
}

long BucketNotLeaf::get_next()
{
    return next_;
}

void BucketNotLeaf::read_from_file(std::string filename, long position)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("[ERROR]: File not found from BucketNotLeaf::read_from_file");
    file.seekg(position * get_sizeof());
    file.read((char *)&size_, sizeof(int));
    file.read((char *)&next_, sizeof(long));
    for (int i = 0; i < FACTOR_BLOCK; i++)
    {
        char key[MAX_LENGTH_VARCHAR];
        long value;
        file.read((char *)&key, sizeof(char) * MAX_LENGTH_VARCHAR);
        file.read((char *)&value, sizeof(long));
        elements_->insert(std::pair<std::string, long>(key, value));
    }
    file.close();
}

void BucketNotLeaf::write_to_file(std::string filename, long position)
{
    std::ofstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("[ERROR]: File not found from BucketNotLeaf::write_to_file");
    file.seekp(position * get_sizeof());
    file.write((char *)&size_, sizeof(int));
    file.write((char *)&next_, sizeof(long));
    for (auto it = elements_->begin(); it != elements_->end(); it++)
    {
        char key[MAX_LENGTH_VARCHAR];
        long value;
        std::strcpy(key, it->first.c_str());
        value = it->second;
        file.write((char *)&key, sizeof(char) * MAX_LENGTH_VARCHAR);
        file.write((char *)&value, sizeof(long));
    }
    file.close();
}

void BucketNotLeaf::show()
{
    for (auto it = elements_->begin(); it != elements_->end(); it++)
    {
        std::cout << it->first << " " << it->second << std::endl;
    }
}

#endif // BUCKETNOTLEAF_H