#pragma once

#include "CoreMinimal.h"
#include "RobotKeywords.h"
#include "RpcValue.h"
#include "UObject/Object.h"
#include <variant>
#include "Keyword.generated.h"


class FXmlNode;
struct FRpcValue;

using FKeywordResponse = std::variant<TSharedPtr<FRpcValue>, FString>;


/// Contains name, type and default value for a Keyword argument
USTRUCT()
struct ROBOTKEYWORDS_API FKeywordArgument {
    GENERATED_BODY()

    UPROPERTY()
    FString Name;

    UPROPERTY()
    FString Type;

    UPROPERTY()
    FString DefaultValue = "";
};


/**
 * Information about a Keyword. Includes:
 *   - The name of the keyword
 *   - A list of Argument accepted by the keyword
 *   - A Docstring what this keyword does
 *   - Any Tags this Keyword might have
 *
 * See the UKeyword documentation.
 */
USTRUCT()
struct ROBOTKEYWORDS_API FKeywordInformation {
    GENERATED_BODY()

    UPROPERTY()
    FString Name;

    UPROPERTY()
    TArray<FKeywordArgument> Arguments;

    UPROPERTY()
    FString Documentation = "";

    UPROPERTY()
    TArray<FString> Tags = {};
};


/**
 * A Keyword is an action that can be executed by testcases.
 *
 * Keywords are discovered and registered after the UnrealLibrary has started an application using the
 * RemoteLibrary via the "get_library_information" RPC by the UKeywordManager.
 * During this process, we iterate all non-abstract subclasses of UKeyword and use UKeyword::GetKeywordInformation
 * to collect information about all Keywords.
 *
 * This includes:
 *   - The name of the Keyword (taken from the DisplayName field)
 *   - A description (taken from the Docstring of the Keyword)
 *   - Keyword tags (can be provided using the "KeywordTags" key in the class metadata, separated by commas)
 *   - Arguments (see below)
 *
 * Any UPROPERTY marked with the metadata "KeywordArgument" will be registered as an argument of the keyword.
 * The type of this argument will be deduced from the type of the property:
 *   - Scalar Arguments: int32 -> int, double -> double, bool -> bool, FString -> str
 *   - A FDateTime struct -> datetime.iso8601
 *   - Containers: TArray<T> -> list, TMap -> dict
 *   - Special case: TArray<uint8> -> bytes
 *   - Using any other type as a KeywordArgument is not supported (yet?) and will produce an error!
 *
 * When a Keyword is executed inside a testcase, the XML-RPC Server receives the "run_keyword" RPC, from which
 * the name of the Keyword and any potential arguments are parsed. The UKeywordManager will then run UKeyword::Execute,
 * passing in the Keyword subclass and the list of arguments. This will instantiate the keyword, fill the UProperties
 * with the provided values and lastly execute UKeyword::Execute().
 *
 * This means: In the vast majority of cases you don't need to parse any arguments yourself, provided the type of the
 * argument is provided by default!
 *
 * To implement your own keyword:
 *   - 1: Create a new UCLASS inheriting from UKeyword
 *   - 2: Add any Arguments needed (see supported types above), adding "meta=(KeywordArgument)" to the UPROPERTY()
 *   - 3: Implement the virtual Execute() and run the code you need.
 *
 * UKeyword::Executes returns a FKeywordResponse, where you can return a value from the Keyword or provide
 * an Error message: @code return Success(value)@endcode or @code return Error("Error Message")@endcode
 *
 * Further, you can also call LogInfo, LogWarning, LogDebug and LogErrorAndReturn to append a message
 * to the Robot Framework output: @code LogWarning(TEXT("Unknown Value: %s"), SomeInteger)@endcode.
 */
UCLASS(Abstract)
class ROBOTKEYWORDS_API UKeyword : public UObject {
    GENERATED_BODY()

    // Used to generate the output logs.
    FStringBuilderBase OutputBuilder;

public:
    /**
     * Executes the actions for this Keyword. Must be implemented by all non-abstract subclasses.
     *
     * @return The result of the execution. You can return Success() or Success(value) on a successful execution
     *   or Error("Message") when an error has occured.
     */
    virtual FKeywordResponse Execute() {
        unimplemented();
        return Success();
    }

    /**
     * Returns all required information for the given UKeyword subclass to be used in the "get_library_information" RPC.
     *
     * Uses the aforementioned metadata tags (see UKeyword documentation) to generate information about the Keyword
     *
     * @param KeywordClass  The subclass of the UKeyword
     * @return Keyword information for the given Keyword class.
     */
    static FKeywordInformation GetKeywordInformation(TSubclassOf<UKeyword> KeywordClass);

