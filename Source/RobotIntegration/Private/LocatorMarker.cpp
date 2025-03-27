#include "LocatorMarker.h"
#include "Framework/MetaData/DriverMetaData.h"


TSharedRef<SWidget> ULocatorMarker::RebuildWidget() {
    if (GetChildrenCount() > 0) {
        TSharedRef<SWidget> Content = GetContentSlot()->Content->TakeWidget();
        Content->AddMetadata(FDriverMetaData::Id(Id));
        Content->SetTag(Tag);
        return Content;
    }

    return SNew(SSpacer);
}