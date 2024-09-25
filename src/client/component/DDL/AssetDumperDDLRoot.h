#pragma once

#include "Dumping/AbstractAssetDumper.h"
#include "Game/T6/T6.h"

namespace T6
{
    class AssetDumperDDLRoot final : public AbstractAssetDumper<ddlRoot_t>
    {
    protected:
        bool ShouldDump(XAssetInfo<ddlRoot_t>* asset) override;
        void DumpAsset(AssetDumpingContext& context, XAssetInfo<ddlRoot_t>* asset) override;
    };
} // namespace T6
