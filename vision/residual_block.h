//
// Created by dewe on 12/9/21.
//

#ifndef DEEP_NETWORKS_RESIDUAL_BLOCK_H
#define DEEP_NETWORKS_RESIDUAL_BLOCK_H

#include "base.h"

namespace sam_dn{
    class ResidualBlockImpl : public ModuleWithSizeInfoImpl
    {
    public:

        explicit ResidualBlockImpl(CNNOption opt);

        inline torch::Tensor forward ( torch::Tensor const& x) noexcept override{
            const auto& residual = torch::relu(x);
            auto out = torch::relu(conv1(x));
            out = conv2(x);
            out += residual;
            out = relu_out ? torch::relu(out) : out;
            return flatten_out ? torch::flatten(x, 1) : x;
        }

        inline TensorDict * forwardDict(TensorDict *x) noexcept override{
            x->insert_or_assign( m_Output, forward(x->at(m_Input)));
            return x;
        }

    private:
        torch::nn::Conv2d conv1{nullptr}, conv2{nullptr};
        bool flatten_out, relu_out{false};
    };

    TORCH_MODULE(ResidualBlock);
}
#endif //DEEP_NETWORKS_RESIDUAL_BLOCK_H
