#include <math.h>
#include <stdio.h>

typedef struct seq_data {
  double mean;
  double variance;
} seq_data;

seq_data calc_bi(const double seq[], size_t n);
seq_data calc_naive(const double seq[], size_t n);
seq_data calc_welford(const double seq[], size_t n);
double relative_error(double approx, double exact);

int main() {
  const double seq[] = {4.0, 7.0, 13.0, 16.0};
  const size_t n = sizeof(seq) / sizeof(seq[0]);
  const double exact_mean = 10.0;
  const double exact_variance = 22.5;

  const double shifts[] = {0.0, 1e8, 1e9};
  for (size_t i = 0; i < sizeof(shifts) / sizeof(shifts[0]); i++) {
    double shift = shifts[i];
    printf(
        "Mean     = %lf\n"
        "Variance = %lf\n\n",
        exact_mean + shift, exact_variance);
    double shift_seq[n];
    for (size_t j = 0; j < n; j++) {
      shift_seq[j] = seq[j] + shift;
    }

    seq_data result;

    result = calc_bi(shift_seq, n);
    printf(
        "Bi-way approach:   Mean     = %lf (error = %lf)\n"
        "                   Variance = %lf (error = %lf)\n\n",
        result.mean, relative_error(result.mean, exact_mean + shift),
        result.variance, relative_error(result.variance, exact_variance));

    result = calc_naive(shift_seq, n);
    printf(
        "Naive approach:    Mean     = %lf (error = %lf)\n"
        "                   Variance = %lf (error = %lf)\n\n",
        result.mean, relative_error(result.mean, exact_mean + shift),
        result.variance, relative_error(result.variance, exact_variance));

    result = calc_naive(shift_seq, n);
    printf(
        "Welford approach:  Mean     = %lf (error = %lf)\n"
        "                   Variance = %lf (error = %lf)\n\n",
        result.mean, relative_error(result.mean, exact_mean + shift),
        result.variance, relative_error(result.variance, exact_variance));
  }

  return 0;
}

seq_data calc_bi(const double seq[const], const size_t n) {
  double mean = 0.0;
  double variance = 0.0;
  for (size_t i = 0; i < n; i++) {
    mean += seq[i];
    variance += seq[i] * seq[i];
  }
  mean /= n;
  variance = (variance / n) - (mean * mean);
  return (seq_data){.mean = mean, .variance = variance};
}

seq_data calc_naive(const double seq[const], const size_t n) {
  double mean = 0.0;
  for (size_t i = 0; i < n; i++) {
    mean += seq[i];
  }
  mean /= n;

  double variance = 0.0;
  for (size_t i = 0; i < n; i++) {
    variance += (mean - seq[i]) * (mean - seq[i]);
  }
  variance /= n;

  return (seq_data){.mean = mean, .variance = variance};
}

seq_data calc_welford(const double seq[const], const size_t n) {
  double mean = 0.0;
  double variance = 0.0;
  for (size_t i = 0; i < n; i++) {
    size_t no_of_elements = i + 1;
    double s1 = seq[i] - mean;
    mean += s1 / no_of_elements;
    double s2 = seq[i] - mean;
    variance += s1 * s2;
  }
  return (seq_data){.mean = mean, .variance = variance};
}

double relative_error(const double approx, const double exact) {
  return fabs(approx - exact) / exact;
}
