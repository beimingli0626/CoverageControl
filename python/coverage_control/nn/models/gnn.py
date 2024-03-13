#  This file is part of the CoverageControl library
#
#  Author: Saurav Agarwal
#  Contact: sauravag@seas.upenn.edu, agr.saurav1@gmail.com
#  Repository: https://github.com/KumarRobotics/CoverageControl
#
#  Copyright (c) 2024, Saurav Agarwal
#
#  The CoverageControl library is free software: you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or (at your
#  option) any later version.
#
#  The CoverageControl library is distributed in the hope that it will be
#  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
#  Public License for more details.
#
#  You should have received a copy of the GNU General Public License along with
#  CoverageControl library. If not, see <https://www.gnu.org/licenses/>.

import torch
import torch_geometric
from torch_geometric.nn import MLP

from .config_parser import GNNConfigParser
from .cnn_backbone import CNNBackBone
from .gnn_backbone import GNNBackBone

__all__ = ["LPAC"]

class LPAC(torch.nn.Module, GNNConfigParser):
    def __init__(self, config):
        super(LPAC, self).__init__()
        self.cnn_config = config['CNN']
        self.parse(config['GNN'])
        self.cnn_backbone = CNNBackBone(self.cnn_config)
        self.gnn_backbone = GNNBackBone(self.config, self.cnn_backbone.latent_size + 2)
        # --- no pos ---
        # self.gnn_backbone = GNNBackBone(self.config, self.cnn_backbone.latent_size)
        # --- no pos ---
        self.gnn_mlp = MLP([self.latent_size, 32, 32])
        self.output_linear = torch.nn.Linear(32, self.output_dim)
        # Register buffers to model
        self.register_buffer("actions_mean", torch.zeros(self.output_dim))
        self.register_buffer("actions_std", torch.ones(self.output_dim))

    def forward(self, data: torch_geometric.data.Data) -> torch.Tensor:
        x, edge_index, edge_weight = data.x, data.edge_index, data.edge_weight
        pos = data.pos
        cnn_output = self.cnn_backbone(x.view(-1, x.shape[-3], x.shape[-2], x.shape[-1]))

        # --- no pos ---
        # gnn_output = self.gnn_backbone(cnn_output, edge_index)
        # mlp_output = self.gnn_mlp(gnn_output)
        # x = self.output_linear(mlp_output)
        # x = self.output_linear(self.gnn_mlp(self.gnn_backbone(cnn_output, edge_index)))
        # --- no pos ---

        gnn_backbone_in = torch.cat([cnn_output, pos], dim=-1)
        # print(gnn_backbone_in)
        # gnn_output = self.gnn_backbone(gnn_backbone_in, edge_index)
        # mid_test = self.gnn_mlp.lins[0](gnn_output)
        # print(f'mid_test sum1: {mid_test.sum()}')
        # mid_test = self.gnn_mlp.norms[0](mid_test)
        # print(f'mid_test sum: {mid_test.sum()}')
        # mlp_output = self.gnn_mlp(self.gnn_backbone(gnn_backbone_in, edge_index)
        # print(f'mlp_output sum: {mlp_output[0]}')
        x = self.output_linear(self.gnn_mlp(self.gnn_backbone(gnn_backbone_in, edge_index)))
        return x

    def load_model(self, model_state_dict_path: str) -> None:
        self.load_state_dict(torch.load(model_state_dict_path), strict=False)

    def load_cnn_backbone(self, model_path: str) -> None:
        self.load_state_dict(torch.load(model_path).state_dict(), strict=False)

    def load_gnn_backbone(self, model_path: str) -> None:
        self.load_state_dict(torch.load(model_path).state_dict(), strict=False)
