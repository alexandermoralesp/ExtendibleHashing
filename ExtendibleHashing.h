#ifndef EXTENDIBLEHASHING_H
#define EXTENDIBLEHASHING_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <bitset>
#include <math.h>

#include "./BucketNotLeaf.h"
#include "./BucketLeaf.h"

size_t get_size_of_file(std::string filename);

void initialize_index_and_records_binary_files(std::string indexFileName, std::string bucketFileName);

template <typename KeyType>
struct Record
{
  KeyType key;
  long pos;
};

size_t get_size_of_file(std::string filename)
{
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open())
    throw std::runtime_error("Cannot open index file");
  file.seekg(0, std::ios::end);
  size_t size_file = file.tellg();
  file.close();
  return size_file;
}

size_t get_number_of_indexes(std::string filename)
{
  size_t size_file = get_size_of_file(filename);
  BucketIndex bi;
  size_t number_of_indexes = size_file / bi.get_size();
  return number_of_indexes;
}

size_t get_number_of_buckets(std::string filename)
{
  size_t size_file = get_size_of_file(filename);
  BucketLeaf<char> br;
  size_t number_of_buckets = size_file / br.get_size();
  return number_of_buckets;
}

template <typename KeyType, int FactorBlock, int depth>
class ExtendibleHashing
{
private:
  std::string index_filename;
  std::string records_filename;
  std::hash<KeyType> hash_fn;

public:
  ExtendibleHashing(std::string filename)
  {
    this->index_filename = filename + ".index.dat";
    this->records_filename = filename + ".records.dat";

    std::ifstream file0(index_filename, std::ios::app);
    if (!file0.is_open())
      throw("eror");
    file0.close();

    std::ifstream file(records_filename, std::ios::app);
    if (!file.is_open())
      throw("eror");
    file.close();
  }
  bool insert(KeyType key, long pos);
  Record<KeyType> search(KeyType key);
  bool remove(KeyType key);

  void readAllIndexes()
  {
    std::cout << "INDEXES:\n\n";
    size_t size_index_file = get_size_of_file(index_filename);
    BucketIndex bi;
    size_t numbers_of_indexes = std::ceil(size_index_file / bi.get_size());
    for (size_t i = 0; i < numbers_of_indexes; i++)
    {
      bi.readFromFile(i, index_filename);
      std::cout << "\nIndex " << i << ": \n";
      bi.show();
    }
  }

  void readAllBuckets()
  {
    std::cout << "BUCKETS\n\n";
    size_t size_buckets_file = get_size_of_file(records_filename);
    BucketLeaf<KeyType> br;
    size_t number_of_buckets = std::ceil(size_buckets_file / br.get_size());
    std::cout << "Number of buckets is: " << number_of_buckets << "\n";
    for (size_t i = 0; i < number_of_buckets; i++)
    {
      br.readFromFile(i, records_filename);
      std::cout << "Bucket " << i << ": \n";
      br.show();
    }
  }

private:
  std::string get_hash_code(const KeyType &key)
  {
    size_t hash_number = hash_fn(key) % (1 << depth);
    std::string hash_code = std::bitset<depth>(hash_number).to_string();
    return hash_code;
  }
  void initialize_index_and_records_binary_files(std::string indexFileName, std::string bucketFileName);
};

