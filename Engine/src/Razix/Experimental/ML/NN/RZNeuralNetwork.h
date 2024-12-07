#pragma once

#include <Eigen/Dense>

#include "Razix/Experimental/ML/NN/RZNNLayer.h"

namespace Razix {
    namespace Experimental {
        namespace ML {

            template<typename T>
            class RZNeuralNetwork
            {
            public:
                RZNeuralNetwork() {}
                ~RZNeuralNetwork() {}

                void addLayer(RZNNLayer<T>* layer)
                {
                    m_Layers.push_back(layer);
                }

                Vector<T> predict(const Vector<T>& input)
                {
                    Vector<T> output = input;
                    for (const auto& layer: m_Layers) {
                        output = layer->forward(output);
                    }
                    return output;
                }

            private:
                std::vector<RZNNLayer<T>*> m_Layers;
            };

        }    // namespace ML
    }    // namespace Experimental
}    // namespace Razix