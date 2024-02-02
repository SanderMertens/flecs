// Copyright 2023 Guganana. All Rights Reserved.

#pragma once

#include <CoreMinimal.h>

#include "Engine/Engine.h"
#include "Misc/CoreDelegates.h"
#include "Misc/FileHelper.h"

#define UNLOG_VERSION TEXT("0.1")
#define UNLOG_ENABLED (!UE_BUILD_SHIPPING)
#define UNLOG_COMPILED_OUT  

// ------------------------------------------------------------------------------------
// Static Generation Helpers
// Templated structs used to select the appropriate template variations when 
// ------------------------------------------------------------------------------------

template< typename InFormatOptions, typename InCategoryPicker, typename InTargetOptions >
struct TStaticConfiguration
{
    using FormatOptions = InFormatOptions;
    using CategoryPicker = InCategoryPicker;
    using TargetOptions = InTargetOptions;
};

template<bool InIsPrintfFormat>
struct TFormatOptions
{
    static constexpr bool IsPrintfFormat = InIsPrintfFormat;
};

// ------------------------------------------------------------------------------------
// Logging Function Generators
// 
// Macros to generate the different variants of logging functions
// ------------------------------------------------------------------------------------

#if UNLOG_ENABLED
#define UNLOG_DECLARE_CATEGORY_LOG_FUNCTION_CONDITIONALS( CategoryPicker, TargetOptions, FunctionName, VerbosityName, IsPrintf ) \
    template<typename TCategory = CategoryPicker, typename FMT, typename... ArgTypes> \
    FORCEINLINE static void FunctionName(const FMT& Format, ArgTypes... Args)\
    {\
        using Configuration = TStaticConfiguration< TFormatOptions<IsPrintf>, CategoryPicker, TargetOptions >;\
        Unlogger::Get().UnlogPrivateImpl< Configuration >(Format, ELogVerbosity::VerbosityName, Args...);\
    }\
    template<typename TCategory = CategoryPicker, typename FMT, typename... ArgTypes>\
    FORCEINLINE static void FunctionName(const bool Condition, const FMT& Format, ArgTypes... Args)\
    {\
        if(Condition)\
        {\
            using Configuration = TStaticConfiguration< TFormatOptions<IsPrintf>, CategoryPicker, TargetOptions >;\
            Unlogger::Get().UnlogPrivateImpl< Configuration >(Format, ELogVerbosity::VerbosityName, Args...);\
        }\
    }\
    template<typename TCategory = CategoryPicker, typename FMT, typename... ArgTypes>\
    FORCEINLINE static void FunctionName(const TFunction<bool()>& LambdaCondition, const FMT& Format, ArgTypes... Args)\
    {\
        FunctionName( LambdaCondition(), Format, Args... );\
    }
#else
#define UNLOG_DECLARE_CATEGORY_LOG_FUNCTION_CONDITIONALS( CategoryPicker, TargetOptions, FunctionName, VerbosityName, IsPrintf ) \
    template< typename... TemplateArgs,typename... TArgs > \
    FORCEINLINE static void FunctionName(TArgs... Args){}
#endif // UNLOG_ENABLED