template <typename KeyType, int FactorBlock, int depth>
bool ExtendibleHashing<KeyType, FactorBlock, depth>::insert(KeyType key, long pos)
{
  int counter_access = 0;
  // TODO: Resolver problema de string keytype para la escritura
  // Get hash code
  std::string hash_code = get_hash_code(key);
  // std::cout << "Hash code: " << hash_code << std::endl;

  // Get size of index file
  size_t size_index_file = get_size_of_file(index_filename);
  if (size_index_file == 0)
  {
    initialize_index_and_records_binary_files(index_filename, records_filename);
    counter_access += 3;
    std::cout << "First Number of indexes: " << get_number_of_indexes(index_filename) << "\n";
    std::cout << "First Number of buckets: " << get_number_of_buckets(records_filename) << "\n";
  }
  counter_access += 3;

  size_index_file = get_size_of_file(index_filename);
  size_t size_records_file = get_size_of_file(records_filename);
  BucketIndex current;
  current.readFromFile(0, index_filename);
  int i = 0;
  long current_position = 0;
  // 100
  while (!current.isLeaf && i < depth)
  {
    if (hash_code[i] == '1')
    {
      current_position = current.one;
      current.readFromFile(current_position, index_filename);
    }
    else
    {
      current_position = current.zero;
      current.readFromFile(current_position, index_filename);
    }
    i++;
  }
  counter_access += depth;
  
  // When current is leaf, the position of the bucket record is zero
  size_t bucket_position = current.zero;
  BucketLeaf<KeyType> bucket_record;
  bucket_record.readFromFile(bucket_position, records_filename);

  // Number of records
  size_t number_of_indexes = size_index_file / current.get_size();
  size_t number_of_buckets = size_records_file / bucket_record.get_size();

  if (bucket_record.size < FactorBlock)
  {
    bucket_record.insert(key, pos);
    bucket_record.writeToFile(bucket_position, records_filename);
  }
  else if (bucket_record.size == FactorBlock && i == depth)
  {
    // Add new bucket to bucket record
    while (bucket_record.next != -1)
    {
      bucket_position = bucket_record.next;
      bucket_record.readFromFile(bucket_record.next, records_filename);
          counter_access += 1;
    }
    if (bucket_record.size < FactorBlock)
    {
      bucket_record.insert(key, pos);
      bucket_record.writeToFile(bucket_position, records_filename);
      counter_access += 1;
    }
    else
    {
      BucketLeaf<KeyType> new_bucket_record;
      new_bucket_record.insert(key, pos);
      new_bucket_record.writeToFile(number_of_buckets, records_filename);
      bucket_record.next = number_of_buckets;
      bucket_record.writeToFile(bucket_position, records_filename);
      counter_access += 2;
    }
  }
  else
  {
    BucketIndex new_zero;
    BucketIndex new_one;

    new_zero.isLeaf = true;
    new_one.isLeaf = true;

    BucketLeaf<KeyType> new_zero_record;
    BucketLeaf<KeyType> new_one_record;

    // Add new bucket to bucket record
    if (hash_code[i] == '1')
    {
      new_one_record.insert(key, pos);
    }
    else
    {
      new_zero_record.insert(key, pos);
    }

    for (int j = 0; j < bucket_record.size; j++)
    {
      std::string hash_code_key = get_hash_code(bucket_record.keys[j]);
      if (hash_code_key[i] == '1')
      {
        new_one_record.insert(bucket_record.keys[j], bucket_record.positions[j]);
      }
      else
      {
        new_zero_record.insert(bucket_record.keys[j], bucket_record.positions[j]);
      }
    }
    // TODO: Remove flag for bucket record
    // Add new buckets to records file
    new_zero_record.writeToFile(bucket_position, records_filename);
    new_one_record.writeToFile(number_of_buckets, records_filename);
    counter_access += 3;
    // Updated current index
    current.zero = number_of_indexes;
    current.one = number_of_indexes + 1;
    current.isLeaf = false;

    BucketIndex zero_child(0, true);
    BucketIndex one_child(1, true);
    zero_child.zero = bucket_position;
    one_child.zero = number_of_buckets;

    current.writeToFile(current_position, index_filename);
    zero_child.writeToFile(number_of_indexes, index_filename);
    one_child.writeToFile(number_of_indexes + 1, index_filename);
    counter_access += 3;
  }
  // std::cout << "Number of indexes: " << get_number_of_indexes(index_filename) << "\n";
  // std::cout << "Number of buckets: " << get_number_of_buckets(records_filename) << "\n";
  std::cout << counter_access << "\n";
  return true;
}

