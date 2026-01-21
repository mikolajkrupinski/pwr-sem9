#include "fpk-element.hpp"

// --- IMPLEMENTACJA METOD POMOCNICZYCH (Internal Polynomial Arithmetic) ---

void FpkElement::normalizePoly(std::vector<FpElement>& poly) {
  if (poly.empty()) return;

  // Pobieramy zero z kontekstu ostatniego elementu
  FpElement zero(poly.back().getContext(), 0);

  while (poly.size() > 1 && poly.back() == zero) {
    poly.pop_back();
  }
}

bool FpkElement::isZeroPoly(const std::vector<FpElement>& poly) {
  if (poly.empty()) return true;
  FpElement zero(poly[0].getContext(), 0);
  return poly.size() == 1 && poly[0] == zero;
}

std::vector<FpElement> FpkElement::castNumToFpElements(
    FieldPtr ctx, const std::vector<num_t>& vec) {
  std::vector<FpElement> result;
  result.reserve(vec.size());
  for (const auto& val : vec) {
    result.emplace_back(FpElement(ctx, val));
  }
  return result;
}

std::vector<FpElement> FpkElement::polyMultiply(
    const std::vector<FpElement>& a, const std::vector<FpElement>& b) {
  if (a.empty() || b.empty()) return {};
  if (isZeroPoly(a) || isZeroPoly(b)) {
    return {FpElement(a.empty() ? nullptr : a[0].getContext(), 0)};
  }

  // Używamy size() jako liczby współczynników
  size_t size_a = a.size();
  size_t size_b = b.size();
  // Stopień wynikowy = (size_a - 1) + (size_b - 1)
  // Rozmiar wynikowy = Stopień + 1 = size_a + size_b - 1
  size_t result_size = size_a + size_b - 1;

  std::vector<FpElement> result(result_size, FpElement(a[0].getContext(), 0));

  for (size_t i = 0; i < size_a; ++i) {
    for (size_t j = 0; j < size_b; ++j) {
      result[i + j] += a[i] * b[j];
    }
  }
  normalizePoly(result);
  return result;
}

void FpkElement::polyDivide(std::vector<FpElement> dividend,
                            const std::vector<FpElement>& divisor,
                            std::vector<FpElement>& quotient,
                            std::vector<FpElement>& remainder) {
  if (dividend.empty() || divisor.empty())
    throw std::runtime_error("Empty polynomial in division");
  FieldPtr ctx = dividend[0].getContext();

  // 1. Kopia dzielnika i normalizacja
  std::vector<FpElement> div_copy = divisor;
  normalizePoly(div_copy);

  if (isZeroPoly(div_copy)) {
    throw std::runtime_error("Division by zero polynomial");
  }

  // 2. Normalizacja dzielnej
  normalizePoly(dividend);

  // Stopnie wielomianów
  int deg_divisor = (int)div_copy.size() - 1;
  int deg_dividend = (int)dividend.size() - 1;

  // 3. Jeśli stopień dzielnej jest mniejszy, wynik to 0, reszta to dzielna
  if (deg_dividend < deg_divisor) {
    quotient = {FpElement(ctx, 0)};
    remainder = dividend;
    return;
  }

  // Inicjalizacja ilorazu zerami. Rozmiar to różnica stopni + 1.
  quotient.assign(deg_dividend - deg_divisor + 1, FpElement(ctx, 0));

  // Odwrotność wiodącego współczynnika dzielnika (potrzebna do skalowania)
  const FpElement& lc_divisor = div_copy.back();
  FpElement lc_divisor_inv = inverse(lc_divisor);
  FpElement zero(ctx, 0);

  // 4. Główna pętla dzielenia (algorytm Dzielenia Wielomianów)
  // Używamy while, bo stopień dividend może spaść o więcej niż 1 w jednym kroku
  while (deg_dividend >= deg_divisor && !isZeroPoly(dividend)) {
    const FpElement& lc_dividend = dividend.back();

    // Obliczamy współczynnik dla aktualnej potęgi ilorazu
    // q(x) = LC(dividend) / LC(divisor)
    FpElement factor = lc_dividend * lc_divisor_inv;

    // Wykładnik potęgi x, o którą musimy przesunąć dzielnik
    int deg_diff = deg_dividend - deg_divisor;

    // Zapisujemy współczynnik w ilorazie
    quotient[deg_diff] = factor;

    // Odejmujemy (factor * divisor * x^deg_diff) od dividend
    // Zamiast przesuwać cały wektor, operujemy na indeksach
    for (int i = 0; i <= deg_divisor; ++i) {
      // dividend[i + deg_diff] -= factor * divisor[i]
      dividend[i + deg_diff] -= factor * div_copy[i];
    }

    // Ważne: Usuwamy wiodące zera, aby zaktualizować stopień wielomianu
    normalizePoly(dividend);
    deg_dividend = (int)dividend.size() - 1;
  }

  remainder = dividend;
}

