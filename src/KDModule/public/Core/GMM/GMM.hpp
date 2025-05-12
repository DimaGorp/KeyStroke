#ifndef GMM_HPP
#define GMM_HPP
#include <iostream>
#include <vector>
#include <cstring>

struct Vec2 { double x, y; Vec2(double x_ = 0, double y_ = 0) : x(x_), y(y_) {} };
struct Mat2 { double xx, yy; Mat2(double xx_ = 1, double yy_ = 1) : xx(xx_), yy(yy_) {} double det() const { return xx * yy; } Mat2 inverse() const { return Mat2(1.0 / xx, 1.0 / yy); } };

class GMM {
private:
    int k;
    std::vector<Vec2> means;
    std::vector<Mat2> covs;
    std::vector<double> weights;
    static constexpr double PI = 3.141592653589793;

    std::vector<std::vector<double>> eStep(const std::vector<Vec2>& data) const;
    void mStep(const std::vector<Vec2>& data, const std::vector<std::vector<double>>& resp);

public:
    GMM(int components = 2);
    double gaussian(const Vec2& x, const Vec2& mean, const Mat2& cov) const;
    double logLikelihood(const std::vector<Vec2>& data) const;
    void train(const std::vector<Vec2>& data, int max_iter = 100, double tol = 1e-4);
    void save(const std::string& filename) const;
    void load(const std::string& filename);
};

#endif