#define UNLOG_DECLARE_CATEGORY_LOG_FUNCTION( CategoryPicker, TargetOptions, FunctionName, VerbosityName )\
    UNLOG_DECLARE_CATEGORY_LOG_FUNCTION_CONDITIONALS( CategoryPicker, TargetOptions, FunctionName, VerbosityName, false )\
    UNLOG_DECLARE_CATEGORY_LOG_FUNCTION_CONDITIONALS( CategoryPicker, TargetOptions, FunctionName##f, VerbosityName, true )

// ------------------------------------------------------------------------------------
// Categories
// 
// Declaring a category generates its own class with a unique and just one statically 
// constructed instance. 
// 
// Category declarations respect the scope they are declared on and are lazyly 
// constructed when used for the first time by a log function. Instace objects will 
// live for rest of app's execution.
// ------------------------------------------------------------------------------------

class UnlogCategoryBase
{
private:
    FName CategoryName;
    ELogVerbosity::Type Verbosity;

public:

    UnlogCategoryBase(const FName& InName, ELogVerbosity::Type InVerbosity)
        : CategoryName(InName)
        , Verbosity(InVerbosity)
    {}

    const FName& GetName() const
    {
        return CategoryName;
    }

    ELogVerbosity::Type GetVerbosity() const
    {
        return Verbosity;
    }
};

template<typename TCategory>
class UnlogCategoryCRTP : public UnlogCategoryBase
{

public:
    using UnlogCategoryBase::UnlogCategoryBase;

    static TCategory& Static()
    {
        static TCategory Type = TCategory::Construct();
        return Type;
    }

    static void PickCategory(UnlogCategoryBase*& InOutCategory)
    {
        InOutCategory = &Static();
    }
};


#if UNLOG_ENABLED
/**
* Creates a new Unlog category.
* Can be used anywhere (headers, inside .cpp files, inside classes or functions)
* Generates a unique class for this category and can be shared with other files if put in a common header
*/
#define UNLOG_CATEGORY(CategoryName)  \
class CategoryName : public UnlogCategoryCRTP< CategoryName > \
{\
protected:\
    using UnlogCategoryCRTP<CategoryName>::UnlogCategoryCRTP; \
    static CategoryName Construct()\
    {\
        return CategoryName( TEXT( #CategoryName ), ELogVerbosity::Log );\
    }\
friend class UnlogCategoryCRTP< CategoryName >;\
};

#define UNLOG_CATEGORY_PUSH( CategoryName ) \
    FUnlogScopedCategory<CategoryName> ScopedCategory_##CategoryName = FUnlogScopedCategory<CategoryName>();

#define UNLOG_CATEGORY_SCOPED(CategoryName) \
    UNLOG_CATEGORY(CategoryName) \
    UNLOG_CATEGORY_PUSH(CategoryName)

#else
#define UNLOG_CATEGORY( CategoryName ) class CategoryName {};
#define UNLOG_CATEGORY_PUSH( CategoryName ) UNLOG_COMPILED_OUT
#define UNLOG_CATEGORY_SCOPED( CategoryName ) UNLOG_CATEGORY( CategoryName )
#endif

// Create the default category used by Unlog
UNLOG_CATEGORY(LogGeneral)

// ------------------------------------------------------------------------------------
// Runtime Settings and Runtime Targets (Experimental)
// ------------------------------------------------------------------------------------

class UnlogRuntimeTargetBase
{
public:
    virtual ~UnlogRuntimeTargetBase() {}
    virtual void ProcessLog(const FName& Category, ELogVerbosity::Type Verbosity, const FString& Message) = 0;
};

struct UnlogRuntimeSettingsBase
{
public:

    UnlogRuntimeSettingsBase()
        : Targets()
        , DefaultCategory()
    {
    }

    template< typename TTarget>
    TSharedRef<TTarget> AddTarget()
    {
        auto Target = TSharedRef<UnlogRuntimeTargetBase>(MakeShareable(new TTarget()));
        Targets.Add(Target);
        return StaticCastSharedRef<TTarget>(Target);
    }

    const TArray<TSharedRef<UnlogRuntimeTargetBase>>& GetTargets()
    {
        return Targets;
    }

private:
    TArray<TSharedRef<UnlogRuntimeTargetBase>> Targets;
    UnlogCategoryBase* DefaultCategory;
};

template< typename ConcreteSettings >
struct RuntimeSettingsCRTP : public UnlogRuntimeSettingsBase
{
public:
    using UnlogRuntimeSettingsBase::UnlogRuntimeSettingsBase;

    static ConcreteSettings MakeSettings()
    {
        ConcreteSettings Settings;
        Settings.PopulateSettings();
        return Settings;
    }

    static ConcreteSettings& Static()
    {
        static ConcreteSettings Type = MakeSettings();
        return Type;
    }
};

struct UnlogDefaultRuntimeSettings : public RuntimeSettingsCRTP<UnlogDefaultRuntimeSettings>
{
public:
    void PopulateSettings()
    {
    }

    using DefaultSettings = LogGeneral;
};

template< typename TUnlogSettings >
struct SetGlobalUnlogSettings
{
    const static int32 Initializer;
};

#if UNLOG_ENABLED
#define UNLOG_DEFAULT_SETTINGS( SettingsName ) \
const int32 SetGlobalUnlogSettings< SettingsName >::Initializer = [] { Unlogger::Get().ApplySettings< SettingsName >();  return 0; }();
#else
#define UNLOG_DEFAULT_SETTINGS( SettingsName ) UNLOG_COMPILED_OUT
#endif

// ------------------------------------------------------------------------------------
// Telemetry
// 
// Sending a single telemetry event on the first time the logger is used, we try to
// include as little data as possible while still giving us valuable information to
// gauge the usage and version distribution of Unlog. This helps us make better and 
// more informed decisions on how to improve.
// ------------------------------------------------------------------------------------
#if WITH_EDITOR && UNLOG_ENABLED
class FTelemetryDispatcher
{

public:
    FTelemetryDispatcher()
    {
        const auto Prod = TEXT("Unlog");
        const auto AppId = AppId::Get().ToString();
        const auto SHA = TEXT("None");
        const auto Date = FDateTime(2023, 8, 20).ToIso8601();
        const auto UEVersion = FEngineVersion::Current().ToString();

        // Running request as a console command cuts direct dependencies to the
        // http module which would make Unlog harder to include since it's a header only lib
        const FString Cmd = FString::Format(
            TEXT(
                "http TEST \"1\" "
                "\"https://api.guganana.com/api/usage?data="
                "%7B"
                "%22pluginName%22%3A%22{0}%22"
                "%2C%22appId%22%3A%22{1}%22"
                "%2C%22versionFriendly%22%3A%22{2}%22"
                "%2C%22versionSHA%22%3A%22{3}%22"
                "%2C%22versionDate%22%3A%22{4}%22"
                "%2C%22unrealVersion%22%3A%22{5}%22"
                "%7D\""
            ),
            { Prod, AppId, UNLOG_VERSION, SHA, Date, UEVersion }
        );

        const auto RunCmd = [Cmd] {
            GEngine->Exec(nullptr, *Cmd);
        };

        if (!GEngine || !GEngine->IsInitialized())
        {
            FCoreDelegates::OnPostEngineInit.AddLambda(RunCmd);
        }
        else
        {
            RunCmd();
        }
    }

private:
    struct AppId
    {
        static FGuid Get()
        {
            static FGuid Id = FGuid();

            if (Id == FGuid())
            {
                if (!TryReadFromFile(Id))
                {
                    Id = FGuid::NewGuid();
                    SaveGuid(Id);
                }
            }

            return Id;
        }

        static FString GetIdFilePath()
        {
            return FPaths::Combine(FPaths::EngineVersionAgnosticUserDir(), TEXT("Unlog"), TEXT("Id"));
        }

        static bool TryReadFromFile(FGuid& OutGuid)
        {
            FString FileData;
            if (FFileHelper::LoadFileToString(FileData, *GetIdFilePath()))
            {
                FGuid Result;
                if (FGuid::Parse(FileData, Result))
                {
                    OutGuid = Result;
                    return true;
                }
            }

            return false;
        }

        static void SaveGuid(const FGuid& Guid)
        {
            FFileHelper::SaveStringToFile(Guid.ToString(), *GetIdFilePath());
        }
    };
};
#endif // WITH_EDITOR && UNLOG_ENABLED

// ------------------------------------------------------------------------------------
// Unlog runtime
// ------------------------------------------------------------------------------------
#if UNLOG_ENABLED
class Unlogger
{
private:
    // Settings should never be destroyed since they are statically created
    UnlogRuntimeSettingsBase* Settings;

    // Pushed categories temporarily override the default category, usually during a certain scope
    TArray<UnlogCategoryBase*> PushedCategories;

public:

    static Unlogger& Get()
    {
        static Unlogger Logger = CreateLogger();
        return Logger;
    }

    static Unlogger CreateLogger()
    {
        // Start with default settings
        Unlogger Logger;
        Logger.ApplyRuntimeSettingsInternal<UnlogDefaultRuntimeSettings>();

#if WITH_EDITOR
        static const FTelemetryDispatcher TelemetryDispatcher = FTelemetryDispatcher();
#endif
        return Logger;
    }

    template< typename TSettings >
    static void ApplyRuntimeSettings()
    {
        Unlogger::Get().ApplyRuntimeSettingsInternal<TSettings>();
    }

    template< typename TSettings >
    void ApplyRuntimeSettingsInternal()
    {
        Settings = &TSettings::Static();
    }

    void PushCategory(UnlogCategoryBase& Category)
    {
        PushedCategories.Push(&Category);
    }

    void PopCategory()
    {
        check(PushedCategories.Num() > 0);
        PushedCategories.Pop();
    }

    FORCEINLINE static FString StringFormat(const TCHAR* Format, const FStringFormatOrderedArguments& Args)
    {
        return FString::Format(Format, Args);
    }

    FORCEINLINE static FString StringFormat(const char* Format, const FStringFormatOrderedArguments& Args)
    {
        return FString::Format( UTF8_TO_TCHAR(Format), Args);
    }


    template<typename CategoryPicker>
    FORCEINLINE const UnlogCategoryBase& PickCategory()
    {
        UnlogCategoryBase* SelectedCategory = PushedCategories.Num() > 0 ? PushedCategories.Last() : nullptr;

        CategoryPicker::PickCategory(SelectedCategory);

        check(SelectedCategory);
        return *SelectedCategory;
    }

    // Use ordered arguments format
    template<
        typename FormatOptions,
        typename FMT,
        typename... ArgTypes >
    FORCEINLINE typename TEnableIf<!FormatOptions::IsPrintfFormat, FString>::Type  ProcessFormatString(const FMT& Format, ArgTypes... Args)
    {
        static_assert(TAnd<TIsConstructible<FStringFormatArg, ArgTypes>...>::Value, "Invalid argument type passed to UnlogPrivateImpl");
        return Unlogger::StringFormat( Format, FStringFormatOrderedArguments({ Args... }) );
    }

    // Use Printf format
    template<
        typename FormatOptions,
        typename FMT,
        typename... ArgTypes >
    FORCEINLINE typename TEnableIf<FormatOptions::IsPrintfFormat, FString>::Type ProcessFormatString(const FMT& Format, ArgTypes... Args)
    {
        static_assert(!TIsArrayOrRefOfType<FMT, char>::Value, "Unlog's printf style functions only support text wrapped by TEXT()");
        FString Result = FString::Printf(Format, Args...);
        return Result;
    }

    template<typename StaticConfiguration, typename FMT, typename... ArgTypes>
    void UnlogPrivateImpl(const FMT& Format, ELogVerbosity::Type Verbosity, ArgTypes... Args)
    {
        const auto& Category = PickCategory< typename StaticConfiguration::CategoryPicker>();
        FName CategoryName = Category.GetName();

        if (Verbosity <= Category.GetVerbosity() && Verbosity != ELogVerbosity::NoLogging)
        {
            FString Result = ProcessFormatString<typename StaticConfiguration::FormatOptions>(Format, Args...);

            // Execute all static targets
            StaticConfiguration::TargetOptions::Call(Category, Verbosity, Result);
        }
    }
};
#endif // UNLOG_ENABLED

#if UNLOG_ENABLED
template< typename TCategory >
struct FUnlogScopedCategory
{
    FUnlogScopedCategory()
    {
        Unlogger::Get().PushCategory(TCategory::Static());
    }

    ~FUnlogScopedCategory()
    {
        Unlogger::Get().PopCategory();
    }
};
#endif

// ------------------------------------------------------------------------------------
// Contexts (Experimental)
// 
// Tracks when the program has entered a specific "context", able to be queried 
// by another system later down the callstack to selectively exclude certain log calls
// without creating a direct dependency between systems to query state.
// 
// e.g. Ignore logging a widget when part of the editor callstack
// 
// Could technically be repurposed to selectively run other code besides logging.
// ------------------------------------------------------------------------------------
template <typename ActualType>
class UnlogContextBase
{
public:

    UnlogContextBase(const FName& InName)
        : ContextName(InName)
        , Counter(0u)
    {}

    FORCEINLINE static ActualType& Static()
    {
        static ActualType Type = ActualType::Construct();
        return Type;
    }

    FORCEINLINE const FName& GetName() const
    {
        return ContextName;
    }

    FORCEINLINE void IncrementCounter()
    {
        Counter++;
    }

    FORCEINLINE void DecrementCounter()
    {
        check(Counter > 0u);
        Counter--;
    }

    FORCEINLINE bool IsActive() const
    {
        return Counter > 0u;
    }

    template< typename Functor >
    FORCEINLINE static void WhenActive(Functor Func)
    {
        if (ActualType::Static().IsActive())
        {
            Func();
        }
    }

    template< typename Functor >
    FORCEINLINE static void WhenNotActive(Functor Func)
    {
        if (!ActualType::Static().IsActive())
        {
            Func();
        }
    }

private:
    FName ContextName;
    uint32 Counter;
};

#define UNLOG_CONTEXT(ContextName) \
class ContextName : public UnlogContextBase< ContextName > \
{\
public:\
    using UnlogContextBase< ContextName >::UnlogContextBase; \
    static ContextName Construct()\
    {\
        return ContextName( TEXT( #ContextName ) );\
    }\
};

template <typename TContext >
struct UnloggerScopedContextEntered
{
    UnloggerScopedContextEntered(bool InValue)
        : Value(InValue)
    {
        if (Value)
        {
            TContext::Static().IncrementCounter();
        }
    }

    UnloggerScopedContextEntered()
        : UnloggerScopedContextEntered(true)
    {}

    ~UnloggerScopedContextEntered()
    {
        if (Value)
        {
            TContext::Static().DecrementCounter();
        }
    }
private:
    bool Value;
};

#define UNLOG_CONTEXT_ENTERED(ContextName, ...) \
    UnloggerScopedContextEntered< ContextName > ScopedContext_##ContextName( __VA_ARGS__ )


// ------------------------------------------------------------------------------------
// Targets
// 
// Specifies where log messages should be routed to.
// 
// By default all messages are routed to FMsg::Log so it's as similar as executing 
// UE_LOG. There's also a few other options like the Viewport or the MessageLog.
// 
// Multiple targets can be used by chaining them inside a TMultiTarget
// ------------------------------------------------------------------------------------
namespace Target
{
    /**
    * Combines multiple targets together.
    * e.g: TMultiTarget< Target::UELog, Target::Viewport>
    */
    template< typename... TTargets >
    struct TMultiTarget
    {
        static void Call(const UnlogCategoryBase& Category, ELogVerbosity::Type Verbosity, const FString& Message)
        {
            auto Ignore = { (TTargets::Call(Category, Verbosity, Message),0)... };
        }
    };

    // Default logging target option just like UE_LOG
    struct UELog
    {
        static void Call(const UnlogCategoryBase& Category, ELogVerbosity::Type Verbosity, const FString& Message)
        {
            FMsg::Logf(nullptr, 0, Category.GetName(), Verbosity, TEXT("%s"), *Message);
        }
    };

    /**
    * Output messages to the in-game viewport.
    * Both TimeOnScreen and Color are statically configurable
    */
    template< int TimeOnScreen, const FColor& InColor >
    struct TViewport
    {
        static void Call(const UnlogCategoryBase& Category, ELogVerbosity::Type Verbosity, const FString& Message)
        {
            GEngine->AddOnScreenDebugMessage(INDEX_NONE, TimeOnScreen, InColor, Message);
        }
    };

    // Default viewport configuration - outputs the message on-screen for 3 seconds and colored in Cyan
    using Viewport = TViewport<3, FColor::Cyan>;

    using Default = UELog;
}

// ------------------------------------------------------------------------------------
// Category pickers
// 
// Specifies the behaviour when the logger is picking which category to use.
// ------------------------------------------------------------------------------------

// Use a specific category represented by TCategory
template< typename TCategory >
struct TSpecificCategory
{
    static void PickCategory(UnlogCategoryBase*& InOutCategory)
    {
        InOutCategory = &TCategory::Static();
    }
};

// Try to infer the category (i.e using the scope) but can also specify which category to fallback to
template< typename TFallbackCategory = LogGeneral >
struct TDeriveCategory
{
    static void PickCategory(UnlogCategoryBase*& InOutCategory)
    {
        // Only populate with default category if it's not already derived
        if (InOutCategory == nullptr)
        {
            InOutCategory = &TFallbackCategory::Static();
        }
    }
};

// ------------------------------------------------------------------------------------
// TUnlog
// 
// Lets you create your own logger using builder-like pattern.
// 
// Simple configuration:
// using MyLogger = TUnlog<>;
// ------------------------------------------------------------------------------------
template<typename InTargetOptions = Target::Default, typename InCategoryPicker = TDeriveCategory<> >
struct TUnlog
{
    using CategoryPicker = InCategoryPicker;
    using TargetOptions = InTargetOptions;

    /**
    * Specify which targets to output the messages to overriding any previous configuration.
    * Can use multiple targets.
    */
    template< typename... Targets >
    using WithTargets = TUnlog< Target::TMultiTarget<Targets...>, InCategoryPicker >;

    // Similar to WithTargets but cumulative to whatever configuration it had before. 
    template< typename... Targets >
    using AddTarget = TUnlog< Target::TMultiTarget<InTargetOptions, Targets...>, InCategoryPicker >;

    /**
    * Specify the default category this logger should use without removing the ability 
    * to derive the category if needed.
    */ 
    template< typename InCategory >
    using WithDefaultCategory = TUnlog< InTargetOptions, TDeriveCategory<InCategory> >;

    // Sets a specific category and removes any ability to infer the category
    template< typename InCategory >
    using WithCategory = TUnlog< InTargetOptions, TSpecificCategory<InCategory> >;

    // Logging functions generation
    UNLOG_DECLARE_CATEGORY_LOG_FUNCTION(CategoryPicker, TargetOptions, Log, Log)
    UNLOG_DECLARE_CATEGORY_LOG_FUNCTION(CategoryPicker, TargetOptions, Warn, Warning)
    UNLOG_DECLARE_CATEGORY_LOG_FUNCTION(CategoryPicker, TargetOptions, Error, Error)
    UNLOG_DECLARE_CATEGORY_LOG_FUNCTION(CategoryPicker, TargetOptions, Display, Display)
    UNLOG_DECLARE_CATEGORY_LOG_FUNCTION(CategoryPicker, TargetOptions, Verbose, Verbose)
    UNLOG_DECLARE_CATEGORY_LOG_FUNCTION(CategoryPicker, TargetOptions, VeryVerbose, VeryVerbose)
};

// ------------------------------------------------------------------------------------
//  Unlog macros helpers
// ------------------------------------------------------------------------------------

#if UNLOG_ENABLED
namespace UnlogMacroHelpers
{
    // Inlined function when using any of the user-facing macro functions
    template< bool IsPrintfFormat, typename MacroOptions, typename FMT, typename... TArgs>
    FORCEINLINE void Run(ELogVerbosity::Type InVerbosity, const FMT& Format, TArgs... Args)
    {
        using Configuration = TStaticConfiguration<
            TFormatOptions<IsPrintfFormat>,
            typename MacroOptions::UnlogOptions::CategoryPicker,
            typename MacroOptions::UnlogOptions::TargetOptions
        >;

        Unlogger::Get().UnlogPrivateImpl<Configuration>(Format, InVerbosity, Args...);
    }

    template <bool IsPrintfFormat, ELogVerbosity::Type InVerbosity, typename MacroOptions, typename FMT, typename... TParms>
    FORCEINLINE static void Run(const FMT& Format, TParms... Args)
    {
        Run<IsPrintfFormat, MacroOptions>(InVerbosity, Format, Args...);
    }

    // Helpers structure to hold the base configuration (usually the class named "Unlog")
    // and any options overriding the base configuration
    template<typename InMacroOverrides, typename TBaseUnlogSettings >
    struct TMacroOptions
    {
        using UnlogOptions = typename InMacroOverrides::template GetSettings<TBaseUnlogSettings>;
    };

    // TMacroArgs is used to statically generate the appropriate settings when using the macros
    template<typename... TArgs>
    struct TMacroArgs;

    // Matches when passing a TUnlog type settings
    template<template <typename, typename> class TUnlogSettings, typename TargetOptions, typename CategoryPicker >
    struct TMacroArgs< TUnlogSettings< TargetOptions, CategoryPicker> >
    {
        template< typename TBaseSettings >
        using GetSettings = TUnlog< TargetOptions, CategoryPicker>;
    };

    // Matches when passing just a category
    template<typename TCategory>
    struct TMacroArgs<TCategory>
    {
        template< typename TBaseSettings >
        using GetSettings = TUnlog< typename TBaseSettings::TargetOptions, TSpecificCategory<TCategory> >;
    };

    // Default match, don't override any settings.
    template<>
    struct TMacroArgs<>
    {
        template< typename TBaseSettings >
        using GetSettings = TBaseSettings;
    };
};
#endif // UNLOG_ENABLED

// ------------------------------------------------------------------------------------
//  Unlog Macros 
// ------------------------------------------------------------------------------------

#if UNLOG_ENABLED

#define PRIV_EXPAND( A ) A
#define PRIV_UNLOG_PARAMS( Message, ... ) ( TEXT( Message ), ##__VA_ARGS__ )
#define PRIV_MACRO_BASED_ON_ARG_NUM( _1, _2, FUNCTION, ... ) FUNCTION

// One parameter matches UNLOG( Verbosity )
#define PRIV_UNLOG_OneParam( IsPrintfFormat, InVerbosity ) \
    UnlogMacroHelpers::Run< IsPrintfFormat, ELogVerbosity::InVerbosity,UnlogMacroHelpers::TMacroOptions< UnlogMacroHelpers::TMacroArgs<>, Unlog > > PRIV_UNLOG_PARAMS

// Two parameters matches UNLOG( Category, Verbosity ) or UNLOG( Options, Verbosity ) 
#define PRIV_UNLOG_TwoParams( IsPrintfFormat, OptionsOrCategory, InVerbosity ) \
    UnlogMacroHelpers::Run< IsPrintfFormat, ELogVerbosity::InVerbosity, UnlogMacroHelpers::TMacroOptions< UnlogMacroHelpers::TMacroArgs< OptionsOrCategory >, Unlog > > PRIV_UNLOG_PARAMS


#define PRIV_UNLOG_IMPL(IsPrintfFormat, ...) \
    PRIV_EXPAND( \
        PRIV_MACRO_BASED_ON_ARG_NUM( __VA_ARGS__ , PRIV_UNLOG_TwoParams, PRIV_UNLOG_OneParam) ( IsPrintfFormat, __VA_ARGS__ ) )

/**
* Uses a particular syntax that separates the logging options from its contents
* Examples:
* UNLOG( Error )( "Failed to validate object: {0}", Object->ToString() );
* UNLOG( MySpecificCategory, Warning )( "Logging on a specific category" );
* UNLOG( Unlog::WithTarget< Targets::Viewport >, Log )( "Using custom logger options");
*/
#define UNLOG(...)              PRIV_UNLOG_IMPL( false, __VA_ARGS__ )

// Printf version of the UNLOG macro function
#define UNLOGF(...)             PRIV_UNLOG_IMPL( true, __VA_ARGS__ )

/**
* Just like the UNLOG macro, but taking a condition as first argument
* Condition is not evaluated if logging is disabled by the compiler
*/
#define UNCLOG(Condition, ...)  if(Condition) UNLOG( __VA_ARGS__ )

// Printf version of the UNCLOG macro
#define UNCLOGF(Condition, ...) if(Condition) UNLOGF( __VA_ARGS__ )

#else
#define UNLOG_PARAMS_EMPTY(...) 
#define UNLOG(...) UNLOG_PARAMS_EMPTY
#define UNLOGF(...) UNLOG_PARAMS_EMPTY
#define UNCLOG(Condition, ...) UNLOG_PARAMS_EMPTY
#define UNCLOGF(Condition, ...) UNLOG_PARAMS_EMPTY
#endif // UNLOG_ENABLED

// ------------------------------------------------------------------------------------
//  Unlog Legacy Macros 
// 
//  Same syntax as UE_LOG and can be easily retrofitted in code-bases that were
//  already using it.
// ------------------------------------------------------------------------------------

#if UNLOG_ENABLED

#define UN_LOG( InMacroArgs, VerbosityName, Message, ... ) \
    UnlogMacroHelpers::Run< false, UnlogMacroHelpers::TMacroOptions< UnlogMacroHelpers::TMacroArgs< InMacroArgs >, \
    Unlog > >( ELogVerbosity::VerbosityName,  Message , ##__VA_ARGS__);

#define UN_LOGF( InMacroArgs, VerbosityName, Message, ... ) \
    UnlogMacroHelpers::Run< true, UnlogMacroHelpers::TMacroOptions< UnlogMacroHelpers::TMacroArgs< InMacroArgs >, Unlog > >( ELogVerbosity::VerbosityName, TEXT( Message ), ##__VA_ARGS__);

#define UN_CLOG( Condition, InMacroArgs, VerbosityName, Message, ... ) \
    { \
        if( Condition ) \
        {\
            UnlogMacroHelpers::Run< false, UnlogMacroHelpers::TMacroOptions< UnlogMacroHelpers::TMacroArgs< InMacroArgs >, Unlog > >( ELogVerbosity::VerbosityName, TEXT( Message ), ##__VA_ARGS__); \
        }\
    }
#define UN_CLOGF( Condition, InMacroArgs, VerbosityName, Message, ... ) \
    {\
        if( Condition ) \
        {\
            UnlogMacroHelpers::Run< true, UnlogMacroHelpers::TMacroOptions< UnlogMacroHelpers::TMacroArgs< InMacroArgs >, Unlog > >( ELogVerbosity::VerbosityName, TEXT( Message ), ##__VA_ARGS__); \
        }\
    }
#else
#define UN_LOG(...) UNLOG_COMPILED_OUT
#define UN_LOGF(...) UNLOG_COMPILED_OUT
#define UN_CLOG(...) UNLOG_COMPILED_OUT
#define UN_CLOGF(...) UNLOG_COMPILED_OUT
#endif // UNLOG_ENABLED
