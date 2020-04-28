// Copyright (C) 2018-2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//

#include "ngraph_functions/subgraph_builders.hpp"
#include "myriad_layers_reshape_test.hpp"

TEST_F(myriadEliminateReshapeTests_nightly, SplitConvConcat) {
    ASSERT_NO_THROW(_cnnNetwork = InferenceEngine::CNNNetwork(ngraph::builder::subgraph::makeSplitConvConcat()));

    StatusCode st;

    ASSERT_NO_THROW(st = _vpuPluginPtr->LoadNetwork(_exeNetwork, _cnnNetwork,
                                                        {
                                                            {
                                                                VPU_CONFIG_KEY(PERF_REPORT_MODE),
                                                                VPU_CONFIG_VALUE(PER_STAGE)
                                                            },
                                                            {
                                                                VPU_CONFIG_KEY(HW_STAGES_OPTIMIZATION),
                                                                CONFIG_VALUE(NO)
                                                            },
                                                            {
                                                                CONFIG_KEY(PERF_COUNT),
                                                                CONFIG_VALUE(YES)
                                                            }
                                                        },
                                                      &_resp));
    ASSERT_EQ(StatusCode::OK, st) << _resp.msg;

    ASSERT_NO_THROW(st = _exeNetwork->CreateInferRequest(_inferRequest, &_resp));
    ASSERT_EQ(StatusCode::OK, st) << _resp.msg;

    ASSERT_NO_THROW(st = _inferRequest->Infer(&_resp));
    ASSERT_EQ(StatusCode::OK, st) << _resp.msg;

    std::map<std::string, InferenceEngineProfileInfo> perfMap;
    ASSERT_NO_THROW(st = _inferRequest->GetPerformanceCounts(perfMap, &_resp));
    ASSERT_EQ(StatusCode::OK, st) << _resp.msg;

    auto layerInfo = perfMap["MobilenetV1/Logits/SpatialSqueeze"];
    ASSERT_EQ(InferenceEngineProfileInfo::NOT_RUN, layerInfo.status);
}

