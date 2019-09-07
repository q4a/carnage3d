#pragma once

#include "GameDefs.h"

class GameMapHelpers final
{
public:
    // construct mesh for specified city area and layer
    // @param cityScape: City scape data
    // @param area: Target map rect
    // @param layerIndex: Target map layer, see MAP_LAYERS_COUNT
    // @param meshData: Output mesh data
    static bool BuildMapMesh(GameMapManager& city, const Rect2D& area, int layerIndex, MapMeshData& meshData);
    static bool BuildMapMesh(GameMapManager& city, const Rect2D& area, MapMeshData& meshData);

    // compute height for specific block slope type
    // @param slope: Index
    // @param posx, posy: Position within block in range [0, MAP_BLOCK_LENGTH]
    static float GetSlopeHeight(int slope, float posx, float posy);

private:
    GameMapHelpers();
    // internals
    static void PutBlockFace(GameMapManager& city, MapMeshData& meshData, const Point3D& positin, eBlockFace face, BlockStyleData* blockInfo);
};