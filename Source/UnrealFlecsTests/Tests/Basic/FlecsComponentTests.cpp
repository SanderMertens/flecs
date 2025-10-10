// Elie Wiese-Namir © 2025. All Rights Reserved.

#pragma once

#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

#include "Worlds/FlecsWorld.h"
#include "Translators/FlecsTranslationSettingTraits.h"

#include "Tests/FlecsTestTypes.h"

/**
 * Layout of the tests:
 * A. Basic Component Registration Tests
 * B. Component Properties/Trait Registration Tests
 * C. Enum Component Registration Tests
 * D. Edge Case Tests
 **/
TEST_CLASS_WITH_FLAGS_AND_TAGS(A2_UnrealFlecsComponentRegistrationTests,
							   "UnrealFlecs.A2_Components",
							   EAutomationTestFlags::EditorContext | EAutomationTestFlags::ProductFilter
								| EAutomationTestFlags::CriticalPriority,
							   "[Flecs][Entity][Tag][Component][Registration]")
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
		FlecsWorld = nullptr;
	}

	AFTER_ALL()
	{
		Fixture.Reset();
	}

	TEST_METHOD(A1_BasicUSTRUCTComponentRegistration_CPPAPI)
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

	TEST_METHOD(A2_BasicUSTRUCTComponentRegistration_StaticStructAPI)
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

	TEST_METHOD(A3_BasicComponentRegistration_CPPAPI)
	{
		const FFlecsEntityHandle StructEntity = FlecsWorld->RegisterComponentType<FFlecsTest_CPPStructValue>();
		ASSERT_THAT(IsTrue(StructEntity.IsValid()));

		ASSERT_THAT(IsTrue(StructEntity.IsComponent()));
		ASSERT_THAT(IsFalse(StructEntity.IsTag()));
	}

	TEST_METHOD(B1_GetComponentPropertyTraits_CPPAPI)
	{
		const FFlecsEntityHandle StructEntity = FlecsWorld->RegisterComponentType<FFlecsTestStruct_WithPropertyTraits>();
		ASSERT_THAT(IsTrue(StructEntity.IsValid()));

		ASSERT_THAT(IsTrue(StructEntity.Has(flecs::Trait)));
	}

	TEST_METHOD(B2_GetComponentPropertyTraits_StaticStructAPI)
	{
		const FFlecsEntityHandle StaticStructEntity = FlecsWorld->RegisterComponentType(FFlecsTestStruct_WithPropertyTraits::StaticStruct());
		ASSERT_THAT(IsTrue(StaticStructEntity.IsValid()));

		ASSERT_THAT(IsTrue(StaticStructEntity.Has(flecs::Trait)));
	}

	TEST_METHOD(B3_GetComponentPropertyTraitsFromAnotherModule_CPPAPI)
	{
		const FFlecsEntityHandle StructEntity = FlecsWorld->RegisterComponentType<FFlecsTranslationPropertyTrait>();
		ASSERT_THAT(IsTrue(StructEntity.IsValid()));

		ASSERT_THAT(IsTrue(StructEntity.Has(flecs::PairIsTag)));
	}

	TEST_METHOD(B4_GetComponentPropertyTraitsFromAnotherModule_StaticStructAPI)
	{
		const FFlecsEntityHandle StaticStructEntity = FlecsWorld->RegisterComponentType(FFlecsTranslationPropertyTrait::StaticStruct());
		ASSERT_THAT(IsTrue(StaticStructEntity.IsValid()));

		ASSERT_THAT(IsTrue(StaticStructEntity.Has(flecs::PairIsTag)));
	}

	TEST_METHOD(C1_EnumComponentRegistration_CPPAPI)
	{
		const FFlecsEntityHandle EnumEntity = FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(IsTrue(EnumEntity.IsValid()));

		ASSERT_THAT(IsTrue(EnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(EnumEntity.IsEnum()));

		const FFlecsEntityHandle StaticEnumEntity = FlecsWorld->RegisterComponentType(StaticEnum<EFlecsTestEnum_UENUM>());
		ASSERT_THAT(IsTrue(StaticEnumEntity.IsValid()));
		ASSERT_THAT(IsTrue(StaticEnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(StaticEnumEntity.IsEnum()));

		ASSERT_THAT(IsTrue(EnumEntity == StaticEnumEntity));
	}

	TEST_METHOD(C2_EnumComponentRegistration_StaticStructAPI)
	{
		const FFlecsEntityHandle StaticEnumEntity = FlecsWorld->RegisterComponentType(StaticEnum<EFlecsTestEnum_UENUM>());
		ASSERT_THAT(IsTrue(StaticEnumEntity.IsValid()));
		
		ASSERT_THAT(IsTrue(StaticEnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(StaticEnumEntity.IsEnum()));

		const FFlecsEntityHandle EnumEntity = FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(IsTrue(EnumEntity.IsValid()));

		ASSERT_THAT(IsTrue(EnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(EnumEntity.IsEnum()));

		ASSERT_THAT(IsTrue(StaticEnumEntity == EnumEntity));
	}

	TEST_METHOD(C3_SparseEnumComponentRegistration_CPPAPI)
	{
		const FFlecsEntityHandle SparseEnumEntity = FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(IsTrue(SparseEnumEntity.IsValid()));

		ASSERT_THAT(IsTrue(SparseEnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(SparseEnumEntity.IsEnum()));

		const FFlecsEntityHandle StaticSparseEnumEntity = FlecsWorld->RegisterComponentType(StaticEnum<EFlecsTestEnum_UENUM>());
		ASSERT_THAT(IsTrue(StaticSparseEnumEntity.IsValid()));
		
		ASSERT_THAT(IsTrue(StaticSparseEnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(StaticSparseEnumEntity.IsEnum()));

		ASSERT_THAT(IsTrue(SparseEnumEntity == StaticSparseEnumEntity));
	}

	TEST_METHOD(C4_SparseEnumComponentRegistration_StaticStructAPI)
	{
		const FFlecsEntityHandle StaticSparseEnumEntity = FlecsWorld->RegisterComponentType(StaticEnum<EFlecsTestEnum_UENUM>());
		ASSERT_THAT(IsTrue(StaticSparseEnumEntity.IsValid()));

		ASSERT_THAT(IsTrue(StaticSparseEnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(StaticSparseEnumEntity.IsEnum()));

		const FFlecsEntityHandle SparseEnumEntity = FlecsWorld->RegisterComponentType<EFlecsTestEnum_UENUM>();
		ASSERT_THAT(IsTrue(SparseEnumEntity.IsValid()));

		ASSERT_THAT(IsTrue(SparseEnumEntity.IsComponent()));
		ASSERT_THAT(IsTrue(SparseEnumEntity.IsEnum()));

		ASSERT_THAT(IsTrue(StaticSparseEnumEntity == SparseEnumEntity));
	}

	TEST_METHOD(D1_RegisterScriptStructOneByteOneUProperty_CPPAPI)
	{
		const FFlecsEntityHandle OneByteOneUPropertyEntity = FlecsWorld->RegisterComponentType<FUStructTestComponent_NonTagUSTRUCT>();
		ASSERT_THAT(IsTrue(OneByteOneUPropertyEntity.IsValid()));

		ASSERT_THAT(IsTrue(OneByteOneUPropertyEntity.IsComponent()));
		ASSERT_THAT(IsFalse(OneByteOneUPropertyEntity.IsTag()));
	}

	TEST_METHOD(D2_RegisterScriptStructOneByteOneUProperty_StaticStructAPI)
	{
		const FFlecsEntityHandle OneByteOneUPropertyEntity = FlecsWorld->RegisterComponentType(FUStructTestComponent_NonTagUSTRUCT::StaticStruct());
		ASSERT_THAT(IsTrue(OneByteOneUPropertyEntity.IsValid()));

		ASSERT_THAT(IsTrue(OneByteOneUPropertyEntity.IsComponent()));
		ASSERT_THAT(IsFalse(OneByteOneUPropertyEntity.IsTag()));
	}

	TEST_METHOD(D3_RegisterScriptStructOneByteWithoutUProperty_CPPAPI)
	{
		const FFlecsEntityHandle OneByteWithoutUPropertyEntity = FlecsWorld->RegisterComponentType<FUSTructTestComponent_AccidentalTag>();
		ASSERT_THAT(IsTrue(OneByteWithoutUPropertyEntity.IsValid()));

		ASSERT_THAT(IsTrue(OneByteWithoutUPropertyEntity.IsComponent()));
		ASSERT_THAT(IsFalse(OneByteWithoutUPropertyEntity.IsTag()));
	}

	TEST_METHOD(D4_RegisterScriptStructOneByteWithoutUProperty_StaticStructAPI)
	{
		const FFlecsEntityHandle OneByteWithoutUPropertyEntity = FlecsWorld->RegisterComponentType(FUSTructTestComponent_AccidentalTag::StaticStruct());
		ASSERT_THAT(IsTrue(OneByteWithoutUPropertyEntity.IsValid()));

		ASSERT_THAT(IsTrue(OneByteWithoutUPropertyEntity.IsComponent()));
		// @TODO: Can we unify this?
		ASSERT_THAT(IsTrue(OneByteWithoutUPropertyEntity.IsTag()));
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
                               "UnrealFlecs.A3_Components",
                               EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter,
                               "[Flecs][Entity][Tag][Component]")
{
	inline static TUniquePtr<FFlecsTestFixtureRAII> Fixture;
	inline static TObjectPtr<UFlecsWorld> FlecsWorld = nullptr;

	inline static FFlecsEntityHandle TagEntity;
	inline static FFlecsEntityHandle ValuedComponentEntity;
	inline static FFlecsComponentHandle ToggleableComponentHandle;

	inline static FFlecsEntityHandle TestEntity;
	inline static FFlecsEntityHandle ToggleableEntityTest;

	inline static flecs::query<FFlecsTestStruct_Value> Query;

	BEFORE_EACH()
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

	AFTER_EACH()
	{
		Query.destruct();
		Query = {};
		
		Fixture.Reset();
		FlecsWorld = nullptr;
	}

	TEST_METHOD(A1_BasicTagAddRemove_Add_CPPAPI_Remove_CPPAPI)
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

	TEST_METHOD(A2_BasicTagAddRemove_Add_StaticStructAPI_Remove_StaticStructAPI)
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

	TEST_METHOD(A3_BasicTagAddRemove_Add_StaticStructAPI_Remove_CPPAPI)
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

	TEST_METHOD(A4_BasicTagAddRemove_Add_CPPAPI_Remove_StaticStructAPI)
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

	TEST_METHOD(A5_BasicTagAddRemove_Add_EntityAPI_Remove_StaticStructAPI)
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

	TEST_METHOD(A6_BasicTagAddRemove_Add_EntityAPI_Remove_EntityAPI)
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

	TEST_METHOD(B1_BasicComponentSetRemove_Set_CPPAPI_Remove_CPPAPI)
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

	TEST_METHOD(B2_BasicComponentSetRemove_Set_StaticStructAPI_Remove_StaticStructAPI)
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

	TEST_METHOD(B3_BasicComponentSetRemove_Set_StaticStructAPI_Remove_CPPAPI)
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

	TEST_METHOD(B4_BasicComponentSetRemove_Set_CPPAPI_Remove_StaticStructAPI)
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

	TEST_METHOD(B5_BasicComponentSetRemove_Set_EntityAPI_Remove_StaticStructAPI)
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

	TEST_METHOD(B6_BasicComponentAddAssignRemove_Add_CPPAPI_Assign_CPPAPI_Remove_CPPAPI)
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

	TEST_METHOD(B7_BasicComponentAddAssignRemove_Add_StaticStructAPI_Assign_StaticStructAPI_Remove_StaticStructAPI)
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

	TEST_METHOD(B8_BasicComponentAddAssignRemove_Add_EntityAPI_Assign_EntityAPI_Remove_EntityAPI)
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

	TEST_METHOD(C1_BasicComponentModified_Set_CPPAPI)
	{
		TestEntity.Set<FFlecsTestStruct_Value>(FFlecsTestStruct_Value{ .Value = 42 });

		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 1));

		TestEntity.Remove<FFlecsTestStruct_Value>();
		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 0));
	}

	TEST_METHOD(C2_BasicComponentModified_Set_StaticStructAPI)
	{
		constexpr FFlecsTestStruct_Value TestValue{ .Value = 42 };
		
		TestEntity.Set(FFlecsTestStruct_Value::StaticStruct(), &TestValue);

		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 1));

		TestEntity.Remove(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 0));
	}

	TEST_METHOD(C3_BasicComponentModified_Set_EntityAPI)
	{
		static constexpr FFlecsTestStruct_Value TestValue{ .Value = 42 };
		
		TestEntity.Set(ValuedComponentEntity, &TestValue);

		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 1));

		TestEntity.Remove(FFlecsTestStruct_Value::StaticStruct());
		ASSERT_THAT(IsTrue(Query.changed()));
		ASSERT_THAT(IsTrue(Query.count() == 0));
	}

	TEST_METHOD(C4_BasicComponentModified_Modified_CPPAPI)
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

	TEST_METHOD(C5_BasicComponentModified_Modified_StaticStructAPI)
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

	TEST_METHOD(C6_BasicComponentModified_Modified_EntityAPI)
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

	TEST_METHOD(D1_BasicEnableDisableToggleComponent_Enable_CPPAPI_Disable_CPPAPI_Toggle_CPPAPI)
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

	TEST_METHOD(D2_BasicEnableDisableToggleComponent_Enable_StaticStructAPI_Disable_StaticStructAPI_Toggle_StaticStructAPI)
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

	TEST_METHOD(D3_BasicEnableDisableToggleComponent_Enable_EntityAPI_Disable_EntityAPI_Toggle_EntityAPI)
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

	TEST_METHOD(D4_BasicEnableDisableToggleComponent_Enable_CPPAPI_Disable_StaticStructAPI_Toggle_CPPAPI)
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

	TEST_METHOD(D5_BasicEnableDisableToggleComponent_Enable_StaticStructAPI_Disable_CPPAPI_Toggle_StaticStructAPI)
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

	TEST_METHOD(D6_BasicEnableDisableToggleComponent_Enable_EntityAPI_Disable_CPPAPI_Toggle_EntityAPI)
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