TEST_F(myriadLayersTests_nightly, ReshapeAfterConcat_Eliminate) {
    std::string model = R"V0G0N(
        <net name="ReshapeAfterConcat_Eliminate" version="2" batch="1">
            <layers>
                <layer name="input1" type="Input" precision="FP16" id="1">
                    <output>
                        <port id="1">
                            <dim>1</dim>
                            <dim>30</dim>
                        </port>
                    </output>
                </layer>
                <layer name="input2" type="Input" precision="FP16" id="2">
                    <output>
                        <port id="2">
                            <dim>1</dim>
                            <dim>20</dim>
                        </port>
                    </output>
                </layer>
                <layer name="input3" type="Input" precision="FP16" id="3">
                    <output>
                        <port id="3">
                            <dim>1</dim>
                            <dim>10</dim>
                        </port>
                    </output>
                </layer>

                <layer name="input1_copy" type="Power" precision="FP16" id="4">
                    <power_data power="1" scale="1" shift="0"/>
                    <input>
                        <port id="4">
                            <dim>1</dim>
                            <dim>30</dim>
                        </port>
                    </input>
                    <output>
                        <port id="5">
                            <dim>1</dim>
                            <dim>30</dim>
                        </port>
                    </output>
                </layer>
                <layer name="input2_copy" type="Power" precision="FP16" id="5">
                    <power_data power="1" scale="1" shift="0"/>
                    <input>
                        <port id="6">
                            <dim>1</dim>
                            <dim>20</dim>
                        </port>
                    </input>
                    <output>
                        <port id="7">
                            <dim>1</dim>
                            <dim>20</dim>
                        </port>
                    </output>
                </layer>
                <layer name="input3_copy" type="Power" precision="FP16" id="6">
                    <power_data power="1" scale="1" shift="0"/>
                    <input>
                        <port id="8">
                            <dim>1</dim>
                            <dim>10</dim>
                        </port>
                    </input>
                    <output>
                        <port id="9">
                            <dim>1</dim>
                            <dim>10</dim>
                        </port>
                    </output>
                </layer>

                <layer name="concat" type="Concat" precision="FP16" id="7">
                    <concat_data axis="1"/>
                    <input>
                        <port id="10">
                            <dim>1</dim>
                            <dim>30</dim>
                        </port>
                        <port id="11">
                            <dim>1</dim>
                            <dim>20</dim>
                        </port>
                        <port id="12">
                            <dim>1</dim>
                            <dim>10</dim>
                        </port>
                    </input>
                    <output>
                        <port id="13">
                            <dim>1</dim>
                            <dim>60</dim>
                        </port>
                    </output>
                </layer>

                <layer name="reshape" type="Reshape" precision="FP16" id="8">
                    <data dim="0,-1,30" axis="0" num_axes="-1"/>
                    <input>
                        <port id="14">
                            <dim>1</dim>
                            <dim>60</dim>
                        </port>
                    </input>
                    <output>
                        <port id="15">
                            <dim>1</dim>
                            <dim>2</dim>
                            <dim>30</dim>
                        </port>
                    </output>
                </layer>

                <layer name="reshape_copy" type="Power" precision="FP16" id="9">
                    <power_data power="1" scale="1" shift="0"/>
                    <input>
                        <port id="16">
                            <dim>1</dim>
                            <dim>2</dim>
                            <dim>30</dim>
                        </port>
                    </input>
                    <output>
                        <port id="17">
                            <dim>1</dim>
                            <dim>2</dim>
                            <dim>30</dim>
                        </port>
                    </output>
                </layer>
            </layers>
            <edges>
                <edge from-layer="1" from-port="1" to-layer="4" to-port="4"/>
                <edge from-layer="2" from-port="2" to-layer="5" to-port="6"/>
                <edge from-layer="3" from-port="3" to-layer="6" to-port="8"/>

                <edge from-layer="4" from-port="5" to-layer="7" to-port="10"/>
                <edge from-layer="5" from-port="7" to-layer="7" to-port="11"/>
                <edge from-layer="6" from-port="9" to-layer="7" to-port="12"/>

                <edge from-layer="7" from-port="13" to-layer="8" to-port="14"/>

                <edge from-layer="8" from-port="15" to-layer="9" to-port="16"/>
            </edges>
        </net>
    )V0G0N";

    StatusCode st;

    ASSERT_NO_THROW(readNetwork(model));

    const auto& network = _cnnNetwork;

    _inputsInfo = network.getInputsInfo();
    _inputsInfo["input1"]->setPrecision(Precision::FP16);
    _inputsInfo["input2"]->setPrecision(Precision::FP16);
    _inputsInfo["input3"]->setPrecision(Precision::FP16);

    _outputsInfo = network.getOutputsInfo();
    _outputsInfo["reshape_copy"]->setPrecision(Precision::FP16);

    ASSERT_NO_THROW(st = _vpuPluginPtr->LoadNetwork(_exeNetwork, network, { {VPU_CONFIG_KEY(PERF_REPORT_MODE), VPU_CONFIG_VALUE(PER_STAGE)},
                                                                              {VPU_CONFIG_KEY(HW_STAGES_OPTIMIZATION), CONFIG_VALUE(NO)},
                                                                              {CONFIG_KEY(PERF_COUNT), CONFIG_VALUE(YES)} }, &_resp));
    ASSERT_EQ(StatusCode::OK, st) << _resp.msg;
    ASSERT_NE(_exeNetwork, nullptr) << _resp.msg;

    ASSERT_NO_THROW(st = _exeNetwork->CreateInferRequest(_inferRequest, &_resp));
    ASSERT_EQ(StatusCode::OK, st) << _resp.msg;

    Blob::Ptr input1;
    ASSERT_NO_THROW(st = _inferRequest->GetBlob("input1", input1, &_resp));
    ASSERT_EQ(StatusCode::OK, st) << _resp.msg;
    GenRandomData(input1);

    Blob::Ptr input2;
    ASSERT_NO_THROW(st = _inferRequest->GetBlob("input2", input2, &_resp));
    ASSERT_EQ(StatusCode::OK, st) << _resp.msg;
    GenRandomData(input2);

    Blob::Ptr input3;
    ASSERT_NO_THROW(st = _inferRequest->GetBlob("input3", input3, &_resp));
    ASSERT_EQ(StatusCode::OK, st) << _resp.msg;
    GenRandomData(input3);

    ASSERT_NO_THROW(st = _inferRequest->Infer(&_resp));
    ASSERT_EQ(StatusCode::OK, st) << _resp.msg;

    Blob::Ptr output;
    ASSERT_NO_THROW(st = _inferRequest->GetBlob("reshape_copy", output, &_resp));
    ASSERT_EQ(StatusCode::OK, st) << _resp.msg;

    _refBlob = make_shared_blob<ie_fp16>({Precision::FP16, output->getTensorDesc().getDims(), Layout::ANY});
    _refBlob->allocate();
    {
        ie_fp16* dst_ptr = _refBlob->buffer().as<ie_fp16*>();
        int dst_offset = 0;

        auto concat = [&](const Blob::Ptr& src) {
            const ie_fp16* src_ptr = src->cbuffer().as<const ie_fp16*>();
            int num = src->getTensorDesc().getDims().back();
            std::copy_n(src_ptr, num, dst_ptr + dst_offset);
            dst_offset += num;
        };

        concat(input1);
        concat(input2);
        concat(input3);
    }

    CompareCommonAbsolute(output, _refBlob, 0);

    std::map<std::string, InferenceEngineProfileInfo> perfMap;
    ASSERT_NO_THROW(st = _inferRequest->GetPerformanceCounts(perfMap, &_resp));
    ASSERT_EQ(StatusCode::OK, st) << _resp.msg;

    auto layerInfo = perfMap["reshape"];
    EXPECT_EQ(InferenceEngineProfileInfo::NOT_RUN, layerInfo.status);
}

