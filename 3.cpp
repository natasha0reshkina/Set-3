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

  void Tester(const ArrayGenerator& generator, const std::string& result) {
    std::ofstream out(result);

    out << "n\tRandomQuickSort\tReversedQuickSort\tAlmostSortedQuickSort\t"
           "RandomHybridSort\tReversedHybridSort\tAlmostSortedHybridSort\n";

    int max_size = generator.getMaxSize();
    for (int size = 500; size <= max_size; size += 100) {
      std::vector<int> randomArray = generator.RandomArray(size);
      std::vector<int> reversedArray = generator.ReversedArray(size);
      std::vector<int> almostSortedArray = generator.AlmostSortedArray(size);

      out << size << "\t";
      auto start = std::chrono::high_resolution_clock::now();
      quickSort(randomArray, 0, randomArray.size() - 1,2 * std::log2(randomArray.size()));
      auto elapsed = std::chrono::high_resolution_clock::now() - start;
      out<< std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()<<"\t";
      start = std::chrono::high_resolution_clock::now();
      quickSort(reversedArray, 0, reversedArray.size() - 1,2 * std::log2(reversedArray.size()));
      elapsed = std::chrono::high_resolution_clock::now() - start;
      out<< std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()<<"\t";
      start = std::chrono::high_resolution_clock::now();
      quickSort(almostSortedArray, 0, almostSortedArray.size() - 1,2 * std::log2(almostSortedArray.size()));
      elapsed = std::chrono::high_resolution_clock::now() - start;
      out<< std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()<<"\t";
      start = std::chrono::high_resolution_clock::now();
      hybridSort(almostSortedArray);
      elapsed = std::chrono::high_resolution_clock::now() - start;
      out<< std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()<<"\t";
      start = std::chrono::high_resolution_clock::now();
      hybridSort(randomArray);
      elapsed = std::chrono::high_resolution_clock::now() - start;
      out<< std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()<<"\t";
      start = std::chrono::high_resolution_clock::now();
      hybridSort(almostSortedArray);
      elapsed = std::chrono::high_resolution_clock::now() - start;
      out<< std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count()<<"\n";
    }

    out.close();
  }

  inline static int threshold;

 private:
  static void insertionSort(std::vector<int>& A, int left, int right) {
    for (int i = left + 1; i <= right; ++i) {
      int key = A[i];
      int j = i - 1;
      while (j >= left && A[j] > key) {
        A[j + 1] = A[j];
        --j;
      }
      A[j + 1] = key;
    }
  }

  static void heapify(std::vector<int>& A, int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && A[left] > A[largest]) {
      largest = left;
    }
    if (right < n && A[right] > A[largest]) {
      largest = right;
    }
    if (largest != i) {
      std::swap(A[i], A[largest]);
      heapify(A, n, largest);
    }
  }

  static void heapSort(std::vector<int>& A, int left, int right) {
    int n = right - left + 1;
    std::vector<int> arr(A.begin() + left, A.begin() + right + 1);
    for (int i = n / 2 - 1; i >= 0; --i) {
      heapify(arr, n, i);
    }

    for (int i = n - 1; i > 0; --i) {
      std::swap(arr[0], arr[i]);
      heapify(arr, i, 0);
    }

    for (int i = 0; i < n; ++i) {
      A[left + i] = arr[i];
    }
  }

  static void quickSort(std::vector<int>& A, int left, int right, int limit) {
    const int max = 16;
    if (left >= right) {
      return;
    }
    int size = right - left + 1;
    if (size <= max) {
      insertionSort(A, left, right);
      return;
    }
    if (limit == 0) {
      heapSort(A, left, right);
      return;
    }
    int central = A[right];
    int i = left - 1;
    for (int j = left; j < right; ++j) {
      if (A[j] < central) {
        std::swap(A[++i], A[j]);
      }
    }
    std::swap(A[i + 1], A[right]);
    int centralInd = i + 1;
    quickSort(A, left, centralInd - 1, limit - 1);
    quickSort(A, centralInd + 1, right, limit - 1);
  }

  static void hybridSort(std::vector<int>& A) {
    int n = A.size();
    int limit = 2 * std::log2(n);
    quickSort(A, 0, n - 1, limit);
  }


};
int main() {
  int max_size = 10000;
  const int threshold = 15;
  SortTester::threshold = threshold;
  ArrayGenerator generator(max_size, 0, 6000);
  SortTester test;
  test.Tester(generator, "result.txt");
  return 0;
}