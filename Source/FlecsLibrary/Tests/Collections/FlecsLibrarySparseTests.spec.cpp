#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "flecs.h"

// Define the Spec using template variables for consistency
BEGIN_DEFINE_SPEC(FFlecsLibrarySparseTestsSpec,
                  "Flecs.Library.Collections.Sparse",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

	void populate(ecs_sparse_t *sp, int count) {
		int prev_count = flecs_sparse_count(sp);
		for (int i = 0; i < count; i ++) {
			int *elem = flecs_sparse_add_t(sp, int);
			TestNotNull("Sparse element should not be null", elem);
			*elem = i;
		}
		
		TestEqual("Sparse count should be correct", flecs_sparse_count(sp), count + prev_count);
	}

END_DEFINE_SPEC(FFlecsLibrarySparseTestsSpec);

namespace
{
	ecs_sparse_t* _flecs_sparse_new(
	struct ecs_allocator_t *allocator,
	struct ecs_block_allocator_t *page_allocator,
	ecs_size_t elem_size)
	{
		ecs_sparse_t *sp = ecs_os_calloc_t(ecs_sparse_t);
		flecs_sparse_init(sp, allocator, page_allocator, elem_size);
		return sp;
	}

	#define flecs_sparse_new(a, p, T) _flecs_sparse_new(a, p, ECS_SIZEOF(T))

	void flecs_sparse_free(
		ecs_sparse_t *sp)
	{
		flecs_sparse_fini(sp);
		ecs_os_free(sp);
	}
}

void FFlecsLibrarySparseTestsSpec::Define()
{
	/*void Sparse_add_1(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    int* elem = flecs_sparse_add_t(sp, int);
    test_assert(elem != NULL);
    test_int(flecs_sparse_count(sp), 1);
    
    int *ptr = flecs_sparse_get_dense_t(sp, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem == ptr);

    flecs_sparse_free(sp);
}

void Sparse_add_1_to_empty(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    int* elem = flecs_sparse_add_t(sp, int);
    test_assert(elem != NULL);
    test_int(flecs_sparse_count(sp), 1);
    
    int *ptr = flecs_sparse_get_dense_t(sp, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem == ptr);

    flecs_sparse_free(sp);
}

void Sparse_add_1_chunk_size_1(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    int* elem = flecs_sparse_add_t(sp, int);
    test_assert(elem != NULL);
    test_int(flecs_sparse_count(sp), 1);
    
    int *ptr = flecs_sparse_get_dense_t(sp, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem == ptr);

    flecs_sparse_free(sp);
}

void Sparse_add_n(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    int* elem1 = flecs_sparse_add_t(sp, int);
    test_assert(elem1 != NULL);
    test_int(flecs_sparse_count(sp), 1);

    int* elem2 = flecs_sparse_add_t(sp, int);
    test_assert(elem2 != NULL);
    test_int(flecs_sparse_count(sp), 2);    
    
    int *ptr = flecs_sparse_get_dense_t(sp, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem1 == ptr);

    ptr = flecs_sparse_get_dense_t(sp, int, 1);
    test_assert(ptr != NULL);
    test_assert(elem2 == ptr);

    flecs_sparse_free(sp);
}

void Sparse_add_n_chunk_size_1(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    int* elem1 = flecs_sparse_add_t(sp, int);
    test_assert(elem1 != NULL);
    test_int(flecs_sparse_count(sp), 1);

    int* elem2 = flecs_sparse_add_t(sp, int);
    test_assert(elem2 != NULL);
    test_int(flecs_sparse_count(sp), 2);    
    
    int *ptr = flecs_sparse_get_dense_t(sp, int, 0);
    test_assert(ptr != NULL);
    test_assert(elem1 == ptr);

    ptr = flecs_sparse_get_dense_t(sp, int, 1);
    test_assert(ptr != NULL);
    test_assert(elem2 == ptr);

    flecs_sparse_free(sp);
}

void Sparse_remove(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 3);
    
    const uint64_t *indices = flecs_sparse_ids(sp);
    uint64_t i0 = indices[0];
    uint64_t i1 = indices[2];

    flecs_sparse_remove_t(sp, int, 1);
    test_int(flecs_sparse_count(sp), 2);

    test_assert(flecs_sparse_get_dense_t(sp, int, 0) == flecs_sparse_get_t(sp, int, i0));
    test_assert(flecs_sparse_get_dense_t(sp, int, 1) == flecs_sparse_get_t(sp, int, i1));

    flecs_sparse_free(sp);
}

void Sparse_remove_first(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 3);

    const uint64_t *indices = flecs_sparse_ids(sp);
    uint64_t i0 = indices[1];
    uint64_t i1 = indices[2];

    flecs_sparse_remove_t(sp, int, 0);
    test_int(flecs_sparse_count(sp), 2);

    test_assert(flecs_sparse_get_dense_t(sp, int, 0) == flecs_sparse_get_t(sp, int, i1));
    test_assert(flecs_sparse_get_dense_t(sp, int, 1) == flecs_sparse_get_t(sp, int, i0));

    flecs_sparse_free(sp);
}

void Sparse_remove_last(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 3);
    
    const uint64_t *indices = flecs_sparse_ids(sp);
    uint64_t i0 = indices[0];
    uint64_t i1 = indices[1];

    flecs_sparse_remove_t(sp, int, 2);
    test_int(flecs_sparse_count(sp), 2);

    test_assert(flecs_sparse_get_dense_t(sp, int, 0) == flecs_sparse_get_t(sp, int, i0));
    test_assert(flecs_sparse_get_dense_t(sp, int, 1) == flecs_sparse_get_t(sp, int, i1));

    flecs_sparse_free(sp);
}

void Sparse_remove_all(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 3);

    void *elem2 = flecs_sparse_get_dense_t(sp, int, 1);
    void *elem3 = flecs_sparse_get_dense_t(sp, int, 2);

    flecs_sparse_remove_t(sp, int, 0);
    test_int(flecs_sparse_count(sp), 2);
    test_assert(flecs_sparse_try_t(sp, int, 0) == NULL);
    test_assert(flecs_sparse_try_t(sp, int, 1) == elem2);
    test_assert(flecs_sparse_try_t(sp, int, 2) == elem3);

    flecs_sparse_remove_t(sp, int, 1);
    test_int(flecs_sparse_count(sp), 1);
    test_assert(flecs_sparse_try_t(sp, int, 0) == NULL);
    test_assert(flecs_sparse_try_t(sp, int, 1) == NULL);
    test_assert(flecs_sparse_try_t(sp, int, 2) == elem3);    

    flecs_sparse_remove_t(sp, int, 2);
    test_int(flecs_sparse_count(sp), 0);
    test_assert(flecs_sparse_try_t(sp, int, 0) == NULL);
    test_assert(flecs_sparse_try_t(sp, int, 1) == NULL);
    test_assert(flecs_sparse_try_t(sp, int, 2) == NULL);    

    flecs_sparse_free(sp);
}

void Sparse_remove_all_n_chunks(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 128);

    int i;
    for (i = 0; i < 128; i ++) {
        flecs_sparse_remove_t(sp, int, i);
        test_int(flecs_sparse_count(sp), 127 - i);
    }

    flecs_sparse_free(sp);
}

void Sparse_clear_1(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 1);

    flecs_sparse_clear(sp);
    test_int(flecs_sparse_count(sp), 0);

    flecs_sparse_free(sp);
}

void Sparse_clear_empty(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    flecs_sparse_clear(sp);
    test_int(flecs_sparse_count(sp), 0);

    flecs_sparse_free(sp);
}

void Sparse_clear_n(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 3);

    flecs_sparse_clear(sp);
    test_int(flecs_sparse_count(sp), 0);

    flecs_sparse_free(sp);
}

void Sparse_clear_n_chunks(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 128);

    flecs_sparse_clear(sp);
    test_int(flecs_sparse_count(sp), 0);

    flecs_sparse_free(sp);
}

void Sparse_add_after_clear(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    populate(sp, 1);

    flecs_sparse_clear(sp);
    test_int(flecs_sparse_count(sp), 0);

    int* elem = flecs_sparse_add_t(sp, int);
    test_assert(elem != NULL);
    *elem = 10;

    const uint64_t *indices = flecs_sparse_ids(sp);
    test_assert(indices != NULL);

    int *ptr = flecs_sparse_get_t(sp, int, indices[0]);
    test_assert(ptr != NULL);
    test_assert(ptr == elem);
    test_int(*ptr, 10);

    flecs_sparse_free(sp);
}

void Sparse_create_delete(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    uint64_t id = flecs_sparse_new_id(sp);
    test_int(flecs_sparse_count(sp), 1);

    flecs_sparse_remove_t(sp, int, id);
    test_int(flecs_sparse_count(sp), 0);
    test_assert(!flecs_sparse_is_alive(sp, id));

    flecs_sparse_free(sp);
}

void Sparse_create_delete_2(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);
    test_assert(sp != NULL);
    test_int(flecs_sparse_count(sp), 0);

    uint64_t id_1 = flecs_sparse_new_id(sp);
    test_int(flecs_sparse_count(sp), 1);
    test_assert(flecs_sparse_is_alive(sp, id_1));
    
    uint64_t id_2 = flecs_sparse_new_id(sp);
    test_int(flecs_sparse_count(sp), 2);
    test_assert(flecs_sparse_is_alive(sp, id_2));

    flecs_sparse_remove_t(sp, int, id_1);
    test_int(flecs_sparse_count(sp), 1);
    test_assert(!flecs_sparse_is_alive(sp, id_1));

    flecs_sparse_remove_t(sp, int, id_2);
    test_int(flecs_sparse_count(sp), 0);
    test_assert(!flecs_sparse_is_alive(sp, id_2));

    flecs_sparse_free(sp);
}

void Sparse_count_of_null(void) {
    test_int(flecs_sparse_count(NULL), 0);
}

void Sparse_try_low_after_ensure_high(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);

    int *ptr_1 = flecs_sparse_ensure_t(sp, int, 5000);
    test_assert(ptr_1 != NULL);

    int *ptr_2 = flecs_sparse_try_t(sp, int, 100);
    test_assert(ptr_2 == NULL);

    flecs_sparse_free(sp);
}

void Sparse_is_alive_low_after_ensure_high(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);

    int *ptr_1 = flecs_sparse_ensure_t(sp, int, 5000);
    test_assert(ptr_1 != NULL);

    bool v = flecs_sparse_is_alive(sp, 100);
    test_bool(v, false);

    flecs_sparse_free(sp);
}

void Sparse_remove_low_after_ensure_high(void) {
    ecs_sparse_t *sp = flecs_sparse_new(NULL, NULL, int);

    int *ptr_1 = flecs_sparse_ensure_t(sp, int, 5000);
    test_assert(ptr_1 != NULL);

    flecs_sparse_remove_t(sp, int, 100);

    flecs_sparse_free(sp);
}

void Sparse_ensure_skip_generation(void) {
    ecs_sparse_t *sp1 = flecs_sparse_new(NULL, NULL, int);
    ecs_sparse_t *sp2 = flecs_sparse_new(NULL, NULL, int);

    uint64_t id = flecs_sparse_new_id(sp1);
    flecs_sparse_ensure(sp2, 0, id);

    test_assert(flecs_sparse_is_alive(sp1, id));
    test_assert(flecs_sparse_is_alive(sp2, id));

    flecs_sparse_remove(sp1, 0, id);
    flecs_sparse_remove(sp2, 0, id);

    test_assert(!flecs_sparse_is_alive(sp1, id));
    test_assert(!flecs_sparse_is_alive(sp2, id));

    uint64_t id_2 = flecs_sparse_new_id(sp1);
    test_assert((uint32_t)id_2 == id);

    test_assert(flecs_sparse_is_alive(sp1, id_2));
    test_assert(!flecs_sparse_is_alive(sp2, id_2));
    test_assert(!flecs_sparse_is_alive(sp1, id));
    test_assert(!flecs_sparse_is_alive(sp2, id));

    flecs_sparse_remove(sp1, 0, id_2);

    test_assert(!flecs_sparse_is_alive(sp1, id_2));
    test_assert(!flecs_sparse_is_alive(sp2, id_2));
    test_assert(!flecs_sparse_is_alive(sp1, id));
    test_assert(!flecs_sparse_is_alive(sp2, id));

    uint64_t id_3 = flecs_sparse_new_id(sp1);
    test_assert((uint32_t)id_3 == id);
    flecs_sparse_ensure(sp2, 0, id_3);

    test_assert(flecs_sparse_is_alive(sp1, id_3));
    test_assert(flecs_sparse_is_alive(sp2, id_3));

    test_assert(!flecs_sparse_is_alive(sp1, id_2));
    test_assert(!flecs_sparse_is_alive(sp2, id_2));
    test_assert(!flecs_sparse_is_alive(sp1, id));
    test_assert(!flecs_sparse_is_alive(sp2, id));

    flecs_sparse_free(sp1);
    flecs_sparse_free(sp2);
}*/
	Describe("Flecs Sparse Creation Tests", [this]()
	{
		It("Should create a new sparse set correctly", [this]()
		{
			ecs_sparse_t* Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse set should not be null", Sparse);
			flecs_sparse_free(Sparse);
		});
	});
	
	Describe("Flecs Sparse Addition Tests", [this]()
	{
		It("Should add a single element correctly", [this]()
		{
			ecs_sparse_t* Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse set should not be null", Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			int* elem = flecs_sparse_add_t(Sparse, int);
			TestNotNull("Sparse element should not be null", elem);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 1);

			int *ptr = flecs_sparse_get_dense_t(Sparse, int, 0);
			TestNotNull("Sparse element should not be null", ptr);
			TestEqual("Sparse element should be correct", *ptr, 0);

			flecs_sparse_free(Sparse);
		});

		It("Should add multiple elements correctly", [this]()
		{
			ecs_sparse_t* Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse set should not be null", Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			int* elem1 = flecs_sparse_add_t(Sparse, int);
			TestNotNull("Sparse element should not be null", elem1);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 1);

			int* elem2 = flecs_sparse_add_t(Sparse, int);
			TestNotNull("Sparse element should not be null", elem2);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 2);

			int *ptr = flecs_sparse_get_dense_t(Sparse, int, 0);
			TestNotNull("Sparse element should not be null", ptr);
			TestEqual("Sparse element should be correct", *ptr, 0);

			ptr = flecs_sparse_get_dense_t(Sparse, int, 1);
			TestNotNull("Sparse element should not be null", ptr);
			TestEqual("Sparse element should be correct", *ptr, 0);

			flecs_sparse_free(Sparse);
		});

		It("Should add multiple elements correctly with chunk size 1", [this]()
		{
			ecs_sparse_t* Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse set should not be null", Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			int* elem1 = flecs_sparse_add_t(Sparse, int);
			TestNotNull("Sparse element should not be null", elem1);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 1);

			int* elem2 = flecs_sparse_add_t(Sparse, int);
			TestNotNull("Sparse element should not be null", elem2);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 2);

			int *ptr = flecs_sparse_get_dense_t(Sparse, int, 0);
			TestNotNull("Sparse element should not be null", ptr);
			TestEqual("Sparse element should be correct", *ptr, 0);

			ptr = flecs_sparse_get_dense_t(Sparse, int, 1);
			TestNotNull("Sparse element should not be null", ptr);
			TestEqual("Sparse element should be correct", *ptr, 0);

			flecs_sparse_free(Sparse);
		});
	});

	Describe("Flecs Sparse Removal Tests", [this]()
	{
		It("Should remove an element correctly", [this]()
		{
			ecs_sparse_t* Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse set should not be null", Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			populate(Sparse, 3);
			
			const uint64_t *indices = flecs_sparse_ids(Sparse);
			uint64_t i0 = indices[0];
			uint64_t i1 = indices[2];

			flecs_sparse_remove_t(Sparse, int, 1);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 2);

			TestEqual("Sparse element should be correct", *flecs_sparse_get_dense_t(Sparse, int, 0), *flecs_sparse_get_t(Sparse, int, i0));
			TestEqual("Sparse element should be correct", *flecs_sparse_get_dense_t(Sparse, int, 1), *flecs_sparse_get_t(Sparse, int, i1));

			flecs_sparse_free(Sparse);
		});

		It("Should remove the first element correctly", [this]()
		{
			ecs_sparse_t* Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse set should not be null", Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			populate(Sparse, 3);

			const uint64_t *indices = flecs_sparse_ids(Sparse);
			uint64_t i0 = indices[1];
			uint64_t i1 = indices[2];

			flecs_sparse_remove_t(Sparse, int, 0);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 2);

			TestEqual("Sparse element should be correct", *flecs_sparse_get_dense_t(Sparse, int, 0), *flecs_sparse_get_t(Sparse, int, i1));
			TestEqual("Sparse element should be correct", *flecs_sparse_get_dense_t(Sparse, int, 1), *flecs_sparse_get_t(Sparse, int, i0));

			flecs_sparse_free(Sparse);
		});

		It("Should remove the last element correctly", [this]()
		{
			ecs_sparse_t* Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse set should not be null", Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			populate(Sparse, 3);
			
			const uint64_t *indices = flecs_sparse_ids(Sparse);
			uint64_t i0 = indices[0];
			uint64_t i1 = indices[1];

			flecs_sparse_remove_t(Sparse, int, 2);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 2);

			TestEqual("Sparse element should be correct", *flecs_sparse_get_dense_t(Sparse, int, 0), *flecs_sparse_get_t(Sparse, int, i0));
			TestEqual("Sparse element should be correct", *flecs_sparse_get_dense_t(Sparse, int, 1), *flecs_sparse_get_t(Sparse, int, i1));

			flecs_sparse_free(Sparse);
		});

		It("Should remove all elements correctly", [this]()
		{
			ecs_sparse_t* Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse set should not be null", Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			populate(Sparse, 3);

			void *elem2 = flecs_sparse_get_dense_t(Sparse, int, 1);
			void *elem3 = flecs_sparse_get_dense_t(Sparse, int, 2);

			flecs_sparse_remove_t(Sparse, int, 0);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 2);
			TestNull("Sparse element should be null", flecs_sparse_try_t(Sparse, int, 0));
			TestEqual("Sparse element should be correct", flecs_sparse_try_t(Sparse, int, 1), static_cast<int*>(elem2));
			TestEqual("Sparse element should be correct", flecs_sparse_try_t(Sparse, int, 2), static_cast<int*>(elem3));

			flecs_sparse_remove_t(Sparse, int, 1);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 1);
			TestNull("Sparse element should be null", flecs_sparse_try_t(Sparse, int, 0));
			TestNull("Sparse element should be null", flecs_sparse_try_t(Sparse, int, 1));
			TestEqual("Sparse element should be correct", flecs_sparse_try_t(Sparse, int, 2), static_cast<int*>(elem3));

			flecs_sparse_remove_t(Sparse, int, 2);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);
			TestNull("Sparse element should be null", flecs_sparse_try_t(Sparse, int, 0));
			TestNull("Sparse element should be null", flecs_sparse_try_t(Sparse, int, 1));
			TestNull("Sparse element should be null", flecs_sparse_try_t(Sparse, int, 2));

			flecs_sparse_free(Sparse);
		});

		It("Should remove all elements correctly with multiple chunks", [this]()
		{
			ecs_sparse_t* Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse set should not be null", Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			populate(Sparse, 128);

			for (int i = 0; i < 128; i ++) {
				flecs_sparse_remove_t(Sparse, int, i);
				TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 127 - i);
			}

			flecs_sparse_free(Sparse);
		});
	});

	Describe("Flecs Sparse Clear Tests", [this]()
	{
		It("Should clear a single element correctly", [this]()
		{
			ecs_sparse_t* Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse set should not be null", Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			populate(Sparse, 1);

			flecs_sparse_clear(Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			flecs_sparse_free(Sparse);
		});

		It("Should clear an empty set correctly", [this]()
		{
			ecs_sparse_t* Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse set should not be null", Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			flecs_sparse_clear(Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			flecs_sparse_free(Sparse);
		});

		It("Should clear multiple elements correctly", [this]()
		{
			ecs_sparse_t* Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse set should not be null", Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			populate(Sparse, 3);

			flecs_sparse_clear(Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			flecs_sparse_free(Sparse);
		});

		It("Should clear multiple elements correctly with multiple chunks", [this]()
		{
			ecs_sparse_t* Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse set should not be null", Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			populate(Sparse, 128);

			flecs_sparse_clear(Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			flecs_sparse_free(Sparse);
		});
	});

	Describe("Flecs Sparse General Tests", [this]()
	{
		It("Should add an element after clearing the set correctly", [this]()
		{
			ecs_sparse_t* Sparse = flecs_sparse_new(NULL, NULL, int);
			
			if (!TestNotNull("Sparse set should not be null", Sparse))
			{
				return;
			}
			
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			populate(Sparse, 1);

			flecs_sparse_clear(Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			int* elem = flecs_sparse_add_t(Sparse, int);
			TestNotNull("Sparse element should not be null", elem);
			*elem = 10;

			const uint64_t *indices = flecs_sparse_ids(Sparse);
			TestNotNull("Sparse indices should not be null", indices);

			int *ptr = flecs_sparse_get_t(Sparse, int, indices[0]);
			if (TestNotNull("Sparse element should not be null", ptr))
			
			TestEqual("Sparse element should be correct", *ptr, 10);

			flecs_sparse_free(Sparse);
		});

		It("Should create and delete an element correctly", [this]()
		{
			ecs_sparse_t* Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse set should not be null", Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			uint64_t id = flecs_sparse_new_id(Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 1);

			flecs_sparse_remove_t(Sparse, int, id);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);
			TestFalse("Sparse element should not be alive", flecs_sparse_is_alive(Sparse, id));

			flecs_sparse_free(Sparse);
		});

		It("Should create and delete multiple elements correctly", [this]()
		{
			ecs_sparse_t* Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse set should not be null", Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			uint64_t id_1 = flecs_sparse_new_id(Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 1);
			TestTrue("Sparse element should be alive", flecs_sparse_is_alive(Sparse, id_1));
			
			uint64_t id_2 = flecs_sparse_new_id(Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 2);
			TestTrue("Sparse element should be alive", flecs_sparse_is_alive(Sparse, id_2));
		
	});
}

#endif // WITH_AUTOMATION_TESTS