/*
 * Layout of the tests:
 * A. Basic Pair Tests
 * B. Variation API Tests
 * C. Set/Assign API Tests
 */
TEST_CLASS_WITH_FLAGS_AND_TAGS(A4_UnrealFlecsBasicPairTests,
							   "UnrealFlecs.A4_Pairs",
							   EAutomationTestFlags::EditorContext | EAutomationTestFlags::ClientContext | EAutomationTestFlags::ProductFilter
							   | EAutomationTestFlags::CriticalPriority,
							   "[Flecs][Pair][CPP-API][StaticStruct-API][Entity-API]")
{
	inline static TUniquePtr<FFlecsTestFixtureRAII> Fixture;
	inline static TObjectPtr<UFlecsWorld> FlecsWorld = nullptr;

	inline static FFlecsEntityHandle TestEntity;

	inline static FFlecsEntityHandle PairEntity1;
	inline static FFlecsEntityHandle PairEntity2;

	inline static FFlecsComponentHandle PairIsTagComponentHandle;

	inline static FFlecsComponentHandle PairTestComponentHandle1;
	inline static FFlecsComponentHandle PairTestComponentHandle2;

	BEFORE_EACH()
	{
		Fixture = MakeUnique<FFlecsTestFixtureRAII>();
		FlecsWorld = Fixture->Fixture.GetFlecsWorld();

		TestEntity = FlecsWorld->CreateEntity("TestEntity");

		PairEntity1 = FlecsWorld->CreateEntity("PairEntity1");
		PairEntity2 = FlecsWorld->CreateEntity("PairEntity2");

		PairIsTagComponentHandle = FlecsWorld->RegisterComponentType<FFlecsTestStruct_PairIsTag>();

		PairTestComponentHandle1 = FlecsWorld->RegisterComponentType<FUSTRUCTPairTestComponent>();
		PairTestComponentHandle2 = FlecsWorld->RegisterComponentType<FUSTRUCTPairTestComponent_Second>();
	}

	AFTER_EACH()
	{
		FlecsWorld = nullptr;
		Fixture.Reset();
	}

	TEST_METHOD(A1_BasicPairAddRemove_Add_EntityAPI_Remove_EntityAPI)
	{
		TestEntity.AddPair(PairEntity1, PairEntity2);
		ASSERT_THAT(IsTrue(TestEntity.HasPair(PairEntity1, PairEntity2)));

		TestEntity.RemovePair(PairEntity1, PairEntity2);
		ASSERT_THAT(IsFalse(TestEntity.HasPair(PairEntity1, PairEntity2)));
	}

	TEST_METHOD(A2_BasicPairAddRemove_Add_CPPAPI_Remove_CPPAPI)
	{
		TestEntity.AddPair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Second>();
		ASSERT_THAT(IsTrue(TestEntity.HasPair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Second>()));

		TestEntity.RemovePair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Second>();
		ASSERT_THAT(IsFalse(TestEntity.HasPair<FUSTRUCTPairTestComponent, FUSTRUCTPairTestComponent_Second>()));
	}

	TEST_METHOD(A3_BasicPairAddRemove_Add_StaticStructAPI_Remove_StaticStructAPI)
	{
		TestEntity.AddPair(FUSTRUCTPairTestComponent::StaticStruct(), FUSTRUCTPairTestComponent_Second::StaticStruct());
		ASSERT_THAT(IsTrue(TestEntity.HasPair(FUSTRUCTPairTestComponent::StaticStruct(), FUSTRUCTPairTestComponent_Second::StaticStruct())));

		TestEntity.RemovePair(FUSTRUCTPairTestComponent::StaticStruct(), FUSTRUCTPairTestComponent_Second::StaticStruct());
		ASSERT_THAT(IsFalse(TestEntity.HasPair(FUSTRUCTPairTestComponent::StaticStruct(), FUSTRUCTPairTestComponent_Second::StaticStruct())));
	}

	TEST_METHOD(A4_BasicPairAddRemove_Add_CPPAPIEntityAPI_Remove_CPPAPIEntityAPI)
	{
		TestEntity.AddPair<FUSTRUCTPairTestComponent>(PairTestComponentHandle2);
		ASSERT_THAT(IsTrue(TestEntity.HasPair<FUSTRUCTPairTestComponent>(PairTestComponentHandle2)));
		
		TestEntity.RemovePair<FUSTRUCTPairTestComponent>(PairTestComponentHandle2);
		ASSERT_THAT(IsFalse(TestEntity.HasPair<FUSTRUCTPairTestComponent>(PairTestComponentHandle2)));
	}

	TEST_METHOD(A5_BasicPairAddRemove_Add_CPPAPIStaticStructAPI_Remove_CPPAPIEntityAPI)
	{
		TestEntity.AddPair<FUSTRUCTPairTestComponent>(FUSTRUCTPairTestComponent::StaticStruct());
		ASSERT_THAT(IsTrue(TestEntity.HasPair<FUSTRUCTPairTestComponent>(FUSTRUCTPairTestComponent::StaticStruct())));

		TestEntity.RemovePair<FUSTRUCTPairTestComponent>(FUSTRUCTPairTestComponent::StaticStruct());
		ASSERT_THAT(IsFalse(TestEntity.HasPair<FUSTRUCTPairTestComponent>(FUSTRUCTPairTestComponent::StaticStruct())));
	}

	TEST_METHOD(A6_BasicPairAddRemove_Add_StaticStructAPIEntityAPI_Remove_StaticStructAPIEntityAPI)
	{
		TestEntity.AddPair(FUSTRUCTPairTestComponent::StaticStruct(), PairTestComponentHandle2);
		ASSERT_THAT(IsTrue(TestEntity.HasPair(FUSTRUCTPairTestComponent::StaticStruct(), PairTestComponentHandle2)));

		TestEntity.RemovePair(FUSTRUCTPairTestComponent::StaticStruct(), PairTestComponentHandle2);
		ASSERT_THAT(IsFalse(TestEntity.HasPair(FUSTRUCTPairTestComponent::StaticStruct(), PairTestComponentHandle2)));
	}

	TEST_METHOD(A7_BasicPairAddSecondRemove_Add_CPPAPI_Remove_EntityAPI_SecondAPI)
	{
		TestEntity.AddPairSecond<FUSTRUCTPairTestComponent_Second>(PairTestComponentHandle1);
		ASSERT_THAT(IsTrue(TestEntity.HasPairSecond<FUSTRUCTPairTestComponent_Second>(PairTestComponentHandle1)));

		TestEntity.RemovePairSecond<FUSTRUCTPairTestComponent_Second>(PairTestComponentHandle1);
		ASSERT_THAT(IsFalse(TestEntity.HasPairSecond<FUSTRUCTPairTestComponent_Second>(PairTestComponentHandle1)));
	}

	TEST_METHOD(A8_BasicPairAddSecondRemove_Add_CPPAPI_Remove_StaticStructAPI_SecondAPI)
	{
		TestEntity.AddPairSecond<FUSTRUCTPairTestComponent_Second>(FUSTRUCTPairTestComponent::StaticStruct());
		ASSERT_THAT(IsTrue(TestEntity.HasPairSecond<FUSTRUCTPairTestComponent_Second>(FUSTRUCTPairTestComponent::StaticStruct())));

		TestEntity.RemovePairSecond<FUSTRUCTPairTestComponent_Second>(FUSTRUCTPairTestComponent::StaticStruct());
		ASSERT_THAT(IsFalse(TestEntity.HasPairSecond<FUSTRUCTPairTestComponent_Second>(FUSTRUCTPairTestComponent::StaticStruct())));
	}

	TEST_METHOD(B1_BasicPairAddRemove_Add_StaticStructAPIEntityAPI_Remove_CPPAPIEntityAPI)
	{
		TestEntity.AddPair(FUSTRUCTPairTestComponent::StaticStruct(), PairTestComponentHandle2);
		ASSERT_THAT(IsTrue(TestEntity.HasPair(FUSTRUCTPairTestComponent::StaticStruct(), PairTestComponentHandle2)));

		TestEntity.RemovePair<FUSTRUCTPairTestComponent>(PairTestComponentHandle2);
		ASSERT_THAT(IsFalse(TestEntity.HasPair(FUSTRUCTPairTestComponent::StaticStruct(), PairTestComponentHandle2)));
	}

	TEST_METHOD(B2_BasicPairAddRemove_Add_CPPAPIStaticStructAPI_Remove_CPPAPIEntityAPI)
	{
		TestEntity.AddPair<FUSTRUCTPairTestComponent>(FUSTRUCTPairTestComponent::StaticStruct());
		ASSERT_THAT(IsTrue(TestEntity.HasPair<FUSTRUCTPairTestComponent>(FUSTRUCTPairTestComponent::StaticStruct())));

		TestEntity.RemovePair<FUSTRUCTPairTestComponent>(FUSTRUCTPairTestComponent::StaticStruct());
		ASSERT_THAT(IsFalse(TestEntity.HasPair<FUSTRUCTPairTestComponent>(FUSTRUCTPairTestComponent::StaticStruct())));
	}

}; // End of A4_UnrealFlecsBasicPairTests

#endif // #if WITH_AUTOMATION_TESTS