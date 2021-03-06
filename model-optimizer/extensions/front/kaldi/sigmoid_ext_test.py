"""
 Copyright (C) 2018-2020 Intel Corporation

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
"""

from extensions.front.kaldi.sigmoid_ext import SigmoidFrontExtractor
from mo.front.kaldi.extractors.common_ext_test import KaldiFrontExtractorTest
from extensions.ops.activation_ops import Sigmoid
from mo.ops.op import Op


class SigmoidFrontExtractorTest(KaldiFrontExtractorTest):
    @classmethod
    def register_op(cls):
        Op.registered_ops['Sigmoid'] = Sigmoid

    def test_assertion(self):
        self.assertRaises(AttributeError, SigmoidFrontExtractor.extract, None)

    def test_extracted_blobs_add_shift(self):
        SigmoidFrontExtractor.extract(self.test_node)
        self.assertTrue(self.test_node.op, 'Sigmoid')
