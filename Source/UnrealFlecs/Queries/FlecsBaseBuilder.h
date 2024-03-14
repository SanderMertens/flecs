// Solstice Games © 2024. All Rights Reserved.

#ifndef FLECS_BASE_BUILDER_H
#define FLECS_BASE_BUILDER_H

#include "CoreMinimal.h"
#include "Macros.h"
#include "Entities/FlecsEntityHandle.h"
#include "Entities/FlecsId.h"

template <typename T, typename Derived, typename Other>
struct TFlecsBaseQueryBuilder
{
    using BuilderType = T;
    
public:
    FORCEINLINE explicit TFlecsBaseQueryBuilder(const flecs::world& InWorld, const char* InName = "")
        : Builder(InWorld, InName)
    {
    }

    FORCEINLINE TFlecsBaseQueryBuilder(const BuilderType& InBuilder) : Builder(InBuilder) {}
    FORCEINLINE TFlecsBaseQueryBuilder(BuilderType* InBuilder) : Builder(*InBuilder) {}

    FORCEINLINE NO_DISCARD BuilderType& GetFlecsBuilder() { return Builder; }
    FORCEINLINE NO_DISCARD const BuilderType& GetFlecsBuilder() const { return Builder; }

    FORCEINLINE operator BuilderType&() { return GetFlecsBuilder(); }
    FORCEINLINE operator const BuilderType&() const { return GetFlecsBuilder(); }
    
    FORCEINLINE NO_DISCARD BuilderType* operator->() { return &Builder; }
    FORCEINLINE NO_DISCARD const BuilderType* operator->() const { return &Builder; }

    template <typename ...TArgs>
    FORCEINLINE NO_DISCARD Derived& Term(const TArgs&... InArgs)
    {
        Builder.term(InArgs...);
        return *static_cast<Derived*>(this);
    }
    