std::vector<FpElement> FpkElement::polySubtract(
    const std::vector<FpElement>& a, const std::vector<FpElement>& b) {
  size_t max_size = std::max(a.size(), b.size());
  FieldPtr ctx = a.empty() ? b[0].getContext() : a[0].getContext();
  std::vector<FpElement> result(max_size, FpElement(ctx, 0));
  FpElement zero(ctx, 0);

  for (size_t i = 0; i < max_size; ++i) {
    const FpElement& coeff_a = (i < a.size()) ? a[i] : zero;
    const FpElement& coeff_b = (i < b.size()) ? b[i] : zero;
    result[i] = coeff_a - coeff_b;
  }
  normalizePoly(result);
  return result;
}

std::vector<FpElement> FpkElement::polyScalarMultiply(
    const std::vector<FpElement>& poly, const FpElement& scalar) {
  FpElement zero(scalar.getContext(), 0);
  if (scalar == zero) {
    return {zero};
  }
  std::vector<FpElement> result = poly;
  for (FpElement& coeff : result) {
    coeff = coeff * scalar;
  }
  normalizePoly(result);  // Na wypadek gdyby skalar był 0, choć jest if wyżej
  return result;
}

void FpkElement::polyExtendedEuclidean(std::vector<FpElement> a,
                                       std::vector<FpElement> b,
                                       std::vector<FpElement>& s,
                                       std::vector<FpElement>& t) {
  FieldPtr ctx = a.empty() ? b[0].getContext() : a[0].getContext();

  std::vector<FpElement> r0 = a;
  std::vector<FpElement> r1 = b;
  std::vector<FpElement> s0 = {FpElement(ctx, 1)};
  std::vector<FpElement> s1 = {FpElement(ctx, 0)};
  // t0 i t1 zazwyczaj nie są potrzebne do obliczenia samego s (odwrotności),
  // ale w pełnym EEA by były. Tutaj skupiamy się na s.

  std::vector<FpElement> q;

  while (!isZeroPoly(r1)) {
    std::vector<FpElement> remainder;

    polyDivide(r0, r1, q, remainder);

    std::vector<FpElement> q_times_s1 = polyMultiply(q, s1);
    std::vector<FpElement> s_new = polySubtract(s0, q_times_s1);

    r0 = r1;
    r1 = remainder;

    s0 = s1;
    s1 = s_new;
  }

  const FpElement& gcd_constant = r0[0];
  FpElement zero(ctx, 0);

  if (gcd_constant == zero) {
    throw std::runtime_error("GCD is zero polynomial, inverse does not exist.");
  }

  FpElement gcd_inv = inverse(gcd_constant);

  // Normalizacja wyniku tak, aby GCD było 1 (moniczne)
  s = polyScalarMultiply(s0, gcd_inv);
  t = {};
}

// --- IMPLEMENTACJA OPERATORÓW FPK ---

FpkElement FpkElement::operator+(const FpkElement& fe) const {
  checkContext(fe);
  size_t k = std::max(coefficients.size(), fe.coefficients.size());
  std::vector<FpElement> result_coeffs;
  result_coeffs.reserve(k);

  FpElement zero(context, 0);

  for (size_t i = 0; i < k; ++i) {
    const FpElement& a = (i < coefficients.size()) ? coefficients[i] : zero;
    const FpElement& b =
        (i < fe.coefficients.size()) ? fe.coefficients[i] : zero;
    result_coeffs.push_back(a + b);
  }

  return FpkElement(context, result_coeffs);
}

FpkElement FpkElement::operator-() const {
  std::vector<FpElement> result_coeffs = coefficients;
  for (FpElement& coeff : result_coeffs) {
    coeff = -coeff;
  }
  return FpkElement(context, result_coeffs);
}

