// Copyright (C) 2018-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#pragma once

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

#include <ie_icnn_network.hpp>
#include <cpp/ie_cnn_network.h>
#include <details/ie_cnn_network_tools.h>

#include "low_precision_transformations/layer_transformation.hpp"
#include "low_precision_transformations/transformation_context.hpp"

namespace InferenceEngine {
namespace details {

IE_SUPPRESS_DEPRECATED_START

class INFERENCE_ENGINE_API_CLASS(ActivationTransformation) : public LayerTransformation {
public:
    ActivationTransformation(const Params& params) : LayerTransformation(params) {}
    ~ActivationTransformation() override {};
    void transform(TransformationContext& context, CNNLayer& layer) const override;
};

IE_SUPPRESS_DEPRECATED_END

}  // namespace details
}  // namespace InferenceEngine
