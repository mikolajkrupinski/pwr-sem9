#include <deque>
#include <iostream>
#include <random>

// Element stored in the reservoir
struct Candidate {
  size_t index;   // Index of element in stream
  double weight;  // Random weight
  int value;      // Value from the stream
};

// Sampling with Replacement
class SwR {
 private:
  const size_t n;  // Size of a single window
  size_t time;     // Time counter, incremented with each new element
  std::deque<Candidate> reservoir;  // Reservoir to hold candidates

  double currentMaxWeight;
  size_t nextCandidateIndex;

  std::mt19937 gen;

  // Generate the next hop index based on geometric distribution
  size_t nextHop() {
    double p = 1.0 - this->currentMaxWeight;
    std::geometric_distribution<size_t> geoDist(p);
    return this->nextCandidateIndex + geoDist(gen);
  }

 public:
  SwR(size_t n) : n(n), time(0), currentMaxWeight(0.0), nextCandidateIndex(0) {
    std::random_device rd;
    gen.seed(rd());
  }

  void onGet(int item) {
    size_t currentIndex = time;
    this->time += 1;

    // If time to add new candidate
    if (currentIndex == this->nextCandidateIndex) {
      std::uniform_real_distribution<double> uniformDist(this->currentMaxWeight,
                                                         1.0);
      double newWeight = uniformDist(gen);
      this->reservoir.push_back(Candidate{currentIndex, newWeight, item});

      this->currentMaxWeight = newWeight;
      this->nextCandidateIndex = this->nextHop();
    }

    // Delete all the expired elements
    size_t expiryThreshold = (currentIndex >= n) ? (currentIndex - n + 1) : 0;
    while (!this->reservoir.empty() &&
           this->reservoir.front().index < expiryThreshold) {
      this->reservoir.pop_front();
    }

    // If reservoir is empty, reset parameters
    if (this->reservoir.empty()) {
      this->currentMaxWeight = 0.0;
      this->nextCandidateIndex = currentIndex + 1;
    }
  }

  int getSample() const {
    if (this->reservoir.empty()) {
      throw std::runtime_error("Reservoir is empty");
    }
    return this->reservoir.front().value;
  }
};

int main() {
  SwR sampler(5);

  for (int i = 1; i <= 20; ++i) {
    sampler.onGet(i);
    try {
      int sample = sampler.getSample();
      std::cout << "Insert " << i << ", sample = " << sample << std::endl;
    } catch (const std::runtime_error& e) {
      std::cout << e.what() << std::endl;
    }
  }

  return 0;
}
