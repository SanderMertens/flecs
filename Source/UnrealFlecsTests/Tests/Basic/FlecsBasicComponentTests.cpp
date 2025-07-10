#pragma once

#if WITH_AUTOMATION_TESTS

#include "CQTest.h"
#include "Fixtures/FlecsWorldFixture.h"
#include "Misc/AutomationTest.h"
#include "Tests/FlecsTestTypes.h"
#include "Translators/FlecsTranslationSettingTraits.h"
#include "Worlds/FlecsWorld.h"

/**
 * Layout of the tests:
 * A. Basic Component Registration Tests
 * B. Component Properties/Trait Registration Tests
 * C. Enum Component Registration Tests
 * D. Edge Case Tests
 **/
TEST_CLASS_WITH_FLAGS_AND_TAGS(A2_UnrealFlecsComponentRegistrationTests,
							   "UnrealFlecs.Basic.Components",
							   EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter
								| EAutomationTestFlags::CriticalPriority,
							   "[Flecs][Basic][Entity][Tag][Component][Registration]")
{
	inline static TUniquePtr<FFlecsTestFixtureRAII> Fixture;
	inline static TObjectPtr<UFlecsWorld> FlecsWorld = nullptr;

	BEFORE_EACH()
	{
		Fixture = MakeUnique<FFlecsTestFixtureRAII>();
		FlecsWorld = Fixture->Fixture.GetFlecsWorld();
	}

	AFTER_EACH()
	{
		Fixture.Reset();
		FlecsWorld = nullptr;
	}

	TEST_METHOD_WITH_TAGS(A1_BasicUSTRUCTComponentRegistration_CPPAPI,
						 "[Flecs][Basic][Component][USTRUCT][Registration][USTRUCT][CPP-API]")
	{
		const FFlecsEntityHandle StructEntity = FlecsWorld->RegisterComponentType<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(StructEntity.IsValid()));
		
		ASSERT_THAT(IsTrue(StructEntity.IsComponent()));
		ASSERT_THAT(IsFalse(StructEntity.IsTag()));
		
		const FFlecsEntityHandle StaticStructEntity = FlecsWorld->RegisterComponentType(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(StaticStructEntity.IsValid()));

		ASSERT_THAT(IsTrue(StaticStructEntity.IsComponent()));
		ASSERT_THAT(IsFalse(StaticStructEntity.IsTag()));

		ASSERT_THAT(IsTrue(StructEntity == StaticStructEntity));
	}

	TEST_METHOD_WITH_TAGS(A2_BasicUSTRUCTComponentRegistration_StaticStructAPI,
		"[Flecs][Basic][Component][Registration][USTRUCT][StaticStruct-API]")
	{
		const FFlecsEntityHandle StaticStructEntity = FlecsWorld->RegisterComponentType(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(StaticStructEntity.IsValid()));

		ASSERT_THAT(IsTrue(StaticStructEntity.IsComponent()));
		ASSERT_THAT(IsFalse(StaticStructEntity.IsTag()));
		
		const FFlecsEntityHandle StructEntity = FlecsWorld->RegisterComponentType<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(StructEntity.IsValid()));

		ASSERT_THAT(IsTrue(StructEntity.IsComponent()));
		ASSERT_THAT(IsFalse(StructEntity.IsTag()));

		ASSERT_THAT(IsTrue(StaticStructEntity == StructEntity));
	}

	TEST_METHOD_WITH_TAGS(A3_BasicComponentRegistration_CPPAPI,
						 "[Flecs][Basic][Component][USTRUCT][Registration][CPP-API]")
	{
		const FFlecsEntityHandle StructEntity = FlecsWorld->RegisterComponentType<FFlecsTest_CPPStructValue>();
		ASSERT_THAT(IsTrue(StructEntity.IsValid()));

		ASSERT_THAT(IsTrue(StructEntity.IsComponent()));
		ASSERT_THAT(IsFalse(StructEntity.IsTag()));
	}

	TEST_METHOD_WITH_TAGS(B1_GetComponentPropertyTraits_CPPAPI,
						 "[Flecs][Basic][Component][USTRUCT][Registration][Property-Traits][CPP-API]")
	{
		const FFlecsEntityHandle StructEntity = FlecsWorld->RegisterComponentType<FFlecsTestStruct_WithPropertyTraits>();
		ASSERT_THAT(IsTrue(StructEntity.IsValid()));

		ASSERT_THAT(IsTrue(StructEntity.Has(flecs::Trait)));
	}

	TEST_METHOD_WITH_TAGS(B2_GetComponentPropertyTraits_StaticStructAPI,
		"[Flecs][Basic][Component][USTRUCT][Registration][Property-Traits][StaticStruct-API]")
	{
		const FFlecsEntityHandle StaticStructEntity = FlecsWorld->RegisterComponentType(FFlecsTestStruct_WithPropertyTraits::StaticStruct());
		ASSERT_THAT(IsTrue(StaticStructEntity.IsValid()));

		ASSERT_THAT(IsTrue(StaticStructEntity.Has(flecs::Trait)));
	}

	TEST_METHOD_WITH_TAGS(B3_GetComponentPropertyTraitsFromAnotherModule_CPPAPI,
						 "[Flecs][Basic][Component][USTRUCT][Registration][Property-Traits][CPP-API]")
	{
		const FFlecsEntityHandle StructEntity = FlecsWorld->RegisterComponentType<FFlecsTranslationPropertyTrait>();
		ASSERT_THAT(IsTrue(StructEntity.IsValid()));

		ASSERT_THAT(IsTrue(StructEntity.Has(flecs::PairIsTag)));
	}

	TEST_METHOD_WITH_TAGS(B4_GetComponentPropertyTraitsFromAnotherModule_StaticStructAPI,
		"[Flecs][Basic][Component][USTRUCT][Registration][Property-Traits][StaticStruct-API]")
	{
		const FFlecsEntityHandle StaticStructEntity = FlecsWorld->RegisterComponentType(FFlecsTranslationPropertyTrait::StaticStruct());
		ASSERT_THAT(IsTrue(StaticStructEntity.IsValid()));

		ASSERT_THAT(IsTrue(StaticStructEntity.Has(flecs::PairIsTag)));
	}

	TEST_METHOD_WITH_TAGS(C1_EnumComponentRegistration_CPPAPI,
						 "[Flecs][Basic][Component][UENUM][Registration][Enums][CPP-API]")
	{
		const FFlecsEntityHandle EnumEntity = FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(IsTrue(EnumEntity.IsValid()));

		ASSERT_THAT(IsTrue(EnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(EnumEntity.IsEnum()));

		const FFlecsEntityHandle StaticEnumEntity = FlecsWorld->RegisterScriptEnum(StaticEnum<EFlecsTestEnum_UENUM>());
		ASSERT_THAT(IsTrue(StaticEnumEntity.IsValid()));
		ASSERT_THAT(IsTrue(StaticEnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(StaticEnumEntity.IsEnum()));

		ASSERT_THAT(IsTrue(EnumEntity == StaticEnumEntity));
	}

	TEST_METHOD_WITH_TAGS(C2_EnumComponentRegistration_StaticStructAPI,
		"[Flecs][Basic][Component][UENUM][Registration][Enums][StaticStruct-API]")
	{
		const FFlecsEntityHandle StaticEnumEntity = FlecsWorld->RegisterScriptEnum(StaticEnum<EFlecsTestEnum_UENUM>());
		ASSERT_THAT(IsTrue(StaticEnumEntity.IsValid()));
		
		ASSERT_THAT(IsTrue(StaticEnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(StaticEnumEntity.IsEnum()));

		const FFlecsEntityHandle EnumEntity = FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(IsTrue(EnumEntity.IsValid()));

		ASSERT_THAT(IsTrue(EnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(EnumEntity.IsEnum()));

		ASSERT_THAT(IsTrue(StaticEnumEntity == EnumEntity));
	}

	TEST_METHOD_WITH_TAGS(C3_SparseEnumComponentRegistration_CPPAPI,
						 "[Flecs][Basic][Component][UENUM][Registration][Enums][SparseEnum]")
	{
		const FFlecsEntityHandle SparseEnumEntity = FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(IsTrue(SparseEnumEntity.IsValid()));

		ASSERT_THAT(IsTrue(SparseEnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(SparseEnumEntity.IsEnum()));

		const FFlecsEntityHandle StaticSparseEnumEntity = FlecsWorld->RegisterScriptEnum(StaticEnum<EFlecsTestEnum_UENUM>());
		ASSERT_THAT(IsTrue(StaticSparseEnumEntity.IsValid()));
		
		ASSERT_THAT(IsTrue(StaticSparseEnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(StaticSparseEnumEntity.IsEnum()));

		ASSERT_THAT(IsTrue(SparseEnumEntity == StaticSparseEnumEntity));
	}

	TEST_METHOD_WITH_TAGS(C4_SparseEnumComponentRegistration_StaticStructAPI,
		"[Flecs][Basic][Component][UENUM][Registration][Enums][SparseEnum]")
	{
		const FFlecsEntityHandle StaticSparseEnumEntity = FlecsWorld->RegisterScriptEnum(StaticEnum<EFlecsTestEnum_UENUM>());
		ASSERT_THAT(IsTrue(StaticSparseEnumEntity.IsValid()));

		ASSERT_THAT(IsTrue(StaticSparseEnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(StaticSparseEnumEntity.IsEnum()));

		const FFlecsEntityHandle SparseEnumEntity = FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(IsTrue(SparseEnumEntity.IsValid()));

		ASSERT_THAT(IsTrue(SparseEnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(SparseEnumEntity.IsEnum()));

		ASSERT_THAT(IsTrue(StaticSparseEnumEntity == SparseEnumEntity));
	}

	TEST_METHOD_WITH_TAGS(D1_RegisterScriptStructOneByteOneUProperty_CPPAPI,
		"[Flecs][Basic][Component][USTRUCT][Registration][EdgeCase][CPP-API]")
	{
		const FFlecsEntityHandle OneByteOneUPropertyEntity = FlecsWorld->RegisterComponentType<FUStructTestComponent_NonTagUSTRUCT>();
		ASSERT_THAT(IsTrue(OneByteOneUPropertyEntity.IsValid()));

		ASSERT_THAT(IsTrue(OneByteOneUPropertyEntity.IsComponent()));
		ASSERT_THAT(IsFalse(OneByteOneUPropertyEntity.IsTag()));
	}

	TEST_METHOD_WITH_TAGS(D2_RegisterScriptStructOneByteOneUProperty_StaticStructAPI,
		"[Flecs][Basic][Component][USTRUCT][Registration][EdgeCase][StaticStruct-API]")
	{
		const FFlecsEntityHandle OneByteOneUPropertyEntity = FlecsWorld->RegisterComponentType(FUStructTestComponent_NonTagUSTRUCT::StaticStruct());
		ASSERT_THAT(IsTrue(OneByteOneUPropertyEntity.IsValid()));

		ASSERT_THAT(IsTrue(OneByteOneUPropertyEntity.IsComponent()));
		ASSERT_THAT(IsFalse(OneByteOneUPropertyEntity.IsTag()));
	}
	
}; // End of A2_UnrealFlecsComponentRegistrationTests


/**
 * Layout of the tests:
 * A. Tag Add/Remove Tests
 * B. Component Add/Set/Remove/Assign Tests
 * C. Component Modified/Set Tests
 * D. Toggle Component Tests
 */
TEST_CLASS_WITH_FLAGS_AND_TAGS(A3_UnrealFlecsBasicComponentTests,
                               "UnrealFlecs.Basic.Components",
                               EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter,
                               "[Flecs][Basic][Entity][Tag][Component]")
{
	inline static TUniquePtr<FFlecsTestFixtureRAII> Fixture;
	inline static TObjectPtr<UFlecsWorld> FlecsWorld = nullptr;

	inline static FFlecsEntityHandle TagEntity;
	inline static FFlecsEntityHandle ValuedComponentEntity;
	inline static FFlecsComponentHandle ToggleableComponentHandle;

	inline static FFlecsEntityHandle TestEntity;
	inline static FFlecsEntityHandle ToggleableEntityTest;

	inline static flecs::query<FFlecsTestStruct_Value> Query;

	BEFORE_ALL()
	{
		Fixture = MakeUnique<FFlecsTestFixtureRAII>();
		FlecsWorld = Fixture->Fixture.GetFlecsWorld();
		
		TagEntity = FlecsWorld->RegisterComponentType<FFlecsTestStruct_Tag>();
		ValuedComponentEntity = FlecsWorld->RegisterComponentType<FFlecsTestStruct_Value>();

		ToggleableComponentHandle = FlecsWorld->RegisterComponentType<FFlecsTestStruct_Toggleable>();

		TestEntity = FlecsWorld->CreateEntity("TestEntity");
		ToggleableEntityTest = FlecsWorld->CreateEntity("ToggleableEntityTest")
			.Add<FFlecsTestStruct_Toggleable>();

		Query = FlecsWorld->World.query_builder<FFlecsTestStruct_Value>()
			.cached()
			.detect_changes()
			.build();
		
	}

	AFTER_ALL()
	{
		Query.destruct();
		Query = {};
		
		Fixture.Reset();
		FlecsWorld = nullptr;
	}

	TEST_METHOD_WITH_TAGS(A1_BasicTagAddRemove_Add_CPPAPI_Remove_CPPAPI,
	                     "[Flecs][Basic][Tag][USTRUCT][CPP-API][Entity-API]")
	{
		TestEntity.Add<FFlecsTestStruct_Tag>();
		ASSERT_THAT(IsTrue(TestEntity.Has(TagEntity)));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsTrue(TestEntity.Has(FFlecsTestStruct_Tag::StaticStruct())));

		TestEntity.Remove<FFlecsTestStruct_Tag>();
		ASSERT_THAT(IsFalse(TestEntity.Has(TagEntity)));
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestStruct_Tag::StaticStruct())));
	}

	TEST_METHOD_WITH_TAGS(A2_BasicTagAddRemove_Add_StaticStructAPI_Remove_StaticStructAPI,
		"[Flecs][Basic][Tag][USTRUCT][StaticStruct-API][Entity-API]")
	{
		TestEntity.Add(FFlecsTestStruct_Tag::StaticStruct());
		ASSERT_THAT(IsTrue(TestEntity.Has(TagEntity)));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsTrue(TestEntity.Has(FFlecsTestStruct_Tag::StaticStruct())));

		TestEntity.Remove(FFlecsTestStruct_Tag::StaticStruct());
		ASSERT_THAT(IsFalse(TestEntity.Has(TagEntity)));
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestStruct_Tag::StaticStruct())));
	}

	TEST_METHOD_WITH_TAGS(A3_BasicTagAddRemove_Add_StaticStructAPI_Remove_CPPAPI,
	                     "[Flecs][Basic][Tag][USTRUCT][StaticStruct-API][CPP-API][StaticStruct-API]")
	{
		TestEntity.Add(FFlecsTestStruct_Tag::StaticStruct());
		ASSERT_THAT(IsTrue(TestEntity.Has(TagEntity)));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsTrue(TestEntity.Has(FFlecsTestStruct_Tag::StaticStruct())));

		TestEntity.Remove<FFlecsTestStruct_Tag>();
		ASSERT_THAT(IsFalse(TestEntity.Has(TagEntity)));
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestStruct_Tag::StaticStruct())));
	}

	TEST_METHOD_WITH_TAGS(A4_BasicTagAddRemove_Add_CPPAPI_Remove_StaticStructAPI,
		"[Flecs][Basic][Tag][USTRUCT][CPP-API][StaticStruct-API][CPP-API]")
	{
		TestEntity.Add<FFlecsTestStruct_Tag>();
		ASSERT_THAT(IsTrue(TestEntity.Has(TagEntity)));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsTrue(TestEntity.Has(FFlecsTestStruct_Tag::StaticStruct())));

		TestEntity.Remove(FFlecsTestStruct_Tag::StaticStruct());
		ASSERT_THAT(IsFalse(TestEntity.Has(TagEntity)));
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestStruct_Tag::StaticStruct())));
	}

	TEST_METHOD_WITH_TAGS(A5_BasicTagAddRemove_Add_EntityAPI_Remove_StaticStructAPI,
	                     "[Flecs][Basic][Tag][USTRUCT][Entity-API]")
	{
		TestEntity.Add(TagEntity);
		ASSERT_THAT(IsTrue(TestEntity.Has(TagEntity)));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsTrue(TestEntity.Has(FFlecsTestStruct_Tag::StaticStruct())));

		TestEntity.Remove(FFlecsTestStruct_Tag::StaticStruct());
		ASSERT_THAT(IsFalse(TestEntity.Has(TagEntity)));
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestStruct_Tag::StaticStruct())));
	}

	TEST_METHOD_WITH_TAGS(A6_BasicTagAddRemove_Add_EntityAPI_Remove_EntityAPI,
	                     "[Flecs][Basic][Tag][USTRUCT][StaticStruct-API][Entity-API]")
	{
		TestEntity.Add(TagEntity);
		ASSERT_THAT(IsTrue(TestEntity.Has(TagEntity)));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsTrue(TestEntity.Has(FFlecsTestStruct_Tag::StaticStruct())));

		TestEntity.Remove(FFlecsTestStruct_Tag::StaticStruct());
		ASSERT_THAT(IsFalse(TestEntity.Has(TagEntity)));
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Tag>()));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestStruct_Tag::StaticStruct())));
	}

	TEST_METHOD_WITH_TAGS(B1_BasicComponentSetRemove_Set_CPPAPI_Remove_CPPAPI,
	                     "[Flecs][Basic][Component][USTRUCT][CPP-API][Entity-API]")
	{
		TestEntity.Set<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ .Value = 42 });
		ASSERT_THAT(IsTrue(TestEntity.Has(ValuedComponentEntity)));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsTrue(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())));

		const auto& [Value] = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Value == 42));

		const void* ValuePtr = TestEntity.GetPtr(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(ValuePtr != nullptr));
		const TSolidNotNull<const FFlecsTestStruct_Value*> ValuePtrTyped = static_cast<const FFlecsTestStruct_Value*>(ValuePtr);
		ASSERT_THAT(IsTrue(ValuePtrTyped->Value == 42));

		TestEntity.Remove<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsFalse(TestEntity.Has(ValuedComponentEntity)));
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())));
	}

	TEST_METHOD_WITH_TAGS(B2_BasicComponentSetRemove_Set_StaticStructAPI_Remove_StaticStructAPI,
		"[Flecs][Basic][Component][USTRUCT][StaticStruct-API][Entity-API]")
	{
		constexpr FFlecsTestStruct_Value TestValue{ .Value = 42 };
		
		TestEntity.Set(FFlecsTestStruct_Value::StaticStruct(), &TestValue);
		ASSERT_THAT(IsTrue(TestEntity.Has(ValuedComponentEntity)));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsTrue(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())));

		const auto& [Value] = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Value == 42));

		const void* ValuePtr = TestEntity.GetPtr(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(ValuePtr != nullptr));
		
		const TSolidNotNull<const FFlecsTestStruct_Value*> ValuePtrTyped = static_cast<const FFlecsTestStruct_Value*>(ValuePtr);
		ASSERT_THAT(IsTrue(ValuePtrTyped->Value == 42));

		TestEntity.Remove(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsFalse(TestEntity.Has(ValuedComponentEntity)));
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())));
	}

	TEST_METHOD_WITH_TAGS(B3_BasicComponentSetRemove_Set_StaticStructAPI_Remove_CPPAPI,
	                     "[Flecs][Basic][Component][USTRUCT][StaticStruct-API][CPP-API]")
	{
		constexpr FFlecsTestStruct_Value TestValue{ .Value = 42 };
		
		TestEntity.Set(FFlecsTestStruct_Value::StaticStruct(), &TestValue);
		ASSERT_THAT(IsTrue(TestEntity.Has(ValuedComponentEntity)));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsTrue(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())));

		const auto& [Value] = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Value == 42));

		const void* ValuePtr = TestEntity.GetPtr(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(ValuePtr != nullptr));
		
		const TSolidNotNull<const FFlecsTestStruct_Value*> ValuePtrTyped = static_cast<const FFlecsTestStruct_Value*>(ValuePtr);
		ASSERT_THAT(IsTrue(ValuePtrTyped->Value == 42));

		TestEntity.Remove<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsFalse(TestEntity.Has(ValuedComponentEntity)));
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())));
	}

	TEST_METHOD_WITH_TAGS(B4_BasicComponentSetRemove_Set_CPPAPI_Remove_StaticStructAPI,
	                     "[Flecs][Basic][Component][USTRUCT][CPP-API][StaticStruct-API]")
	{
		TestEntity.Set<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ .Value = 42 });
		ASSERT_THAT(IsTrue(TestEntity.Has(ValuedComponentEntity)));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsTrue(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())));

		const auto& [Value] = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Value == 42));

		const void* ValuePtr = TestEntity.GetPtr(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(ValuePtr != nullptr));
		
		const TSolidNotNull<const FFlecsTestStruct_Value*> ValuePtrTyped = static_cast<const FFlecsTestStruct_Value*>(ValuePtr);
		ASSERT_THAT(IsTrue(ValuePtrTyped->Value == 42));

		TestEntity.Remove(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsFalse(TestEntity.Has(ValuedComponentEntity)));
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())));
	}

	TEST_METHOD_WITH_TAGS(B5_BasicComponentSetRemove_Set_EntityAPI_Remove_StaticStructAPI,
	                     "[Flecs][Basic][Component][USTRUCT][Entity-API]")
	{
		static constexpr FFlecsTestStruct_Value TestValue{ .Value = 42 };
		
		TestEntity.Set(ValuedComponentEntity, &TestValue);
		ASSERT_THAT(IsTrue(TestEntity.Has(ValuedComponentEntity)));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsTrue(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())));

		const auto& [Value] = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Value == 42));

		const void* ValuePtr = TestEntity.GetPtr(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(ValuePtr != nullptr));
		
		const TSolidNotNull<const FFlecsTestStruct_Value*> ValuePtrTyped = static_cast<const FFlecsTestStruct_Value*>(ValuePtr);
		ASSERT_THAT(IsTrue(ValuePtrTyped->Value == 42));

		TestEntity.Remove(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsFalse(TestEntity.Has(ValuedComponentEntity)));
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())));
	}

	TEST_METHOD_WITH_TAGS(B6_BasicComponentAddAssignRemove_Add_CPPAPI_Assign_CPPAPI_Remove_CPPAPI,
	                     "[Flecs][Basic][Component][USTRUCT][CPP-API][Entity-API]")
	{
		static constexpr int32 StartingValue = 0;
		
		TestEntity.Add<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(TestEntity.Has(ValuedComponentEntity)));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsTrue(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())));

		const auto& [OldValue] = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(OldValue == StartingValue));
		
		TestEntity.Assign<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ .Value = 42 });

		const auto& [Value] = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Value == 42));

		const void* ValuePtr = TestEntity.GetPtr(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(ValuePtr != nullptr));
		
		const TSolidNotNull<const FFlecsTestStruct_Value*> ValuePtrTyped = static_cast<const FFlecsTestStruct_Value*>(ValuePtr);
		ASSERT_THAT(IsTrue(ValuePtrTyped->Value == 42));

		TestEntity.Remove<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsFalse(TestEntity.Has(ValuedComponentEntity)));
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())));
	}

	TEST_METHOD_WITH_TAGS(B7_BasicComponentAddAssignRemove_Add_StaticStructAPI_Assign_StaticStructAPI_Remove_StaticStructAPI,
		"[Flecs][Basic][Component][USTRUCT][StaticStruct-API][Entity-API]")
	{
		static constexpr int32 StartingValue = 0;

		static constexpr FFlecsTestStruct_Value DefaultValue{ .Value = 42 };
		
		TestEntity.Add(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(TestEntity.Has(ValuedComponentEntity)));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsTrue(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())));

		const auto& [OldValue] = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(OldValue == StartingValue));
		
		TestEntity.Assign(FFlecsTestStruct_Value::StaticStruct(), &DefaultValue);

		const auto& [Value] = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Value == 42));

		const void* ValuePtr = TestEntity.GetPtr(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(ValuePtr != nullptr));
		
		const TSolidNotNull<const FFlecsTestStruct_Value*> ValuePtrTyped = static_cast<const FFlecsTestStruct_Value*>(ValuePtr);
		ASSERT_THAT(IsTrue(ValuePtrTyped->Value == 42));

		TestEntity.Remove(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsFalse(TestEntity.Has(ValuedComponentEntity)));
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())));
	}

	TEST_METHOD_WITH_TAGS(B8_BasicComponentAddAssignRemove_Add_EntityAPI_Assign_EntityAPI_Remove_EntityAPI,
	                     "[Flecs][Basic][Component][USTRUCT][Entity-API]")
	{
		static constexpr int32 StartingValue = 0;

		static constexpr FFlecsTestStruct_Value DefaultValue{ .Value = 42 };
		
		TestEntity.Add(ValuedComponentEntity);
		ASSERT_THAT(IsTrue(TestEntity.Has(ValuedComponentEntity)));
		ASSERT_THAT(IsTrue(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsTrue(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())));

		const auto& [OldValue] = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(OldValue == StartingValue));
		
		TestEntity.Assign(ValuedComponentEntity, &DefaultValue);

		const auto& [Value] = TestEntity.Get<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Value == 42));

		const void* ValuePtr = TestEntity.GetPtr(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(ValuePtr != nullptr));
		
		const TSolidNotNull<const FFlecsTestStruct_Value*> ValuePtrTyped = static_cast<const FFlecsTestStruct_Value*>(ValuePtr);
		ASSERT_THAT(IsTrue(ValuePtrTyped->Value == 42));

		TestEntity.Remove(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsFalse(TestEntity.Has(ValuedComponentEntity)));
		ASSERT_THAT(IsFalse(TestEntity.Has<FFlecsTestStruct_Value>()));
		ASSERT_THAT(IsFalse(TestEntity.Has(FFlecsTestStruct_Value::StaticStruct())));
	}

	TEST_METHOD_WITH_TAGS(C1_BasicComponentModified_Set_CPPAPI,
	                     "[Flecs][Basic][Component][USTRUCT][CPP-API][Entity-API]")
	{
		TestEntity.Set<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ .Value = 42 });

		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 1));

		TestEntity.Remove<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 0));
	}

	TEST_METHOD_WITH_TAGS(C2_BasicComponentModified_Set_StaticStructAPI,
		"[Flecs][Basic][Component][USTRUCT][StaticStruct-API][Entity-API]")
	{
		constexpr FFlecsTestStruct_Value TestValue{ .Value = 42 };
		
		TestEntity.Set(FFlecsTestStruct_Value::StaticStruct(), &TestValue);

		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 1));

		TestEntity.Remove(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 0));
	}

	TEST_METHOD_WITH_TAGS(C3_BasicComponentModified_Set_EntityAPI,
	                     "[Flecs][Basic][Component][USTRUCT][Entity-API]")
	{
		static constexpr FFlecsTestStruct_Value TestValue{ .Value = 42 };
		
		TestEntity.Set(ValuedComponentEntity, &TestValue);

		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 1));

		TestEntity.Remove(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 0));
	}

	TEST_METHOD_WITH_TAGS(C4_BasicComponentModified_Modified_CPPAPI,
	                     "[Flecs][Basic][Component][USTRUCT][CPP-API][Entity-API]")
	{
		TestEntity.Add<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 1));

		ASSERT_THAT(IsFalse(Query.changed()));
		
		TestEntity.Modified<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 1));

		TestEntity.Remove<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 0));
	}

	TEST_METHOD_WITH_TAGS(C5_BasicComponentModified_Modified_StaticStructAPI,
		"[Flecs][Basic][Component][USTRUCT][StaticStruct-API][Entity-API]")
	{
		TestEntity.Add(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 1));

		ASSERT_THAT(IsFalse(Query.changed()));
		
		TestEntity.Modified(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 1));

		TestEntity.Remove(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 0));
	}

	TEST_METHOD_WITH_TAGS(C6_BasicComponentModified_Modified_EntityAPI,
	                     "[Flecs][Basic][Component][USTRUCT][Entity-API]")
	{
		TestEntity.Add(ValuedComponentEntity);
		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 1));

		ASSERT_THAT(IsFalse(Query.changed()));
		
		TestEntity.Modified(ValuedComponentEntity);
		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 1));

		TestEntity.Remove(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 0));
	}

	TEST_METHOD_WITH_TAGS(D1_BasicEnableDisableToggleComponent_Enable_CPPAPI_Disable_CPPAPI_Toggle_CPPAPI,
	                     "[Flecs][Basic][Component][USTRUCT][CPP-API][Entity-API]")
	{
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has<FFlecsTestStruct_Toggleable>()));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has(ToggleableComponentHandle)));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has(FFlecsTestStruct_Toggleable::StaticStruct())));
		
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled(ToggleableComponentHandle)));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled(FFlecsTestStruct_Toggleable::StaticStruct())));

		ToggleableEntityTest.Disable<FFlecsTestStruct_Toggleable>();
		ASSERT_THAT(IsFalse(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Enable<FFlecsTestStruct_Toggleable>();
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Toggle<FFlecsTestStruct_Toggleable>();
		ASSERT_THAT(IsFalse(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Toggle<FFlecsTestStruct_Toggleable>();
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));
	}

	TEST_METHOD_WITH_TAGS(D2_BasicEnableDisableToggleComponent_Enable_StaticStructAPI_Disable_StaticStructAPI_Toggle_StaticStructAPI,
		"[Flecs][Basic][Component][USTRUCT][StaticStruct-API][Entity-API]")
	{
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has<FFlecsTestStruct_Toggleable>()));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has(ToggleableComponentHandle)));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has(FFlecsTestStruct_Toggleable::StaticStruct())));
		
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled(ToggleableComponentHandle)));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled(FFlecsTestStruct_Toggleable::StaticStruct())));

		ToggleableEntityTest.Disable(FFlecsTestStruct_Toggleable::StaticStruct());
		ASSERT_THAT(IsFalse(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Enable(FFlecsTestStruct_Toggleable::StaticStruct());
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Toggle(FFlecsTestStruct_Toggleable::StaticStruct());
		ASSERT_THAT(IsFalse(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Toggle(FFlecsTestStruct_Toggleable::StaticStruct());
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));
	}

	TEST_METHOD_WITH_TAGS(D3_BasicEnableDisableToggleComponent_Enable_EntityAPI_Disable_EntityAPI_Toggle_EntityAPI,
	                     "[Flecs][Basic][Component][USTRUCT][Entity-API]")
	{
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has<FFlecsTestStruct_Toggleable>()));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has(ToggleableComponentHandle)));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has(FFlecsTestStruct_Toggleable::StaticStruct())));
		
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled(ToggleableComponentHandle)));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled(FFlecsTestStruct_Toggleable::StaticStruct())));

		ToggleableEntityTest.Disable(ToggleableComponentHandle);
		ASSERT_THAT(IsFalse(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Enable(ToggleableComponentHandle);
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Toggle(ToggleableComponentHandle);
		ASSERT_THAT(IsFalse(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Toggle(ToggleableComponentHandle);
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));
	}

	TEST_METHOD_WITH_TAGS(D4_BasicEnableDisableToggleComponent_Enable_CPPAPI_Disable_StaticStructAPI_Toggle_CPPAPI,
	                     "[Flecs][Basic][Component][USTRUCT][CPP-API][StaticStruct-API]")
	{
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has<FFlecsTestStruct_Toggleable>()));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has(ToggleableComponentHandle)));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has(FFlecsTestStruct_Toggleable::StaticStruct())));
		
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled(ToggleableComponentHandle)));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled(FFlecsTestStruct_Toggleable::StaticStruct())));

		ToggleableEntityTest.Disable<FFlecsTestStruct_Toggleable>();
		ASSERT_THAT(IsFalse(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Enable(FFlecsTestStruct_Toggleable::StaticStruct());
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Toggle<FFlecsTestStruct_Toggleable>();
		ASSERT_THAT(IsFalse(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Toggle<FFlecsTestStruct_Toggleable>();
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));
	}

	TEST_METHOD_WITH_TAGS(D5_BasicEnableDisableToggleComponent_Enable_StaticStructAPI_Disable_CPPAPI_Toggle_StaticStructAPI,
		"[Flecs][Basic][Component][USTRUCT][StaticStruct-API][CPP-API]")
	{
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has<FFlecsTestStruct_Toggleable>()));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has(ToggleableComponentHandle)));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has(FFlecsTestStruct_Toggleable::StaticStruct())));
		
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled(ToggleableComponentHandle)));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled(FFlecsTestStruct_Toggleable::StaticStruct())));

		ToggleableEntityTest.Disable(FFlecsTestStruct_Toggleable::StaticStruct());
		ASSERT_THAT(IsFalse(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Enable<FFlecsTestStruct_Toggleable>();
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Toggle(FFlecsTestStruct_Toggleable::StaticStruct());
		ASSERT_THAT(IsFalse(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Toggle(FFlecsTestStruct_Toggleable::StaticStruct());
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));
	}

	TEST_METHOD_WITH_TAGS(D6_BasicEnableDisableToggleComponent_Enable_EntityAPI_Disable_CPPAPI_Toggle_EntityAPI,
	                     "[Flecs][Basic][Component][USTRUCT][Entity-API]")
	{
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has<FFlecsTestStruct_Toggleable>()));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has(ToggleableComponentHandle)));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.Has(FFlecsTestStruct_Toggleable::StaticStruct())));
		
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled(ToggleableComponentHandle)));
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled(FFlecsTestStruct_Toggleable::StaticStruct())));

		ToggleableEntityTest.Disable(ToggleableComponentHandle);
		ASSERT_THAT(IsFalse(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Enable(FFlecsTestStruct_Toggleable::StaticStruct());
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Toggle(ToggleableComponentHandle);
		ASSERT_THAT(IsFalse(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));

		ToggleableEntityTest.Toggle(ToggleableComponentHandle);
		ASSERT_THAT(IsTrue(ToggleableEntityTest.IsEnabled<FFlecsTestStruct_Toggleable>()));
	}

}; // End of A3_UnrealFlecsBasicComponentTests

TEST_CLASS_WITH_FLAGS_AND_TAGS(A4_UnrealFlecsBasicPairTests,
							   "UnrealFlecs.Basic.Pairs",
							   EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter
							   | EAutomationTestFlags::CriticalPriority,
							   "[Flecs][Basic][Pair]")
{
	inline static TUniquePtr<FFlecsTestFixtureRAII> Fixture;
	inline static TObjectPtr<UFlecsWorld> FlecsWorld = nullptr;

	inline static FFlecsEntityHandle TestEntity;

	BEFORE_ALL()
	{
		Fixture = MakeUnique<FFlecsTestFixtureRAII>();
		FlecsWorld = Fixture->Fixture.GetFlecsWorld();

		TestEntity = FlecsWorld->CreateEntity("TestEntity");
	}

	AFTER_ALL()
	{
		FlecsWorld = nullptr;
		Fixture.Reset();
	}
	
}; // End of A4_UnrealFlecsBasicPairTests

#endif // #if WITH_AUTOMATION_TESTS