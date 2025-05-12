#include "GMM/GMM.hpp"
#include <fstream>
#include <random>
#include <cmath>

GMM::GMM(int components) : k(components) {
    means.resize(k, Vec2());
    covs.resize(k, Mat2());
    weights.resize(k, 1.0 / k);
}

double GMM::gaussian(const Vec2& x, const Vec2& mean, const Mat2& cov) const {
    double dx = x.x - mean.x;
    double dy = x.y - mean.y;
    double det = cov.det();
    double inv_xx = cov.inverse().xx;
    double inv_yy = cov.inverse().yy;
    double exponent = -0.5 * (dx * dx * inv_xx + dy * dy * inv_yy);
    return (1.0 / (2.0 * PI * std::sqrt(det))) * std::exp(exponent);
}

double GMM::logLikelihood(const std::vector<Vec2>& data) const {
    double ll = 0.0;
    for (const auto& x : data) {
        double p = 0.0;
        for (int i = 0; i < k; ++i) {
            p += weights[i] * gaussian(x, means[i], covs[i]);
        }
        ll += std::log(p);
    }
    return ll;
}

std::vector<std::vector<double>> GMM::eStep(const std::vector<Vec2>& data) const {
    std::vector<std::vector<double>> resp(data.size(), std::vector<double>(k));
    for (size_t i = 0; i < data.size(); ++i) {
        double sum = 0.0;
        for (int j = 0; j < k; ++j) {
            resp[i][j] = weights[j] * gaussian(data[i], means[j], covs[j]);
            sum += resp[i][j];
        }
        for (int j = 0; j < k; ++j) {
            resp[i][j] /= sum; // Normalize
        }
    }
    return resp;
}

void GMM::mStep(const std::vector<Vec2>& data, const std::vector<std::vector<double>>& resp) {
    std::vector<double> Nk(k, 0.0);
    for (int j = 0; j < k; ++j) {
        Nk[j] = 0.0;
        means[j] = Vec2(0, 0);
        covs[j] = Mat2(0, 0);
        for (size_t i = 0; i < data.size(); ++i) {
            Nk[j] += resp[i][j];
            means[j].x += resp[i][j] * data[i].x;
            means[j].y += resp[i][j] * data[i].y;
        }
        means[j].x /= Nk[j];
        means[j].y /= Nk[j];

        for (size_t i = 0; i < data.size(); ++i) {
            double dx = data[i].x - means[j].x;
            double dy = data[i].y - means[j].y;
            covs[j].xx += resp[i][j] * dx * dx;
            covs[j].yy += resp[i][j] * dy * dy;
        }
        covs[j].xx /= Nk[j];
        covs[j].yy /= Nk[j];
        weights[j] = Nk[j] / data.size();
    }
}

void GMM::train(const std::vector<Vec2>& data, int max_iter, double tol) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 100);
    for (int i = 0; i < k; ++i) {
        means[i] = Vec2(dis(gen), dis(gen));
        covs[i] = Mat2(10, 10); // Initial variance
    }

    double prev_ll = -std::numeric_limits<double>::infinity();
    for (int iter = 0; iter < max_iter; ++iter) {
        auto resp = eStep(data);
        mStep(data, resp);
        double ll = logLikelihood(data);
        if (std::abs(ll - prev_ll) < tol) break;
        prev_ll = ll;
    }
}

void GMM::save(const std::string& filename) const {
    std::ofstream out(filename);
    out << k << "\n";
    for (int i = 0; i < k; ++i) {
        out << means[i].x << " " << means[i].y << " " << covs[i].xx << " " << covs[i].yy << " " << weights[i] << "\n";
    }
}

void GMM::load(const std::string& filename) {
    std::ifstream in(filename);
    in >> k;
    means.resize(k);
    covs.resize(k);
    weights.resize(k);
    for (int i = 0; i < k; ++i) {
        in >> means[i].x >> means[i].y >> covs[i].xx >> covs[i].yy >> weights[i];
    }
}