#ifndef BUCKETNOTLEAF_H
#define BUCKETNOTLEAF_H

/* Dependencies */
#include <iostream>
#include <fstream>
#include <cstring>
#include <exception>
#include <map>
#include <type_traits>
/* Constants */
const int MAX_LENGTH_VARCHAR = 30;
const int FACTOR_BLOCK = 3;

template <typename T>
class BucketNotLeaf
{
private:
    std::map<T, long> *elements_;
    int size_;
    long next_;

public:
    BucketNotLeaf();
    ~BucketNotLeaf();
    void set_next(long next);
    void add_element(T key, long value);
    void remove_element(T);
    long get_position(T);
    int get_size();
    long get_next();
    std::map<T, long> *get_elements();
    void read_from_file(std::string filename, long position);
    void write_to_file(std::string filename, long position);
    void show();

private:
    int get_sizeof();
};

template <typename T>
int BucketNotLeaf<T>::get_sizeof()
{
    return sizeof(long) + sizeof(int) + (sizeof(long) + MAX_LENGTH_VARCHAR) * MAX_LENGTH_VARCHAR;
}

template <typename T>
BucketNotLeaf<T>::BucketNotLeaf()
{
    elements_ = new std::map<std::string, long>();
    size_ = 0;
    next_ = -1;
}

template <typename T>
BucketNotLeaf<T>::~BucketNotLeaf()
{
    delete elements_;
}

template <typename T>
void BucketNotLeaf<T>::set_next(long next)
{
    next_ = next;
}

template <typename T>
void BucketNotLeaf<T>::add_element(T key, long value)
{
    if (size_ == FACTOR_BLOCK)
        throw std::runtime_error("BucketNotLeaf is full");
    if (elements_->find(key) == elements_->end())
    {
        elements_->insert(std::pair<std::string, long>(key, value));
        size_++;
    }
}

template <typename T>
void BucketNotLeaf<T>::remove_element(T key)
{
    if (size_ == 0)
        throw std::runtime_error("BucketNotLeaf is empty");
    if (elements_->find(key) != elements_->end())
    {
        elements_->erase(key);
        size_--;
    }
}

template <typename T>
long BucketNotLeaf<T>::get_position(T key)
{
    return (*elements_)[key];
}

template <typename T>
int BucketNotLeaf<T>::get_size()
{
    return size_;
}
template <typename T>
std::map<T, long> *BucketNotLeaf<T>::get_elements()
{
    return elements_;
}
template <typename T>
long BucketNotLeaf<T>::get_next()
{
    return next_;
}

template <typename T>
void BucketNotLeaf<T>::read_from_file(std::string filename, long position)
{

    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("[ERROR]: File not found from BucketNotLeaf::read_from_file");
    file.seekg(position * get_sizeof());
    file.read((char *)&size_, sizeof(int));
    file.read((char *)&next_, sizeof(long));
    if (std::is_same<T, std::string>::value)
    {
        for (int i = 0; i < size_; i++)
        {
            char key[MAX_LENGTH_VARCHAR];
            long value;
            file.read((char *)&key, sizeof(char) * MAX_LENGTH_VARCHAR);
            file.read((char *)&value, sizeof(long));
            std::string strkey(key);
            elements_->insert(std::pair<std::string, long>(strkey, value));
        }
    }
    else
    {
        for (int i = 0; i < size_; i++)
        {
            T key;
            long value;
            file.read((char *)&key, sizeof(T));
            file.read((char *)&value, sizeof(long));
            elements_->insert(std::pair<T, long>(key, value));
        }
    }
    file.close();
}
template <typename T>
void BucketNotLeaf<T>::write_to_file(std::string filename, long position)
{
    std::ofstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open())
        throw std::runtime_error("[ERROR]: File not found from BucketNotLeaf::write_to_file");
    file.seekp(position * get_sizeof());
    file.write((char *)&size_, sizeof(int));
    file.write((char *)&next_, sizeof(long));
    if (std::is_same<T, std::string>::value)
    {
        int i = 0;
        for (auto it = elements_->begin(); it != elements_->end(); it++)
        {
            char key[MAX_LENGTH_VARCHAR];
            strcpy(key, it->first.c_str());
            file.write((char *)&key, sizeof(char) * MAX_LENGTH_VARCHAR);
            file.write((char *)&it->second, sizeof(long));
            i++;
        }
        for (int j = i; j < FACTOR_BLOCK; j++)
        {
            char key[MAX_LENGTH_VARCHAR];
            long value = -1;
            strcpy(key, "");
            file.write((char *)&key, sizeof(char) * MAX_LENGTH_VARCHAR);
            file.write((char *)&value, sizeof(long));
        }
    }
    else
    {
        int i = 0;
        for (auto it = elements_->begin(); it != elements_->end(); it++)
        {
            file.write((char *)&it->first, sizeof(T));
            file.write((char *)&it->second, sizeof(long));
            i++;
        }
        for (int j = i; j < FACTOR_BLOCK; j++)
        {
            T key;
            long value = -1;
            file.write((char *)&key, sizeof(T));
            file.write((char *)&value, sizeof(long));
        }
    }
    file.close();
}

template <typename T>
void BucketNotLeaf<T>::show()
{
    for (auto it = elements_->begin(); it != elements_->end(); it++)
    {
        std::cout << it->first << " " << it->second << std::endl;
    }
}

#endif // BUCKETNOTLEAF_H