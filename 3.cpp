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
    std::sort(data.begin(), data.end());
    std::reverse(data.begin(), data.end());
    return data;
  }

  std::vector<int> generateAlmostSortedArray(int size) {
    std::vector<int> data = generateRandomArray(size);
    std::sort(data.begin(), data.end());
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
    out << "n\tRandomQuickSort\tReversedQuickSort\tAlmostSortedQuickSort\t"
           "RandomHybridSort\tReversedHybridSort\tAlmostSortedHybridSort\n";

    for (int size = minSize; size <= maxSize; size += step) {
      std::vector<int> randomArray = generator.generateRandomArray(size);
      std::vector<int> reversedArray = generator.generateReversedArray(size);
      std::vector<int> almostSortedArray = generator.generateAlmostSortedArray(size);
      auto start = std::chrono::high_resolution_clock::now();
      quickSort(randomArray, 0, randomArray.size() - 1, 2 * std::log2(randomArray.size()));
      auto elapsed = std::chrono::high_resolution_clock::now() - start;
      out << size << "\t" << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << "\t";
      start = std::chrono::high_resolution_clock::now();
      quickSort(reversedArray, 0, reversedArray.size() - 1, 2 * std::log2(reversedArray.size()));
      elapsed = std::chrono::high_resolution_clock::now() - start;
      out << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << "\t";
      start = std::chrono::high_resolution_clock::now();
      quickSort(almostSortedArray, 0, almostSortedArray.size() - 1, 2 * std::log2(almostSortedArray.size()));
      elapsed = std::chrono::high_resolution_clock::now() - start;
      out << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << "\t";

       randomArray = generator.generateRandomArray(size);
      reversedArray = generator.generateReversedArray(size);
      almostSortedArray = generator.generateAlmostSortedArray(size);
      start = std::chrono::high_resolution_clock::now();
      hybridSort(randomArray);
      elapsed = std::chrono::high_resolution_clock::now() - start;
      out << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << "\t";
      start = std::chrono::high_resolution_clock::now();
      hybridSort(reversedArray);
      elapsed = std::chrono::high_resolution_clock::now() - start;
      out << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << "\t";
      start = std::chrono::high_resolution_clock::now();
      hybridSort(almostSortedArray);
      elapsed = std::chrono::high_resolution_clock::now() - start;
      out << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << "\n";
    }
  }

 public:
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
  ArrayGenerator generator;
  std::ofstream out("results.txt");
  SortTester tester;
  tester.testSorts(generator, out);
  out.close();
  std::cout << "Results in results.txt";
  return 0;
}