    FORCEINLINE NO_DISCARD Derived& Term(const FString& InName)
    {
        Builder.term(TCHAR_TO_ANSI(*InName));
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Term(const FString& InName, const FString& InRelation)
    {
        Builder.term(TCHAR_TO_ANSI(*InName), TCHAR_TO_ANSI(*InRelation));
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Term(const FFlecsId& InId)
    {
        Builder.term(InId.GetFlecsId());
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Term(const FFlecsId& InId, const FFlecsId& InRelation)
    {
        Builder.term(InId.GetFlecsId(), InRelation.GetFlecsId());
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Term(const FString& InName, const FFlecsId& InRelation)
    {
        Builder.term(TCHAR_TO_ANSI(*InName), InRelation.GetFlecsId());
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Term(const FFlecsId& InId, const FString& InRelation)
    {
        Builder.term(InId.GetFlecsId(), TCHAR_TO_ANSI(*InRelation));
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Term(const FFlecsEntityHandle& InEntity)
    {
        Builder.term(InEntity.GetEntity());
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Term(const FFlecsEntityHandle& InEntity, const FFlecsId& InRelation)
    {
        Builder.term(InEntity.GetEntity(), InRelation.GetFlecsId());
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Term(const FFlecsEntityHandle& InEntity, const FString& InRelation)
    {
        Builder.term(InEntity.GetEntity(), TCHAR_TO_ANSI(*InRelation));
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& TermAt(const int32 InIndex)
    {
        Builder.term_at(InIndex);
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& And()
    {
        Builder.and_();
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Or()
    {
        Builder.or_();
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Not()
    {
        Builder.not_();
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Optional()
    {
        Builder.optional();
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Parent(const FFlecsId& InId)
    {
        Builder.parent(InId.GetFlecsId());
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Scope(const FFlecsId& InId)
    {
        Builder.scope(InId.GetFlecsId());
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Self(const FFlecsId& InId)
    {
        Builder.self(InId.GetFlecsId());
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Cascade()
    {
        Builder.cascade();
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Cascade(const FFlecsId& InId)
    {
        Builder.cascade(InId.GetFlecsId());
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Up(const FFlecsId& InId)
    {
        Builder.up(InId.GetFlecsId());
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Flags(flecs::flags32_t InFlags)
    {
        Builder.flags(InFlags);
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Role(flecs::id_t InRole)
    {
        Builder.role(InRole);
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& InOut(flecs::inout_kind_t InOut)
    {
        Builder.inout(InOut);
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Operator(flecs::oper_kind_t Operator)
    {
        Builder.oper(Operator);
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Id(const FFlecsId& InId)
    {
        Builder.id(InId.GetFlecsId());
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Entity(const FFlecsEntityHandle& InEntity)
    {
        Builder.entity(InEntity.GetEntity());
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Name(const FString& InName)
    {
        Builder.name(TCHAR_TO_ANSI(*InName));
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Var(const FString& InVarName)
    {
        Builder.var(TCHAR_TO_ANSI(*InVarName));
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& ScopeOpen()
    {
        Builder.scope_open();
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& ScopeClose()
    {
        Builder.scope_close();
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Desc()
    {
        Builder.desc();
        return *static_cast<Derived*>(this);
    }
    
    FORCEINLINE NO_DISCARD Other Build()
    {
        return Other(Builder.build());
    }
    
    template <typename ...TArgs>
    FORCEINLINE NO_DISCARD Derived& With(const TArgs&... InArgs)
    {
        Builder.template with<TArgs...>(std::forward<TArgs>(InArgs)...);
        return *static_cast<Derived*>(this);
    }

    template <typename First, typename Second>
    FORCEINLINE NO_DISCARD Derived& With()
    {
        Builder.template with<First, Second>();
        return *static_cast<Derived*>(this);
    }
    
    template <typename First, typename ...TArgs>
    FORCEINLINE NO_DISCARD Derived& With(TArgs... InArgs)
    {
        Builder.template with<First, TArgs...>(std::forward<TArgs>(InArgs)...);
        return *static_cast<Derived*>(this);
    }


    template <typename ...TArgs>
    FORCEINLINE NO_DISCARD Derived& Without(TArgs... InArgs)
    {
        Builder.template without<TArgs...>(std::forward<TArgs>(InArgs)...);
        return *static_cast<Derived*>(this);
    }

    template <typename First, typename Second>
    FORCEINLINE NO_DISCARD Derived& Without()
    {
        Builder.template without<First, Second>();
        return *static_cast<Derived*>(this);
    }

    template <typename First, typename ...TArgs>
    FORCEINLINE NO_DISCARD Derived& Without(TArgs... InArgs)
    {
        Builder.template without<First, TArgs...>(std::forward<TArgs>(InArgs)...);
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Write()
    {
        Builder.write();
        return *static_cast<Derived*>(this);
    }

    template <typename ...TArgs>
    FORCEINLINE NO_DISCARD Derived& Write(const TArgs&... InArgs)
    {
        Builder.template write<TArgs...>(std::forward<TArgs>(InArgs)...);
        return *static_cast<Derived*>(this);
    }

    template <typename First, typename Second>
    FORCEINLINE NO_DISCARD Derived& Write()
    {
        Builder.template write<First, Second>();
        return *static_cast<Derived*>(this);
    }

    template <typename First, typename ...TArgs>
    FORCEINLINE NO_DISCARD Derived& Write(TArgs... InArgs)
    {
        Builder.template write<First, TArgs...>(std::forward<TArgs>(InArgs)...);
        return *static_cast<Derived*>(this);
    }

    FORCEINLINE NO_DISCARD Derived& Read()
    {
        Builder.read();
        return *static_cast<Derived*>(this);
    }

    template <typename ...TArgs>
    FORCEINLINE NO_DISCARD Derived& Read(const TArgs&... InArgs)
    {
        Builder.template read<TArgs...>(std::forward<TArgs>(InArgs)...);
        return *static_cast<Derived*>(this);
    }

    template <typename First, typename Second>
    FORCEINLINE NO_DISCARD Derived& Read()
    {
        Builder.template read<First, Second>();
        return *static_cast<Derived*>(this);
    }

    template <typename First, typename ...TArgs>
    FORCEINLINE NO_DISCARD Derived& Read(TArgs... InArgs)
    {
        Builder.template read<First, TArgs...>(std::forward<TArgs>(InArgs)...);
        return *static_cast<Derived*>(this);
    }

protected:
    BuilderType Builder;
}; // struct TFlecsBaseQueryBuilder

#endif // FLECS_BASE_BUILDER_H