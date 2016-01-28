#ifndef PREDICTOR_H
#define PREDICTOR_H

struct Predictor
{
    double getPrediction(double value) {
        double delta = value - lastValue;

        if (valueCount == 2) {
            trendDelta = delta - lastDelta;
        } else if (valueCount > 2) {
            delta = smoothFactor * delta + (1.0 - smoothFactor) * (lastDelta + trendDelta);
            trendDelta = trendFactor * (delta - lastDelta) + (1.0 - trendFactor) * trendDelta;

            double slowstartScale = pow(1.0 - 1.0 / valueCount, 3);
            value += delta + trendDelta * predictionFactor * slowstartScale;
        }

        lastValue = value;
        lastDelta = delta;
        valueCount++;

        return value;
    }

    void reset() {
        valueCount = -5;
        lastValue = 0;
        lastDelta = 0;
        trendDelta = 0;
    }

    double lastValue = 0;
    double lastDelta = 0;
    double trendDelta = 0;

    double predictionFactor = 7.5;
    double smoothFactor = 0.098;
    double trendFactor = 0.054;

    int valueCount = 0;
};

#endif // PREDICTOR_H

