#include "RpcValue.h"

#include "XmlFile.h"
#include "XmlRpcServer.h"

ERpcValueType FRpcValue::Type() const {
    switch (Value.GetIndex()) {
        case 0: return ERpcValueType::Integer;
        case 1: return ERpcValueType::Double;
        case 2: return ERpcValueType::Boolean;
        case 3: return ERpcValueType::String;
        case 4: return ERpcValueType::DateTime;
        case 5: return ERpcValueType::Binary;
        case 6: return ERpcValueType::List;
        case 7: return ERpcValueType::Struct;
        case 8: return ERpcValueType::Null;
        default: {
            _STL_UNREACHABLE;
        }
    }
}

FXmlNode* FRpcValue::ParseToXml() const {
    const FString XmlString = ParseToXmlString();
    
    FXmlFile Parser{};
    Parser.LoadFile(XmlString, EConstructMethod::ConstructFromBuffer);

    if (!Parser.IsValid()) {
        UE_LOG(LogXmlRpcServer, Error, TEXT("Could not Parse FRpcValue into XML: %s"), *Parser.GetLastError())
        return nullptr;
    }
    
    return Parser.GetRootNode();
}

FString FRpcValue::ParseToXmlString() const {
    FStringBuilderBase Builder;
    ParseIntoStringBuilder(Builder);
    return Builder.ToString();
}

void FRpcValue::ParseIntoStringBuilder(FStringBuilderBase& Builder) const {
    Builder.Append("<value>");

    switch (Value.GetIndex()) {
        case 0: Builder.Appendf(TEXT("<i4>%d</i4>"), GetInteger()); break;
        case 1: Builder.Appendf(TEXT("<double>%f</double>"), GetDouble()); break;
        case 2: Builder.Appendf(TEXT("<bool>%s</bool>"), GetBoolean() ? TEXT("1") : TEXT("0")); break;
        case 3: Builder.Appendf(TEXT("<string>%s</string>"), *GetString()); break;
        case 4: {
            Builder.Appendf(TEXT("<dateTime.iso8601>%s</dateTime.iso8601>"), *GetDateTime().ToIso8601());
            break;
        }
        case 5: {
            Builder.Appendf(TEXT("<base64>%s<base64>"), *FBase64::Encode(GetBinary()));
            break;
        }
        case 6: {
            Builder.Append("<array><data>");
            for (const auto& Val : GetList()) {
                Val->ParseIntoStringBuilder(Builder);
            }
            Builder.Append("</data></array>");

            break;
        }
        case 7: {
            Builder.Append("<struct>");
            for (const auto& [Name, Val] : GetStruct()) {
                Builder.Appendf(TEXT("<member><name>%s</name>"), *Name);
                Val->ParseIntoStringBuilder(Builder);
                Builder.Append("</member>");
            }
            Builder.Append("</struct>");

            break;
        }
        default: {
            _STL_UNREACHABLE;
        }
    }

    Builder.Append("</value>");
}

bool FRpcValue::ParseIntoProperty(FProperty* Property, void* PropertyValue) const {
    if (const auto* NumericProperty = CastField<FNumericProperty>(Property)) {
        if (NumericProperty->IsInteger()) {
            check(IsInteger())

            NumericProperty->SetIntPropertyValue(PropertyValue, static_cast<int64>(GetInteger()));
            return true;
        }

        if (NumericProperty->IsFloatingPoint()) {
            check(IsDouble())

            NumericProperty->SetFloatingPointPropertyValue(PropertyValue, GetDouble());
            return true;
        }

        return false;
    }

    if (const auto* BoolProperty = CastField<FBoolProperty>(Property)) {
        check(IsBoolean())

        BoolProperty->SetPropertyValue(PropertyValue, GetBoolean());
        return true;
    }

    if (const auto* StringProperty = CastField<FStrProperty>(Property)) {
        check(IsString())

        StringProperty->SetPropertyValue(PropertyValue, GetString());
        return true;
    }

    if (const auto* ArrayProperty = CastField<FArrayProperty>(Property)) {
        FScriptArrayHelper Helper{ArrayProperty, PropertyValue};

        if (CastField<FByteProperty>(ArrayProperty->Inner)) {
            check(IsBinary())

            auto Array = GetBinary();
            Helper.MoveAssign(&Array);

            return true;
        }

        if (CastField<FObjectPtrProperty>(ArrayProperty->Inner)) {
            check(IsList())

            const auto& Array = GetList();
            Helper.AddValues(Array.Num());

            for (int32 i = 0; i < Array.Num(); ++i) {
                Array[i]->ParseIntoProperty(ArrayProperty->Inner, Helper.GetRawPtr(i));
            }

            return true;
        }

        return false;
    }

    if (const auto* MapProperty = CastField<FMapProperty>(Property)) {
        check(IsStruct())
        check(CastField<FStrProperty>(MapProperty->KeyProp))
        check(CastField<FObjectPtrProperty>(MapProperty->ValueProp))

        // const auto& Map = std::get<TMap<FString, TSharedPtr<FRpcValue>>>(*Element);
        // FScriptMapHelper Helper{MapProperty, PropertyValue};
        checkf(false, TEXT("ToDo"))
        return false;
    }

    return false;
}