TEST_F(myriadLayerReshapeFasterRCNN_nightly, Reshape) {
    InferenceEngine::SizeVector input_tensor = {1, 14, 14, 24};
    InferenceEngine::SizeVector output_tensor = {1, 2352, 2};
    std::map<std::string, std::string> layer_params = {
              {"axis", "0"}
            ,{"dim", "0,-1,2"}
            ,{"num_axes", std::to_string(-1)}
    };
    _testNet.addLayer(LayerInitParams("Reshape")
             .params(layer_params)
             .in({input_tensor})
             .out({output_tensor}),
            ref_reshape_wrap);
    ASSERT_TRUE(generateNetAndInfer(NetworkInitParams().useHWOpt( CheckMyriadX())));
}

INSTANTIATE_TEST_CASE_P(accuracy, myriadLayerReshape_nightly,
    ::testing::Combine(
        ::testing::ValuesIn(s_reshapeInParams),
        ::testing::ValuesIn(s_reshapeOutParams))
);

INSTANTIATE_TEST_CASE_P(fc_to_conv_case, myriadLayerReshape_nightly,
    ::testing::Values(
        std::make_tuple(
            SizeVector{400, 12544},
            SizeVector{8, 50, 256, 7, 7}
        ),
        std::make_tuple(
            SizeVector{256, 8, 7, 50, 7},
            SizeVector{1, 256, 56, 350}
        ),
        std::make_tuple(
            SizeVector{1, 1024, 8, 50},
            SizeVector{1024, 8, 1, 50, 1}
        ),
        std::make_tuple(
            SizeVector{8, 50, 1024, 1, 1},
            SizeVector{400, 1024}
        )
    )
);

INSTANTIATE_TEST_CASE_P(accuracy, myriadLayersTestsReshapeBeforeFC_nightly,
        ::testing::Values(CONFIG_VALUE(YES), CONFIG_VALUE(NO))
);

INSTANTIATE_TEST_CASE_P(accuracy, myriadLayersTestsReshapeFasterRCNN_nightly,
        ::testing::Combine(
            ::testing::ValuesIn(s_convTensor)
          , ::testing::Values<param_size>(MAKE_STRUCT(param_size, 1, 1))
          , ::testing::Values<param_size>(MAKE_STRUCT(param_size, 1, 1))
          , ::testing::Values<param_size>(MAKE_STRUCT(param_size, 0, 0))
          , ::testing::Values<uint32_t>(24)
          , ::testing::Values<uint32_t>(1)
          )
);