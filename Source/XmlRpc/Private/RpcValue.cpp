#include "RpcValue.h"

#include "XmlFile.h"
#include "XmlRpcServer.h"

ERpcValueType FRpcValue::Type() const {
    switch (Value.GetIndex()) {
        case 0: return ERpcValueType::Nil;
        case 1: return ERpcValueType::Integer;
        case 2: return ERpcValueType::Double;
        case 3: return ERpcValueType::Boolean;
        case 4: return ERpcValueType::String;
        case 5: return ERpcValueType::DateTime;
        case 6: return ERpcValueType::Binary;
        case 7: return ERpcValueType::List;
        case 8: return ERpcValueType::Struct;
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
        case 0: Builder.Append("<nil/>"); break;
        case 1: Builder.Appendf(TEXT("<i4>%d</i4>"), GetInteger()); break;
        case 2: Builder.Appendf(TEXT("<double>%f</double>"), GetDouble()); break;
        case 3: Builder.Appendf(TEXT("<bool>%s</bool>"), GetBoolean() ? TEXT("1") : TEXT("0")); break;
        case 4: Builder.Appendf(TEXT("<string>%s</string>"), *GetString()); break;
        case 5: {
            Builder.Appendf(TEXT("<dateTime.iso8601>%s</dateTime.iso8601>"), *GetDateTime().ToIso8601());
            break;
        }
        case 6: {
            Builder.Appendf(TEXT("<base64>%s<base64>"), *FBase64::Encode(GetBinary()));
            break;
        }
        case 7: {
            Builder.Append("<array><data>");
            for (const auto& Val : GetList()) {
                Val->ParseIntoStringBuilder(Builder);
            }
            Builder.Append("</data></array>");

            break;
        }
        case 8: {
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

static TMap<FString, FString> XmlEscapes = {
    {"&amp;", "&"},
    {"&quot;", "\""},
    {"&apos;", "'"},
    {"&lt;", "<"},
    {"&gt;", ">"},
};

FString UnescapeXml(FString Escaped) {
    for (const auto& [Find, Replace] : XmlEscapes) {
        Escaped = Escaped.Replace(*Find, *Replace);
    }
    return Escaped;
}

TSharedPtr<FRpcValue> FRpcValue::FromXml(const FXmlNode* ValueNode) {
    // If no type is defined, this is a String
    if (!ValueNode->GetFirstChildNode()) {
        FString Content = UnescapeXml(ValueNode->GetContent());
        return MakeShared<FRpcValue>(Content);
    }

    const FString& Type = ValueNode->GetFirstChildNode()->GetTag();
    const FString& Content = ValueNode->GetFirstChildNode()->GetContent();

    if (Type == "nil") {
        return MakeShared<FRpcValue>();
    }
    if (Type == "i4") {
        return MakeShared<FRpcValue>(FPlatformString::Atoi(*Content));
    }
    if (Type == "bool") {
        return MakeShared<FRpcValue>(Content == "1");
    }
    if (Type == "string") {
        const FString& Unescaped = UnescapeXml(Content);
        return MakeShared<FRpcValue>(Unescaped);
    }
    if (Type == "double") {
        return MakeShared<FRpcValue>(FPlatformString::Atod(*Content));
    }
    if (Type == "dateTime.iso8601") {
        FDateTime DateTime;
        if (!FDateTime::ParseIso8601(*Content, DateTime)) {
            UE_LOG(LogXmlRpcServer, Error, TEXT("Could not parse ISO-8601 timestamp"))
        }
        return MakeShared<FRpcValue>(DateTime);
    }
    if (Type == "base64") {
        TArray<uint8> Base64;
        if (!FBase64::Decode(Content, Base64)) {
            UE_LOG(LogXmlRpcServer, Error, TEXT("Could not decode Base64 String"))
        }
        return MakeShared<FRpcValue>(Base64);
    }
    if (Type == "array") {
        TArray<TSharedPtr<FRpcValue>> Array;
        for (const FXmlNode* Node : ValueNode->GetFirstChildNode()->GetFirstChildNode()->GetChildrenNodes()) {
            Array.Push(FromXml(Node));
        }
        return MakeShared<FRpcValue>(Array);
    }
    if (Type == "struct") {
        TMap<FString, TSharedPtr<FRpcValue>> Struct;
        for (const FXmlNode* MemberNode : ValueNode->GetFirstChildNode()->GetChildrenNodes()) {
            Struct.Emplace(
                MemberNode->FindChildNode("name")->GetContent(), FromXml(MemberNode->FindChildNode("value"))
            );
        }
        return MakeShared<FRpcValue>(Struct);
    }

    UE_LOG(LogXmlRpcServer, Error, TEXT("Unknown Value Type: '%s'"), *Type)
    return nullptr;
}


FXmlNode* FRpcMethodResponse::ParseToXml() const {
    const FString XmlString = ParseToXmlString();

    FXmlFile Parser{};
    Parser.LoadFile(XmlString, EConstructMethod::ConstructFromBuffer);

    if (!Parser.IsValid()) {
        UE_LOG(LogXmlRpcServer, Error, TEXT("Could not Parse FRpcValue into XML: %s"), *Parser.GetLastError())
        return nullptr;
    }

    return Parser.GetRootNode();
}

FString FRpcMethodResponse::ParseToXmlString() const {
    FStringBuilderBase XmlString;
    XmlString.Append("<methodResponse>");

    if (IsSuccess()) {
        XmlString.Append("<params><param>");
        GetSuccess()->ParseIntoStringBuilder(XmlString);
        XmlString.Append("</param></params>");
    } else {
        XmlString.Append("<fault><value><struct>");
        XmlString.Appendf(
            TEXT("<member>"
                 "<name>faultCode</name>"
                 "<value><i4>%d</i4></value>"
                 "</member>"),
            GetFault().Key
        );
        XmlString.Appendf(
            TEXT("<member>"
                 "<name>faultString</name>"
                 "<value><string>%s</string></value>"
                 "</member>"),
            *GetFault().Value
        );
        XmlString.Append("</struct></value></fault>");
    }

    XmlString.Append("</methodResponse>");
    return XmlString.ToString();
}
