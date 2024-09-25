#include "AssetDumperDDLRoot.h"

#include "Game/T6/DDL/JsonDDLRootWriter.h"

#include <format>

using namespace T6;

bool AssetDumperDDLRoot::ShouldDump(XAssetInfo<ddlRoot_t>* asset)
{
    return true;
}

void AssetDumperDDLRoot::DumpAsset(AssetDumpingContext& context, XAssetInfo<ddlRoot_t>* asset)
{
    const auto fileName = std::format("{}root.json", asset->m_name);
    const auto primaryAssetFile = context.OpenAssetFile(fileName);

    if (!primaryAssetFile)
        return;

    DumpDDLRootAsJson(*primaryAssetFile, context, asset->Asset());
}
