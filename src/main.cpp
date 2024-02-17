#include <algorithm>
#include <cmath>
#include <iostream>
#include <ostream>
#include <vector>

class Primes {
protected:
  Primes();
  int cacheSize;
  int highesValueChecked = 1;
  std::vector<int> primeCache;

  // SIMD implementation
  void genPrimesSIMD(int);

  // Goldbach method
  bool contains(int);
  int higestCalculatedNum;

private:
  bool isPrimeSIMD(int);
};

Primes::Primes() {
  // highesValueChecked = 3;
  primeCache.push_back(2);
  primeCache.push_back(3);
}

bool Primes::contains(int num) {
  // // naive solution
  // for (int prime : primeCache) {
  //   if (prime == num)
  //     return true;
  // }
  //
  // return false;

  // Skip the last few numbers
  // end When checked num is under looking num

  // Can start at num / 3 + 1 as
  //  2 / 3 + 1 = 1 higher than needed
  //  3 / 3 + 1 = 2 higher than needed
  //  5 / 3 + 1 = 2 correct
  //  7 / 3 + 1 = 3 correct
  //  11 / 3 + 1 = 4 correct
  //  13 / 3 + 1 = 5 correct
  //  17 / 3 + 1 = 6 correct
  //  19 / 3 + 1 = 7 correct
  //  23 / 3 + 1 = 8 correct
  //  29 / 3 + 1 = 10 higher than needed
  //  Will always be higher from here on out
  int index = (int)(num / 3) + 1;

  // need to correct for potentially too high starting index
  // take 100
  // cache size 25; index 34;
  // 34 - 25 - 1= 9 (0 indexed)
  // 34 - 10 = 24
  index -= (index >= cacheSize) * (index - cacheSize - 1);

  int current_num;
  bool found = false;

  bool completed = false;
  while (!completed) {
    current_num = primeCache[index];

    found = num == current_num;
    completed = found || current_num < num;
    index--;
  }

  return found;
}

bool Primes::isPrimeSIMD(int n) {
  int num;
  num = 5;

  int lim = n;
  bool isPrime = (n % 2 != 0 && n % 3 != 0);
  bool completed = !isPrime || 25 > lim;
  while (!completed) {
    isPrime = n % num != 0 && n % (num + 2) != 0 && n % (num + 6) != 0 &&
              n % (num + 8) != 0 && n % (num + 12) != 0 &&
              n % (num + 14) != 0 && n % (num + 18) != 0 && n % (num + 20) != 0;

    completed = !isPrime || (num + 20) * (num + 20) > lim;

    num += 24;
  }

  highesValueChecked = n;
  return isPrime;
}

void Primes::genPrimesSIMD(int n) {
  int upperPrimeLimit = (int)(n / 3);
  upperPrimeLimit = upperPrimeLimit << 1;
  primeCache.reserve(upperPrimeLimit);

  for (int i = 5; i <= n; i += 6) {
    if (isPrimeSIMD(i)) {
      primeCache.push_back(i);
    }

    if (isPrimeSIMD(i + 2)) {
      primeCache.push_back(i + 2);
    }
  }

  cacheSize = primeCache.size();
  higestCalculatedNum = primeCache[cacheSize];
}

/* ------ */

class Goldbach : Primes {
private:
  int limit = 1000;
  bool debug = 0;

public:
  Goldbach();
  int ask_settings();
  void print_settings();
  void calculate();
  int getLimit();
  void setLimit(int);
};

int Goldbach::getLimit() { return limit; }
void Goldbach::setLimit(int n) { limit = n; }

int Goldbach ::ask_settings() {
  std::cout << "Till what number would you like to calculate?" << std::endl;
  std::cin >> limit;

  if (limit <= 0) {
    std::cout << "Limit isn't set correctly" << std::endl;
    return -1;
  }

  return 0;
}

Goldbach::Goldbach() {}

void Goldbach ::print_settings() {
  std::cout << std::endl << "Settings:" << std::endl;
  std::cout << "limit: " << this->limit << std::endl;
  std::cout << std::endl;
}

void Goldbach ::calculate() {
  genPrimesSIMD(limit);

  std::cout << "Generated primes" << std::endl;

  bool calulated = false;

  for (int num = 4; num <= limit; num += 2) {
    calulated = false;

    for (int i = 0; i <= cacheSize; ++i) {
      int currentPrime = primeCache[i];

      if (contains(num - currentPrime)) {
        // std::cout << currentPrime << " + " << num - currentPrime << " = " <<
        // num
        //           << std::endl;
        calulated = true;
        break;
      }
    }

    if (!calulated) {
      std::cout << "FOUND AN ERROR" << std::endl;
      exit(-1);
    }
  }
}

int main(int argc, char *argv[]) {

  Goldbach gb;

  // gb.ask_settings();
  // gb.print_settings();

  // auto begin = std::chrono::high_resolution_clock::now();

  gb.setLimit(1000);
  gb.calculate();

  // auto end = std::chrono::high_resolution_clock::now();
  // auto duration =
  //     std::chrono::duration_cast<std::chrono::milliseconds>(end - begin)
  //         .count();

  // std::cout << "Calculation took: " << duration << "ms" << std::endl;

  return 0;
}
