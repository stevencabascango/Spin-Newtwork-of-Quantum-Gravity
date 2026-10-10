#pragma once
#include <cmath>
#include <stdexcept>
namespace snl {
struct PhysicsParameters {
    double spring=2.0;
    double repulsion=12000.0;
    double damping=2.0;
    double softening=5.0;
    static constexpr double timeStep=1.0/120.0;
    void validate() const {
        if(!std::isfinite(spring)||spring<0||!std::isfinite(repulsion)||repulsion<0||
           !std::isfinite(damping)||damping<0||!std::isfinite(softening)||softening<=0)
            throw std::invalid_argument("Invalid physics parameters");
    }
};
}
