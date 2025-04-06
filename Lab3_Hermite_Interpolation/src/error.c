#include "../include/error.h"

ErrorResult calculateError(double trueValues[], double interpValues[], int numPoints) {
    ErrorResult result;
    result.max_error = 0.0;
    result.mean_squared_error = 0.0;

    for (int i = 0; i < numPoints; i++) {
        double error = fabs(trueValues[i] - interpValues[i]);
        if (error > result.max_error) {
            result.max_error = error;
        }
        result.mean_squared_error += error * error;
    }

    result.mean_squared_error /= numPoints;
    return result;
}