template <typename KeyType, int FactorBlock, int depth>
void ExtendibleHashing<KeyType, FactorBlock, depth>::initialize_index_and_records_binary_files(std::string indexFileName, std::string bucketFileName)
{
  BucketIndex root;
  root.zero = 1;
  root.one = 2;

  BucketIndex child0(0, 1);
  child0.isLeaf = true;
  BucketIndex child1(1, 1);
  child1.isLeaf = true;

  child0.zero = 0;
  child1.zero = 1;

  root.writeToFile(0, index_filename);
  child0.writeToFile(1, index_filename);
  child1.writeToFile(2, index_filename);

  BucketLeaf<KeyType> bucketRecord0;
  bucketRecord0.writeToFile(0, bucketFileName);
  BucketLeaf<KeyType> bucketRecord1;
  bucketRecord1.writeToFile(1, bucketFileName);
}

template <typename KeyType, int FactorBlock, int depth>
Record<KeyType> ExtendibleHashing<KeyType, FactorBlock, depth>::search(KeyType key)
{
  size_t index_file = get_size_of_file(index_filename);
  if (!index_file)
    throw std::runtime_error("Index file is empty");
  std::string hash_code = get_hash_code(key);
  BucketIndex current;
  current.readFromFile(0, index_filename);
  int i = 0;
  long current_position = 0;
  while (!current.isLeaf && i < depth)
  {
    if (hash_code[i] == '1')
    {
      current_position = current.one;
      current.readFromFile(current_position, index_filename);
    }
    else
    {
      current_position = current.zero;
      current.readFromFile(current_position, index_filename);
    }
    i++;
  }
  long record_position = current.zero;
  BucketLeaf<KeyType> bucket_record;
  bucket_record.readFromFile(record_position, records_filename);
  if (bucket_record.size == 0)
    throw std::runtime_error("Record not found");
  do
  {
    for (int i = 0; i < bucket_record.size; i++)
    {
      if (bucket_record.keys[i] == key && bucket_record.positions[i] != -1)
      {
        Record<KeyType> record;
        record.key = key;
        record.pos = bucket_record.positions[i];
        return record;
      }
    }
    if (bucket_record.next == -1)
      break;
    record_position = bucket_record.next;
    bucket_record.readFromFile(record_position, records_filename);
  } while (record_position != -1);
  throw std::runtime_error("Key not found");
}

template <typename KeyType, int FactorBlock, int depth>
bool ExtendibleHashing<KeyType, FactorBlock, depth>::remove(KeyType key)
{
  int counter_access = 0;
  std::cout << "Access: "<< counter_access;
  size_t index_file = get_size_of_file(index_filename);
  if (!index_file)
    throw std::runtime_error("Index file is empty");
  std::string hash_code = get_hash_code(key);
  BucketIndex current;
  current.readFromFile(0, index_filename);
  counter_access++;
  
  int i = 0;
  long current_position = 0;
  while (!current.isLeaf && i < depth)
  {
    if (hash_code[i] == '1')
    {
      current_position = current.one;
      current.readFromFile(current_position, index_filename);
    }
    else
    {
      current_position = current.zero;
      current.readFromFile(current_position, index_filename);
    }
    i++;
    counter_access++;
  }
  long record_position = current.zero;
  BucketLeaf<KeyType> bucket_record;
  bucket_record.readFromFile(record_position, records_filename);
  counter_access++;
  if (bucket_record.size == 0) 
  {
    return true;
    std::cout << "Remove: " << counter_access << "\n";
  }
  do
  {
    for (int i = 0; i < bucket_record.size; i++)
    {
      if (bucket_record.keys[i] == key)
      {
        bucket_record.remove(key);
        bucket_record.writeToFile(record_position, records_filename);
        counter_access++;
        std::cout << "Remove: " << counter_access << "\n";
        return true;
      }
    }
    if (bucket_record.next == -1)
      break;
    record_position = bucket_record.next;
    bucket_record.readFromFile(record_position, records_filename);
  } while (record_position != -1);
  std::cout << "Remove: " << counter_access << "\n";
  return false;
}

#endif