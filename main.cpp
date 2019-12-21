#include <iostream>
#include <vector>
#include <random>
#include <iomanip>

using Vector = std::vector<double>;

std::mt19937 gen{std::random_device{}()};

Vector generateAlpha(size_t r)
{
    std::uniform_real_distribution<double> fun{0., 1.};
    if (r == 3) {
        double middle = fun(gen);
        return {0.5 * (1 - middle), middle, 0.5 * (1 - middle)};
    } else if (r == 5) {
        double middle = fun(gen);
        
        std::uniform_real_distribution<double> fun1{0., 1 - middle};
        double left = 0.5 * fun1(gen);
        
        return {0.5 * (1 - middle - left), left, middle, left, 0.5 * (1 - middle - left)};
    }
    
    return {};
}

// Зашумленность
double getNoise(const Vector &vector)
{
    double value = 0;
    for (size_t i = 1; i < vector.size(); ++i) {
        value += (vector[i] - vector[i - 1]) * (vector[i] - vector[i - 1]);
    }
    
    return std::sqrt(value / vector.size());
}

// Отличие
double getMirror(const Vector &nonoise, const Vector &noise)
{
    double value = 0;
    for (size_t i = 0; i < noise.size(); ++i) {
        value += (noise[i] - nonoise[i]) * (noise[i] - nonoise[i]);
    }
    
    return std::sqrt(value / noise.size());
    
}
Vector removeTrashFromFunction(const Vector &function, const Vector &alpha)
{
    Vector result = function;
    
    size_t m = alpha.size() / 2;
    for (size_t i = m; i <= 100 - m; ++i) {
        double sum = 0;
        for (size_t j = i - m; j <= i + m; ++j) {
            sum += function[j] * function[j] * alpha[j - i + m];
        }
        result[i] = std::sqrt(sum);
    }
    
    return result;
}

Vector trashFunction(const Vector &function)
{
    std::uniform_real_distribution<double> fun{-0.25, 0.25};
    
    Vector result;
    for (const auto &v : function) {
        result.push_back(v + fun(gen));
    }
    return result;
}

void printFunction(const Vector &function)
{
    std::cout << "Copy (yotx.ru):\n    ";
    for (size_t i = 0; i <= 100; ++i) {
        std::cout << "(" << i / 100. * 3.1415 << ";" << function[i] << ") ";
    }
    std::cout << std::endl << std::endl;
}

void produce3(const Vector &noise)
{
    static const size_t N = std::log(1 - 0.95) / std::log(1 - 0.01 / 3.1415);
    
    double bestLambdaEver = 0;
    double bestDistanceEver = 999999;
    Vector bestAlphaEver;
    double bestCriteriaEver = 0;
    std::pair<double, double> bestWdEver;
    std::vector<std::pair<double, double>> wds;
    std::cout << "|     h      |     dis    |          alpha         |      w     |    d       |\n"
    << "+ ---------- + ---------- + ---------------------- + ---------- + ---------- +\n"
    << "| ";
    for (size_t lambda = 0; lambda <= 10; ++lambda) {
        std::cout << std::setw(10) << lambda / 10. << " | ";
        
        struct
        {
            Vector alpha{};
            double w{0};
            double d{0};
            double criteria{99999};
        } best;
        
        for (size_t i = 0; i < N; ++i) {
            auto alpha = generateAlpha(3);
            auto nonoise = removeTrashFromFunction(noise, alpha);
            double w = getNoise(nonoise);
            double d = getMirror(nonoise, noise);
            double criteria = lambda * w + (1 - lambda / 10.) * d;
            if (criteria < best.criteria) {
                best.alpha = std::move(alpha);
                best.w = w;
                best.d = d;
                best.criteria = criteria;
            }
        }
        
        double distance = std::sqrt(best.w * best.w + best.d * best.d);
        std::cout << std::setw(10) << distance << " | "
        << std::setw(6) << std::setprecision(4) << std::fixed << best.alpha[0] << ", "
        << std::setw(6) << std::setprecision(4) << std::fixed << best.alpha[1] << ", "
        << std::setw(6) << std::setprecision(4) << std::fixed << best.alpha[2] << " | "
        << std::setw(10) << best.w << " | "
        << std::setw(10) << best.d << " |\n"
        << "+ ---------- + ---------- + ---------------------- + ---------- + ---------- +\n"
        << "| ";
        
        wds.emplace_back(best.w, best.d);
        if (distance < bestDistanceEver) {
            bestDistanceEver = distance;
            bestAlphaEver = best.alpha;
            bestLambdaEver = lambda / 10.;
            bestCriteriaEver = best.criteria;
            bestWdEver = {best.w, best.d};
        }
    }
    
    std::cout << "Best row: "
    << "\n    h*:   " << bestLambdaEver
    << "\n    dis:  " << bestDistanceEver
    << "\n    J:    " << bestCriteriaEver
    << "\n    w, d: " << bestWdEver.first << " " << bestWdEver.second << std::endl;
    
    std::cout << "\nBest be 3 (function): \n";
    printFunction(removeTrashFromFunction(noise, bestAlphaEver));
    
    std::cout << "\nBest be 3 (points): \n";
    std::cout << "Copy (yotx.ru):\n    ";
    for (const auto &pair : wds) {
        std::cout << "(" << pair.first << ";" << pair.second << ") ";
    }
    std::cout << std::endl << std::endl;
}

