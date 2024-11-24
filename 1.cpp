#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <fstream>

class ArrayGenerator {
 public:
  std::vector<int> generateRandomArray(int size) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 6000);

    std::vector<int> data(size);
    for (int i = 0; i < size; ++i) {
      data[i] = dis(gen);
    }
    return data;
  }

  std::vector<int> generateReversedArray(int size) {
    std::vector<int> data = generateRandomArray(size);
    std::reverse(data.begin(), data.end());
    return data;
  }

  std::vector<int> generateAlmostSortedArray(int size) {
    std::vector<int> data = generateRandomArray(size);
    for (int i = 1; i < size; ++i) {
      if (data[i] > data[i - 1]) {
        int temp = data[i];
        data[i] = data[i - 1];
        data[i - 1] = temp;
      }
    }
    return data;
  }
};

class SortTester {
 public:
  static const int minSize = 500;
  static const int maxSize = 10000;
  static const int step = 100;
  static const int threshold = 50;
  void testSorts(ArrayGenerator& generator, std::ofstream& out) {
    out << "n\tRandomMerge\tReversedMerge\tAlmostSortedMerge\t"
           "RandomHybrid\tReversedHybrid\tAlmostSortedHybrid\n";

    for (int size = minSize; size <= maxSize; size += step) {
      std::vector<int> randomArray = generator.generateRandomArray(size);
      std::vector<int> reversedArray = generator.generateReversedArray(size);
      std::vector<int> almostSortedArray = generator.generateAlmostSortedArray(size);
      auto start = std::chrono::high_resolution_clock::now();
      mergeSort(randomArray, 0, randomArray.size() - 1);
      auto elapsed = std::chrono::high_resolution_clock::now() - start;
      out << size << "\t" << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << "\t";
      start = std::chrono::high_resolution_clock::now();
      mergeSort(reversedArray, 0, reversedArray.size() - 1);
      elapsed = std::chrono::high_resolution_clock::now() - start;
      out << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << "\t";
      start = std::chrono::high_resolution_clock::now();
      mergeSort(almostSortedArray, 0, almostSortedArray.size() - 1);
      elapsed = std::chrono::high_resolution_clock::now() - start;
      out << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << "\t";
      start = std::chrono::high_resolution_clock::now();
      hybridMergeSort(randomArray, 0, randomArray.size() - 1);
      elapsed = std::chrono::high_resolution_clock::now() - start;
      out << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << "\t";
      start = std::chrono::high_resolution_clock::now();
      hybridMergeSort(reversedArray, 0, reversedArray.size() - 1);
      elapsed = std::chrono::high_resolution_clock::now() - start;
      out << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << "\t";
      start = std::chrono::high_resolution_clock::now();
      hybridMergeSort(almostSortedArray, 0, almostSortedArray.size() - 1);
      elapsed = std::chrono::high_resolution_clock::now() - start;
      out << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << "\n";
    }
  }

 public:
  static void insertionSort(std::vector<int>& array, int left, int right) {
    for (int i = left + 1; i <= right; ++i) {
      int key = array[i];
      int j = i - 1;
      while (j >= left && array[j] > key) {
        array[j + 1] = array[j];
        --j;
      }
      array[j + 1] = key;
    }
  }

  static void merge(std::vector<int>& array, int left, int mid, int right) {
    std::vector<int> arr(right - left + 1);
    int i = left, j = mid + 1, k = 0;
    while (i <= mid && j <= right) {
      if (array[i] <= array[j]) {
        arr[k++] = array[i++];
      } else {
        arr[k++] = array[j++];
      }
    }
    while (i <= mid) {
      arr[k++] = array[i++];
    }
    while (j <= right) {
      arr[k++] = array[j++];
    }
    for (int i = left, k = 0; i <= right; ++i, ++k) {
      array[i] = arr[k];
    }
  }

  static void mergeSort(std::vector<int>& arr, int left, int right) {
    if (left >= right)
      return;
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
  }

  static void hybridMergeSort(std::vector<int>& array, int left, int right) {
    if (right - left + 1 <= threshold) {
      insertionSort(array, left, right);
    } else if (left < right) {
      int mid = left + (right - left) / 2;
      hybridMergeSort(array, left, mid);
      hybridMergeSort(array, mid + 1, right);
      merge(array, left, mid, right);
    }
  }
};

int main() {
  ArrayGenerator generator;
  std::ofstream out("results.txt");
  SortTester tester;
  tester.testSorts(generator, out);
  out.close();
  std::cout << "Results in results.txt";
  return 0;
}
