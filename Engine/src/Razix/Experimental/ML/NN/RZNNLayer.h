#pragma once

#include <Eigen/Dense>

namespace Razix {
    namespace Experimental {
        namespace ML {

            template<typename T>
            using Matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;

            template<typename T>
            using Vector = Eigen::Matrix<T, Eigen::Dynamic, 1>;

            // Define different activation functions
            // ex. Relu, Sigmoid etx.
            namespace Activation {
                template<typename T>
                T relu(const T& x)
                {
                    return x.array().max(0.0);
                }

                template<typename T>
                T sigmoid(const T& x)
                {
                    return 1.0 / (1.0 + (-x.array()).exp());
                }
            }    // namespace Activation

            template<typename T>
            class RZNNLayer
            {
            public:
                RZNNLayer() {}
                ~RZNNLayer() {}

                void create(u32 inputSize, u32 outputSize, std::function<T(const T&)> activation)
                {
                }

                virtual Vector<T> forward(const Vector<T>& input)       = 0;
                virtual Vector<T> backpropagate(const Vector<T>& input) = 0;

                virtual std::string getType() const = 0;

                const Matrix<T>& getWeights() const { return weights; }
                const Vector<T>& getBiases() const { return biases; }

            private:
                Matrix<T> weights;
                Vector<T> biases;

                Vector<T> lastInput;    // Cache input for backward pass
                Vector<T> lastZ;        // Cache Z = Wx + b for derivative computation

                std::function<T(const T&)> activationFn;
            };

        }    // namespace ML
    }        // namespace Experimental
}    // namespace Razix