    /**
     * Takes the Keyword subclass and all provided arguments received in a "run_keyword" RPC and executed them.
     *
     * This will instantiate the Keyword, fill all UProperties of the instance with the provided arguments,
     * call UKeyword::Execute and finally generate an XML-RPC response that can be returned from the RPC.
     *
     * @param KeywordClass The subclass of the UKeyword
     * @param Arguments Arguments passed to the execution of the Keyword
     * @return XML-RPC response for the execution of the keyword
     */
    static TSharedPtr<FRpcValue> Run(
        TSubclassOf<UKeyword> KeywordClass, const TArray<TSharedPtr<FRpcValue>>& Arguments
    );

protected:
    /**
     * Log a message that will be reported back to Robot Framework.
     *
     * @param Level Log level of the message. Can be FAIL, ERROR, WARN, INFO, DEBUG or TRACE. The
     *   Pseudo-levels HTML and CONSOLE are currently untested, but might still work.
     * @param Fmt Format String. Can contain printf specifiers
     * @param Args Values to fill in the printf specifiers in the format string.
     */
    template<typename FmtType, typename... Types>
    void Log(const FString& Level, const FmtType& Fmt, Types... Args) {
        const FString Message = FString::Printf(Fmt, Forward<Types>(Args)...);
        
        UE_LOG(LogRobotKeywords, Log, TEXT("Keyword Log: *%s* %s"), *Level, *Message);
        OutputBuilder.Appendf(TEXT("*%s* %s\n"), *Level, *Message);
    }

    /// Log a warning message. @see UKeyword::Log
    template<typename FmtType, typename... Types>
    void LogWarning(const FmtType& Fmt, Types... Args) {
        Log("WARN", Fmt, Forward<Types>(Args)...);
    }

    /// Log an info message. @see UKeyword::Log
    template<typename FmtType, typename... Types>
    void LogInfo(const FmtType& Fmt, Types... Args) {
        Log("INFO", Fmt, Forward<Types>(Args)...);
    }

    /// Log a debug message. @see UKeyword::Log
    template<typename FmtType, typename... Types>
    void LogDebug(const FmtType& Fmt, Types... Args) {
        Log("DEBUG", Fmt, Forward<Types>(Args)...);
    }

    /// Successful execution without a return value
    FKeywordResponse Success();

    /// Successful execution returning an integer
    FKeywordResponse Success(const int32& Value);

    /// Successful execution returning a boolean
    FKeywordResponse Success(const bool& Value);

    /// Successful execution returning a string
    FKeywordResponse Success(const FString& Value);

    /// Successful execution returning a double
    FKeywordResponse Success(const double& Value);

    /// Successful execution returning a DateTime
    FKeywordResponse Success(const FDateTime& Value);

    /// Successful execution returning some bytes
    FKeywordResponse Success(const TArray<uint8>& Value);

    /// Successful execution returning an array of values
    FKeywordResponse Success(const TArray<TSharedPtr<FRpcValue>>& Value);

    /// Successful execution returning a dictionary
    FKeywordResponse Success(const TMap<FString, TSharedPtr<FRpcValue>>& Value);

    /// Erroneous execution with an error message
    FKeywordResponse Error(const FString& Message);

    /// Log an error and return an erroneous FKeywordResponse with the same error message
    template<typename FmtType, typename... Types>
    FKeywordResponse LogErrorAndReturn(const FmtType& Fmt, Types... Args) {
        Log("FAIL", Fmt, Forward<Types>(Args)...);
        return Error(FString::Printf(Fmt, Forward<Types>(Args)...));
    }

private:
    /**
     * Receives the response from the execution and the OutputBuilder containing any logs. Builds an XML that will
     * be returned to the Robot Framework, following the expected format for the result of a remote Keyword
     *
     * @see https://robotframework.org/robotframework/latest/RobotFrameworkUserGuide.html#executing-remote-keywords
     *
     * @param Response The Keyword Response returned by UKeyword::Execute
     * @param OutputBuilder The OutputBuilder containing the Logs
     * @return An XML-RPC Struct containing the result of the execution
     */
    static TSharedPtr<FRpcValue> GenerateResponse(
        const FKeywordResponse& Response, const FStringBuilderBase& OutputBuilder
    );
};