void produce5(const Vector &noise)
{
    static const size_t N = std::log(1 - 0.95) / std::log(1 - 0.01 / 3.1415);
    
    double bestLambdaEver = 0;
    double bestDistanceEver = 999999;
    Vector bestAlphaEver;
    double bestCriteriaEver = 0;
    std::pair<double, double> bestWdEver;
    std::vector<std::pair<double, double>> wds;
    std::cout << "|     h      |     dis    |                  alpha                 |      w     |    d       |\n"
    << "+ ---------- + ---------- + -------------------------------------- + ---------- + ---------- +\n"
    << "| ";
    for (size_t lambda = 0; lambda <= 10; ++lambda) {
        std::cout << std::setw(10) << lambda / 10. << " | ";
        
        struct
        {
            Vector alpha{};
            double w{0};
            double d{0};
            double criteria{99999};
        } best;
        
        for (size_t i = 0; i < N; ++i) {
            auto alpha = generateAlpha(5);
            auto nonoise = removeTrashFromFunction(noise, alpha);
            double w = getNoise(nonoise);
            double d = getMirror(nonoise, noise);
            double criteria = lambda * w + (1 - lambda / 10.) * d;
            if (criteria < best.criteria) {
                best.alpha = std::move(alpha);
                best.w = w;
                best.d = d;
                best.criteria = criteria;
            }
        }
        
        double distance = std::sqrt(best.w * best.w + best.d * best.d);
        std::cout << std::setw(10) << distance << " | "
        << std::setw(6) << std::setprecision(4) << std::fixed << best.alpha[0] << ", "
        << std::setw(6) << std::setprecision(4) << std::fixed << best.alpha[1] << ", "
        << std::setw(6) << std::setprecision(4) << std::fixed << best.alpha[2] << ", "
        << std::setw(6) << std::setprecision(4) << std::fixed << best.alpha[3] << ", "
        << std::setw(6) << std::setprecision(4) << std::fixed << best.alpha[4] << " | "
        << std::setw(10) << best.w << " | "
        << std::setw(10) << best.d << " |\n"
        << "+ ---------- + ---------- + -------------------------------------- + ---------- + ---------- +\n"
        << "| ";
        
        wds.emplace_back(best.w, best.d);
        if (distance < bestDistanceEver) {
            bestDistanceEver = distance;
            bestAlphaEver = best.alpha;
            bestLambdaEver = lambda / 10.;
            bestCriteriaEver = best.criteria;
            bestWdEver = {best.w, best.d};
        }
    }
    
    std::cout << "Best row: "
    << "\n    h*:   " << bestLambdaEver
    << "\n    dis:  " << bestDistanceEver
    << "\n    J:    " << bestCriteriaEver
    << "\n    w, d: " << bestWdEver.first << " " << bestWdEver.second << std::endl;
    
    std::cout << "\nBest be 5 (function): \n";
    printFunction(removeTrashFromFunction(noise, bestAlphaEver));
    
    std::cout << "\nBest be 5 (points): \n";
    std::cout << "Copy (yotx.ru):\n    ";
    for (const auto &pair : wds) {
        std::cout << "(" << pair.first << ";" << pair.second << ") ";
    }
    std::cout << std::endl << std::endl;
}

int main()
{
    Vector function;
    for (size_t i = 0; i <= 100; ++i) {
        function.push_back(std::sin(i / 100. * 3.1415) + 0.5);
    }
    
    Vector noise = trashFunction(function);
    
    std::cout << "Source: \n";
    printFunction(function);
    std::cout << "Noise: \n";
    printFunction(noise);
    
    produce3(noise);
    
    produce5(noise);
    
    return 0;
}
