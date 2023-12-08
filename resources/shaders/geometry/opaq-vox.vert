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

#include "common/chunk.gsl"

in uint2 vs.data : u32;

out float2 fs.texCoords;
out float3 fs.normal;

uniform pushConstants
{
	uint32 instanceIndex;
} pc;

buffer readonly Instance
{
	InstanceData data[];
} instance;

void main()
{
	float4 position = float4(decodeChunkPosition(vs.data), 1.0f);
	gl.position = instance.data[pc.instanceIndex].mvp * position;
	fs.texCoords = decodeChunkTexCoords(vs.data);
	fs.normal = decodeChunkNormal(vs.data);
}

// TODO: create separate shader with normals update for mooving objects.