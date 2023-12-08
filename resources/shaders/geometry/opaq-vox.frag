//--------------------------------------------------------------------------------------------------
// Voxfield - An open source voxel based multiplayer sandbox game.
// Copyright (C) 2022-2023  Nikita Fediuchin
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//--------------------------------------------------------------------------------------------------

#include "common/gbuffer.gsl"

pipelineState
{
	depthTesting = on;
	depthWriting = on;
}

in float2 fs.texCoords;
in float3 fs.normal;

out float4 fb.gBuffer0;
out float4 fb.gBuffer1;
out float4 fb.gBuffer2;

void main()
{
	float3 color = float3(1.0f);
	float metallic = 0.0f;
	float roughness = 1.0f;
	float reflectance = 0.5f;
	float3 emissive = float3(0.0f);

	fb.gBuffer0 = encodeGBuffer0(color, metallic); 
	fb.gBuffer1 = encodeGBuffer1(fs.normal, reflectance);
	fb.gBuffer2 = encodeGBuffer2(emissive, roughness);
}