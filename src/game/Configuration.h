#ifndef TRIPPIN_CONFIGURATION_H
#define TRIPPIN_CONFIGURATION_H

#include <string>
#include <sstream>
#include "nlohmann/json.hpp"

namespace trippin {
    struct Configuration {
        struct Scale {
            std::string name;
            double multiplier;
            int minWidth;
        };

        int spiritSecondsBehind;
        int tickPeriod;
        int spiritClockTickPeriod;
        int activationProximity;
        int deactivationProximity;
        int pointsPerSecond;
        int shakeDuration;
        int shakeHertz;
        int shakeAmplitude;
        std::string map;
        std::string loadMap;
        std::string loadAutoPlay;
        std::vector<Scale> scales;
        void load(const std::string &name);
        static std::string getConfigFile(const std::string &name);

        double ticksPerSecond() const;
        double engineTicksPerSpiritClockTick() const;
        double pointsPerTick() const;
    };

    void from_json(const nlohmann::json &j, Configuration &config);
}

#endif
