#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <fstream>
class ArrayGenerator {
 public:
  [[nodiscard]] int getMaxSize() const {
    return max_size;
  }

  explicit ArrayGenerator(int max_size, int min, int max)
      : max_size(max_size), min(min), max(max), seed(std::random_device{}()) {
    madeRandomArray();
    madeReversedArray();
    madeAlmostSortedArray();
  }

  std::vector<int> RandomArray(int size) const {
    return std::vector<int>(randomArray.begin(), randomArray.begin() + size);
  }

  std::vector<int> ReversedArray(int size) const {
    return std::vector<int>(reversedArray.begin(), reversedArray.begin() + size);
  }

  std::vector<int> AlmostSortedArray(int size) const {
    return std::vector<int>(notsortedArray.begin(), notsortedArray.begin() + size);
  }

 private:
  int max_size;
  int min, max;
  std::vector<int> randomArray;
  std::vector<int> reversedArray;
  std::vector<int> notsortedArray;
  std::mt19937 seed;

  void madeRandomArray() {
    std::uniform_int_distribution<int> dist(min, max);
    randomArray.resize(max_size);
    for (int& elem : randomArray) {
      elem = dist(seed);
    }
  }

  void madeReversedArray() {
    reversedArray = randomArray;
    std::sort(reversedArray.rbegin(), reversedArray.rend());
  }

  void madeAlmostSortedArray() {
    notsortedArray = randomArray;
    std::sort(notsortedArray.begin(), notsortedArray.end());
    std::uniform_int_distribution<int> dist(0, max_size - 1);
    for (int i = 0; i < max_size / 100; ++i) {
      int first = dist(seed);
      int second = dist(seed);
      std::swap(notsortedArray[first], notsortedArray[second]);
    }
  }
};
class SortTester {
 public:
  SortTester() = default;

  void testSorts(const ArrayGenerator& generator, const std::string& result) {
    std::ofstream out(result);

    out << "n\tRandomMerge\tReversedMerge\tAlmostSortedMerge\t"
           "RandomHybrid\tReversedHybrid\tAlmostSortedHybrid\n";

    int max_size = generator.getMaxSize();
    for (int size = 500; size <= max_size; size += 100) {
      std::vector<int> randomArray = generator.RandomArray(size);
      std::vector<int> reversedArray = generator.ReversedArray(size);
      std::vector<int> almostSortedArray = generator.AlmostSortedArray(size);

      out << size << "\t";
      out << measureTime(randomArray, mergeSort) << "\t";
      out << measureTime(reversedArray, mergeSort) << "\t";
      out << measureTime(almostSortedArray, mergeSort) << "\t";
      out << measureTime(randomArray, hybridMergeSort) << "\t";
      out << measureTime(reversedArray, hybridMergeSort) << "\t";
      out << measureTime(almostSortedArray, hybridMergeSort) << "\n";
    }

    out.close();
  }

  inline static int threshold;

 private:
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
    for (i = left, k = 0; i <= right; ++i, ++k) {
      array[i] = arr[k];
    }
  }
  static void mergeSort(std::vector<int>& arr, int left, int right) {
    if (left >= right) return;
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

  template <typename sortByFunction>
  long long measureTime(std::vector<int> arr, sortByFunction func) {
    auto start = std::chrono::high_resolution_clock::now();
    func(arr, 0, arr.size() - 1);
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
  }
};
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  std::cout.tie(nullptr);

  int maxSize = 7000;
  const int threshold = 15;
  SortTester::threshold = threshold;

  ArrayGenerator generator(maxSize, 0, 4000);
  SortTester tester;
  tester.testSorts(generator, "result.txt");

  return 0;
}