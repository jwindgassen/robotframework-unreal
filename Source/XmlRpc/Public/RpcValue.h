#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include <variant>
#include "RpcValue.generated.h"


class FXmlNode;
struct FRpcValue;
using FRpcValueList = TArray<TSharedPtr<FRpcValue>>;
using FRpcValueStruct = TMap<FString, TSharedPtr<FRpcValue>>;


UENUM()
enum class ERpcValueType : uint8 { Integer, Double, Boolean, String, DateTime, Binary, List, Struct, Null };


#define RPC_VALUE_DEFINE_TYPE(CppType, EnumType)                                 \
    explicit FRpcValue(CppType Value) : Value(TInPlaceType<CppType>{}, Value) {} \
    bool Is##EnumType() const {                                                  \
        return Type() == ERpcValueType::EnumType;                                \
    }                                                                            \
    CppType Get##EnumType() const {                                              \
        return Value.Get<CppType>();                                             \
    }


USTRUCT()
struct XMLRPC_API FRpcValue {
    GENERATED_BODY()

private:
    using FValueType = TVariant<
        int32, double, bool, FString, FDateTime, TArray<uint8>, FRpcValueList, FRpcValueStruct, FEmptyVariantState>;

    FValueType Value;

public:
    // Default Constructor for an empty Variant
    FRpcValue() : Value(TInPlaceType<FEmptyVariantState>{}) {}

    ERpcValueType Type() const;

    operator bool() const {
        return Type() != ERpcValueType::Null;
    }

    RPC_VALUE_DEFINE_TYPE(int32, Integer)
    RPC_VALUE_DEFINE_TYPE(double, Double)
    RPC_VALUE_DEFINE_TYPE(bool, Boolean)
    RPC_VALUE_DEFINE_TYPE(FString, String)
    RPC_VALUE_DEFINE_TYPE(FDateTime, DateTime)
    RPC_VALUE_DEFINE_TYPE(TArray<uint8>, Binary)
    RPC_VALUE_DEFINE_TYPE(FRpcValueList, List)
    RPC_VALUE_DEFINE_TYPE(FRpcValueStruct, Struct)

    template<typename T>
    T& Get() {
        return Value.Get<T>();
    }

    template<typename T>
    const T& Get() const {
        return Value.Get<T>();
    }

    template<typename T>
    T* TryGet() {
        return Value.TryGet<T>();
    }

    template<typename T>
    const T* TryGet() const {
        return Value.TryGet<T>();
    }

    FXmlNode* ParseToXml() const;
    
    FString ParseToXmlString() const;

    bool ParseIntoProperty(FProperty* Property, void* PropertyValue) const;

private:
    void ParseIntoStringBuilder(FStringBuilderBase& Builder) const;
    
};

#undef RPC_VALUE_DEFINE_TYPE


struct XMLRPC_API FRpcMethodResponse :
    std::variant<
        TSharedPtr<FRpcValue>, /* Success */
        TTuple<int32, FString> /* Fault */
        > {
    using FBase = std::variant<TSharedPtr<FRpcValue>, TTuple<int32, FString>>;
    using FBase::FBase;
    using FBase::operator=;
    using FBase::index;
};
