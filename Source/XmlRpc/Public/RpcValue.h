#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RpcValue.generated.h"


class FXmlNode;
struct FRpcValue;
using FRpcValueList = TArray<TSharedPtr<FRpcValue>>;
using FRpcValueStruct = TMap<FString, TSharedPtr<FRpcValue>>;


UENUM()
enum class ERpcValueType : uint8 { Integer, Double, Boolean, String, DateTime, Binary, List, Struct, Nil };


#define RPC_VALUE_DEFINE_TYPE(CppType, EnumType)                                 \
    explicit FRpcValue(CppType Value) : Value(TInPlaceType<CppType>{}, Value) {} \
    bool Is##EnumType() const {                                                  \
        return Type() == ERpcValueType::EnumType;                                \
    }                                                                            \
    CppType Get##EnumType() const {                                              \
        return Value.Get<CppType>();                                             \
    }


/**
 * A struct containing the value of an RPC argument using a Variant.
 *
 * Values can be received by their respective getters. You must ensure the value is actually of the
 * desired type before getting the value!
 */
USTRUCT()
struct XMLRPC_API FRpcValue {
    GENERATED_BODY()

    friend struct FRpcMethodResponse;

private:
    using FValueType = TVariant<
        FEmptyVariantState, int32, double, bool, FString, FDateTime, TArray<uint8>, FRpcValueList, FRpcValueStruct>;

    FValueType Value;

public:
    FRpcValue() = default;

    RPC_VALUE_DEFINE_TYPE(int32, Integer)
    RPC_VALUE_DEFINE_TYPE(double, Double)
    RPC_VALUE_DEFINE_TYPE(bool, Boolean)
    RPC_VALUE_DEFINE_TYPE(FString, String)
    RPC_VALUE_DEFINE_TYPE(FDateTime, DateTime)
    RPC_VALUE_DEFINE_TYPE(TArray<uint8>, Binary)
    RPC_VALUE_DEFINE_TYPE(FRpcValueList, List)
    RPC_VALUE_DEFINE_TYPE(FRpcValueStruct, Struct)

    ERpcValueType Type() const;

    operator bool() const {
        return Type() != ERpcValueType::Nil;
    }

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

    /// Parses the value into an XML Node
    FXmlNode* ParseToXml() const;

    /// Parses the value into an XML string
    FString ParseToXmlString() const;

    /**
     * Tries to fill the property of an object with the value.
     *
     * @param Property The property to fill
     * @param PropertyValue The object that has the property
     * @return Whether the value could be parsed into the object
     */
    bool ParseIntoProperty(FProperty* Property, void* PropertyValue) const;

    /// Parses an XML Node into a PRC Value. Returns nullptr when invalid/unsupported XML
    static TSharedPtr<FRpcValue> FromXml(const FXmlNode* ValueNode);

private:
    void ParseIntoStringBuilder(FStringBuilderBase& Builder) const;
};


#undef RPC_VALUE_DEFINE_TYPE


/**
 * Response from a remote procedure.
 *
 * Either an RPC Value to be returned when the remote procedure was successful or a tuple
 * of error code and message when it was erroneous.
 */
USTRUCT()
struct XMLRPC_API FRpcMethodResponse {
    GENERATED_BODY()

    using FSuccess = TSharedPtr<FRpcValue>;
    using FFault = TTuple<int32, FString>;

private:
    TVariant<FEmptyVariantState, FSuccess, FFault> Value;

public:
    FRpcMethodResponse() = default;

    explicit FRpcMethodResponse(FSuccess Value) : Value(TInPlaceType<FSuccess>{}, Value) {}

    explicit FRpcMethodResponse(FFault Value) : Value(TInPlaceType<FFault>{}, Value) {}

    explicit FRpcMethodResponse(int32 FaultCode, const FString& FaultMessage) :
        Value(TInPlaceType<FFault>{}, MakeTuple(FaultCode, FaultMessage)) {}

    bool IsSuccess() const {
        return Value.IsType<FSuccess>();
    }

    operator bool() const {
        return IsSuccess();
    }

    FSuccess GetSuccess() const {
        return Value.Get<FSuccess>();
    }

    FFault GetFault() const {
        return Value.Get<FFault>();
    }

    FXmlNode* ParseToXml() const;

    FString ParseToXmlString() const;
};
