#pragma once

#include <variant>

/**
 * Schema Definition: https://xmlrpc.com/spec.md
 *
 * Represents any valid value an RPC parameter / return type can have.
 * Unfortunately we can't use TVariant, as it is final, and we need to inherit because we need recursive types in it
 */

struct XMLRPC_API FRpcValue :
    std::variant<
        int32, bool, FString, double,        /* Simple scalar types */
        FDateTime, TArray<uint8>,            /* DateTime and Base64 binary data */
        TArray<TSharedPtr<FRpcValue>>,       /* XML-RPC Arrays */
        TMap<FString, TSharedPtr<FRpcValue>> /* XML-RPC Structs */
        > {
    using FBase = std::variant<
        int32, bool, FString, double, FDateTime, TArray<uint8>, TArray<TSharedPtr<FRpcValue>>,
        TMap<FString, TSharedPtr<FRpcValue>>>;
    using FBase::FBase;
    using FBase::operator=;
    using FBase::index;
};

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