FpkElement FpkElement::operator-(const FpkElement& fe) const {
  checkContext(fe);
  return *this + (-fe);
}

FpkElement FpkElement::operator*(const FpkElement& fe) const {
  checkContext(fe);

  // 1. Mnożenie wielomianów (wynik może mieć stopień >= stopnia ciała)
  std::vector<FpElement> product_coeffs =
      polyMultiply(this->coefficients, fe.coefficients);

  // 2. Pobieramy wielomian redukujący Q(x) z kontekstu
  const std::vector<num_t>& poly_nums = context->getPoly();
  std::vector<FpElement> mod_poly = castNumToFpElements(context, poly_nums);

  // 3. Sprawdzamy czy konieczna jest redukcja
  // Stopień wielomianu modulo to mod_poly.size() - 1.
  if (product_coeffs.size() < mod_poly.size()) {
    return FpkElement(context, product_coeffs);
  }

  // 4. Redukcja (Dzielenie z resztą)
  std::vector<FpElement> quotient_coeffs;
  std::vector<FpElement> remainder_coeffs;

  polyDivide(product_coeffs, mod_poly, quotient_coeffs, remainder_coeffs);

  return FpkElement(context, remainder_coeffs);
}

FpkElement FpkElement::operator/(const FpkElement& fe) const {
  checkContext(fe);
  return *this * inverse(fe);
}

FpkElement& FpkElement::operator+=(const FpkElement& fe) {
  *this = *this + fe;
  return *this;
}

FpkElement& FpkElement::operator-=(const FpkElement& fe) {
  *this = *this - fe;
  return *this;
}

FpkElement& FpkElement::operator*=(const FpkElement& fe) {
  *this = *this * fe;
  return *this;
}

FpkElement& FpkElement::operator/=(const FpkElement& fe) {
  *this = *this / fe;
  return *this;
}

FpkElement& FpkElement::operator=(const FpkElement& fe) {
  if (this == &fe) return *this;
  if (this->context && fe.context && *this->context != *fe.context) {
    throw std::invalid_argument(
        "Cannot assign FpkElement from different field.");
  }
  this->context = fe.context;
  this->coefficients = fe.coefficients;
  return *this;
}

// --- FUNKCJE FRIEND ---

FpkElement inverse(const FpkElement& fe) {
  if (FpkElement::isZeroPoly(fe.coefficients)) {
    throw std::runtime_error("Division by zero in Fpk");
  }

  std::vector<FpElement> s, t;

  // Pobieramy wielomian redukujący Q(x)
  std::vector<FpElement> mod_poly = FpkElement::castNumToFpElements(
      fe.getContext(), fe.getContext()->getPoly());

  // Szukamy s(x) takiego, że: s(x)*fe(x) + t(x)*mod_poly(x) = 1
  FpkElement::polyExtendedEuclidean(fe.coefficients, mod_poly, s, t);

  return FpkElement(fe.getContext(), s);
}

FpkElement power(const FpkElement& fe, num_t exp) {
  if (exp == 0) {
    return FpkElement(fe.getContext(), {FpElement(fe.getContext(), 1)});
  }

  if (FpkElement::isZeroPoly(fe.coefficients)) {
    return fe;
  }

  FpkElement result(fe.getContext(), {FpElement(fe.getContext(), 1)});
  FpkElement base = fe;

  while (exp > 0) {
    if (exp % 2 == 1) {
      result *= base;
    }
    base *= base;
    exp /= 2;
  }

  return result;
}

std::string FpkElement::toStr() const {
  std::string result = "";
  for (const auto& coeff : coefficients) {
    result += coeff.toStr() + ",";
  }
  if (!result.empty()) result.pop_back();
  return result;
}

void FpkElement::fromStr(const std::string& str) {
  if (!context)
    throw std::runtime_error("Context not initialized before fromStr");
  std::vector<FpElement> coeffs;
  size_t start = 0;
  size_t end = str.find(',');
  while (end != std::string::npos) {
    std::string coeff_str = str.substr(start, end - start);
    FpElement coeff(context);
    coeff.fromStr(coeff_str);
    coeffs.push_back(coeff);
    start = end + 1;
    end = str.find(',', start);
  }
  std::string coeff_str = str.substr(start);
  FpElement coeff(context);
  coeff.fromStr(coeff_str);
  coeffs.push_back(coeff);

  this->coefficients = coeffs;
  normalizePoly(this->coefficients);
}