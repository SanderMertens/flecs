#if WITH_AUTOMATION_TESTS

#include "Misc/AutomationTest.h"

#include "flecs.h"

// Define the Spec using template variables for consistency
BEGIN_DEFINE_SPEC(FFlecsLibrarySparseTestsSpec,
                  "Flecs.Library.Collections.Sparse",
                  EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter);

	ecs_sparse_t* Sparse = nullptr;

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
} // namespace

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
		AfterEach([this]()
		{
		});
		
		BeforeEach([this]()
		{
			Sparse = nullptr;
		});
		
		It("Should create and free a sparse set", [this]()
		{
			Sparse = flecs_sparse_new(NULL, NULL, int);
			
			if (!TestNotNull("Sparse should not be null", Sparse))
			{
				return;
			}
			
			flecs_sparse_free(Sparse);
			Sparse = nullptr;
			TestNull("Sparse should be null after free", Sparse);
		});
	}); 
	
	Describe("Flecs Sparse Addition Tests", [this]()
	{
		BeforeEach([this]()
		{
			Sparse = flecs_sparse_new(NULL, NULL, int);
			check(Sparse != nullptr);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);
		});

		AfterEach([this]()
		{
			flecs_sparse_free(Sparse);
			Sparse = nullptr;
		});
		
		It("Should add a single element correctly", [this]()
		{
			int* elem = flecs_sparse_add_t(Sparse, int);
			
			if (!TestNotNull("Sparse element should not be null", elem))
			{
				return;
			}
			
			*elem = 0;
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 1);
		});

		It("Should add a single element correctly to an empty sparse set", [this]()
		{
			int* elem = flecs_sparse_add_t(Sparse, int);
			
			if (!TestNotNull("Sparse element should not be null", elem))
			{
				return;
			}
			
			*elem = 0;
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 1);
		});

		It("Should add a single element correctly with a chunk size of 1", [this]()
		{
			int* elem = flecs_sparse_add_t(Sparse, int);
			
			if (!TestNotNull("Sparse element should not be null", elem))
			{
				return;
			}
			
			*elem = 0;
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 1);
		});

		It("Should add multiple elements correctly", [this]()
		{
			int* elem1 = flecs_sparse_add_t(Sparse, int);
			int* elem2 = flecs_sparse_add_t(Sparse, int);
			
			if (!TestNotNull("Sparse element 1 should not be null", elem1))
			{
				return;
			}
			
			if (!TestNotNull("Sparse element 2 should not be null", elem2))
			{
				return;
			}
			
			*elem1 = 0;
			*elem2 = 1;
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 2);
		});

		It("Should add multiple elements correctly with a chunk size of 1", [this]()
		{
			int* elem1 = flecs_sparse_add_t(Sparse, int);
			int* elem2 = flecs_sparse_add_t(Sparse, int);
			
			if (!TestNotNull("Sparse element 1 should not be null", elem1))
			{
				return;
			}
			
			if (!TestNotNull("Sparse element 2 should not be null", elem2))
			{
				return;
			}
			
			*elem1 = 0;
			*elem2 = 1;
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 2);
		});
	});

	Describe("Flecs Sparse Removal Tests", [this]()
	{
		BeforeEach([this]()
		{
			Sparse = flecs_sparse_new(NULL, NULL, int);
			check(Sparse != nullptr);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);
		});

		AfterEach([this]()
		{
			flecs_sparse_free(Sparse);
			Sparse = nullptr;
		});
		
		It("Should remove an element correctly", [this]()
		{
			populate(Sparse, 3);
			
			const uint64_t* indices = flecs_sparse_ids(Sparse);
			uint64_t i0 = indices[0];
			uint64_t i1 = indices[2];
			
			flecs_sparse_remove_t(Sparse, int, 1);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 2);

			if (!TestEqual("Sparse element 0 should be correct", *flecs_sparse_get_dense_t(Sparse, int, 0), *flecs_sparse_get_t(Sparse, int, i0)))
			{
				return;
			}

			if (!TestEqual("Sparse element 2 should be correct", *flecs_sparse_get_dense_t(Sparse, int, 1), *flecs_sparse_get_t(Sparse, int, i1)))
			{
				return;
			}
		});

		It("Should remove the first element correctly", [this]()
		{
			populate(Sparse, 3);
			
			const uint64_t* indices = flecs_sparse_ids(Sparse);
			uint64_t i0 = indices[1];
			uint64_t i1 = indices[2];
			
			flecs_sparse_remove_t(Sparse, int, 0);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 2);

			if (!TestEqual("Sparse element 0 should be correct", *flecs_sparse_get_dense_t(Sparse, int, 0), *flecs_sparse_get_t(Sparse, int, i1)))
			{
				return;
			}

			if (!TestEqual("Sparse element 1 should be correct", *flecs_sparse_get_dense_t(Sparse, int, 1), *flecs_sparse_get_t(Sparse, int, i0)))
			{
				return;
			}
		});

		It("Should remove the last element correctly", [this]()
		{
			populate(Sparse, 3);
			
			const uint64_t* indices = flecs_sparse_ids(Sparse);
			uint64_t i0 = indices[0];
			uint64_t i1 = indices[1];
			
			flecs_sparse_remove_t(Sparse, int, 2);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 2);

			if (!TestEqual("Sparse element 0 should be correct", *flecs_sparse_get_dense_t(Sparse, int, 0), *flecs_sparse_get_t(Sparse, int, i0)))
			{
				return;
			}

			if (!TestEqual("Sparse element 1 should be correct", *flecs_sparse_get_dense_t(Sparse, int, 1), *flecs_sparse_get_t(Sparse, int, i1)))
			{
				return;
			}
		});

		It("Should remove all elements correctly", [this]()
		{
			populate(Sparse, 3);
			
			void* elem2 = flecs_sparse_get_dense_t(Sparse, int, 1);
			void* elem3 = flecs_sparse_get_dense_t(Sparse, int, 2);
			
			flecs_sparse_remove_t(Sparse, int, 0);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 2);
			if (!TestNull("Sparse element 0 should be null", flecs_sparse_try_t(Sparse, int, 0)))
			{
				return;
			}

			if (!TestEqual("Sparse element 1 should be correct", flecs_sparse_try_t(Sparse, int, 1), static_cast<int*>(elem2)))
			{
				return;
			}

			if (!TestEqual("Sparse element 2 should be correct", flecs_sparse_try_t(Sparse, int, 2), static_cast<int*>(elem3)))
			{
				return;
			}

			flecs_sparse_remove_t(Sparse, int, 1);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 1);

			if (!TestNull("Sparse element 0 should be null", flecs_sparse_try_t(Sparse, int, 0)))
			{
				return;
			}

			if (!TestNull("Sparse element 1 should be null", flecs_sparse_try_t(Sparse, int, 1)))
			{
				return;
			}

			if (!TestEqual("Sparse element 2 should be correct", flecs_sparse_try_t(Sparse, int, 2), static_cast<int*>(elem3)))
			{
				return;
			}

			flecs_sparse_remove_t(Sparse, int, 2);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);

			if (!TestNull("Sparse element 0 should be null", flecs_sparse_try_t(Sparse, int, 0)))
			{
				return;
			}

			if (!TestNull("Sparse element 1 should be null", flecs_sparse_try_t(Sparse, int, 1)))
			{
				return;
			}

			if (!TestNull("Sparse element 2 should be null", flecs_sparse_try_t(Sparse, int, 2)))
			{
				return;
			}
		});

		It("Should remove all elements correctly with multiple chunks", [this]()
		{
			populate(Sparse, 128);
			
			for (int i = 0; i < 128; i ++)
			{
				flecs_sparse_remove_t(Sparse, int, i);
				TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 127 - i);
			}
		});
	});

	Describe("Flecs Sparse Clearing Tests", [this]()
	{
		BeforeEach([this]()
		{
			Sparse = flecs_sparse_new(NULL, NULL, int);
			check(Sparse != nullptr);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);
		});

		AfterEach([this]()
		{
			flecs_sparse_free(Sparse);
			Sparse = nullptr;
		});
		
		It("Should clear a sparse set with a single element", [this]()
		{
			populate(Sparse, 1);
			flecs_sparse_clear(Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);
		});

		It("Should clear an empty sparse set", [this]()
		{
			flecs_sparse_clear(Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);
		});

		It("Should clear a sparse set with multiple elements", [this]()
		{
			populate(Sparse, 3);
			flecs_sparse_clear(Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);
		});

		It("Should clear a sparse set with multiple elements and multiple chunks", [this]()
		{
			populate(Sparse, 128);
			flecs_sparse_clear(Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);
		});
	});

	
	Describe("Flecs Sparse Multi Tests", [this]()
	{
		BeforeEach([this]()
		{
			Sparse = flecs_sparse_new(NULL, NULL, int);
			check(Sparse != nullptr);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);
		});

		AfterEach([this]()
		{
			flecs_sparse_free(Sparse);
			Sparse = nullptr;
		});

		It("Should add an element after clearing a sparse set", [this]()
		{
			populate(Sparse, 1);
			flecs_sparse_clear(Sparse);
			TestEqual("Sparse count should be correct", flecs_sparse_count(Sparse), 0);
			
			int* elem = flecs_sparse_add_t(Sparse, int);
			
			if (!TestNotNull("Sparse element should not be null", elem))
			{
				return;
			}
			
			*elem = 10;
			const uint64_t* indices = flecs_sparse_ids(Sparse);

			if (!TestNotNull("Sparse indices should not be null", indices))
			{
				return;
			}
			
			int* ptr = flecs_sparse_get_t(Sparse, int, indices[0]);
			
			if (!TestNotNull("Sparse ptr should not be null", ptr))
			{
				return;
			}
			
			TestEqual("Sparse ptr should be correct", *ptr, 10);
		});
	});
		
	Describe("Flecs Sparse ID and Generation Tests", [this]()
	{
		BeforeEach([this]()
		{
			// Some tests won't need a valid Sparse 
			// (e.g. Sparse_count_of_null), so we create 
			// conditionally in each It block if needed.
		});

		AfterEach([this]()
		{
			if (Sparse)
			{
				flecs_sparse_free(Sparse);
				Sparse = nullptr;
			}
		});

		It("Should create and delete an element by ID", [this]()
		{
			Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse should not be null", Sparse);

			uint64_t id = flecs_sparse_new_id(Sparse);
			TestEqual("Sparse count should be correct after new_id", flecs_sparse_count(Sparse), 1);
			
			flecs_sparse_remove_t(Sparse, int, id);
			TestEqual("Sparse count should be correct after remove", flecs_sparse_count(Sparse), 0);
			
			TestFalse("Sparse ID should not be alive", flecs_sparse_is_alive(Sparse, id));
		});

		It("Should create and delete two elements by ID", [this]()
		{
			Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse should not be null", Sparse);

			uint64_t id_1 = flecs_sparse_new_id(Sparse);
			TestEqual("Sparse count should be 1 after creating first ID", flecs_sparse_count(Sparse), 1);
			TestTrue("First ID should be alive", flecs_sparse_is_alive(Sparse, id_1));

			uint64_t id_2 = flecs_sparse_new_id(Sparse);
			TestEqual("Sparse count should be 2 after creating second ID", flecs_sparse_count(Sparse), 2);
			TestTrue("Second ID should be alive", flecs_sparse_is_alive(Sparse, id_2));

			flecs_sparse_remove_t(Sparse, int, id_1);
			TestEqual("Sparse count should be correct after removing first ID", flecs_sparse_count(Sparse), 1);
			TestFalse("First ID should not be alive", flecs_sparse_is_alive(Sparse, id_1));

			flecs_sparse_remove_t(Sparse, int, id_2);
			TestEqual("Sparse count should be correct after removing second ID", flecs_sparse_count(Sparse), 0);
			TestFalse("Second ID should not be alive", flecs_sparse_is_alive(Sparse, id_2));
		});

		It("Should return zero count for a null pointer", [this]()
		{
			TestEqual("flecs_sparse_count(NULL) should return 0", flecs_sparse_count(nullptr), 0);
		});

		It("Should try a low ID after ensuring a high ID", [this]()
		{
			Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse should not be null", Sparse);

			int* ptr_1 = flecs_sparse_ensure_t(Sparse, int, 5000);
			TestNotNull("Ensuring a high ID should succeed", ptr_1);

			// Try a lower ID, which hasn't been ensured
			int* ptr_2 = flecs_sparse_try_t(Sparse, int, 100);
			TestNull("Trying a lower ID that was not ensured should be null", ptr_2);
		});

		It("Should check is_alive for a low ID after ensuring a high ID", [this]()
		{
			Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse should not be null", Sparse);

			int* ptr_1 = flecs_sparse_ensure_t(Sparse, int, 5000);
			TestNotNull("Ensuring a high ID should succeed", ptr_1);

			bool isAlive = flecs_sparse_is_alive(Sparse, 100);
			TestFalse("Lower ID should not be alive", isAlive);
		});

		It("Should remove a low ID after ensuring a high ID (should be a no-op)", [this]()
		{
			Sparse = flecs_sparse_new(NULL, NULL, int);
			TestNotNull("Sparse should not be null", Sparse);

			int* ptr_1 = flecs_sparse_ensure_t(Sparse, int, 5000);
			TestNotNull("Ensuring a high ID should succeed", ptr_1);

			// Removing a lower ID that was never ensured or added
			flecs_sparse_remove_t(Sparse, int, 100);

			// Should not crash or affect the high ID
			TestEqual("Sparse count should still be 1", flecs_sparse_count(Sparse), 1);
			TestTrue("High ID should remain alive", flecs_sparse_is_alive(Sparse, 5000));
		});

		It("Should ensure skip generation across two Sparse sets", [this]()
		{
			ecs_sparse_t* sp1 = flecs_sparse_new(NULL, NULL, int);
			ecs_sparse_t* sp2 = flecs_sparse_new(NULL, NULL, int);

			TestNotNull("sp1 should not be null", sp1);
			TestNotNull("sp2 should not be null", sp2);

			uint64_t id = flecs_sparse_new_id(sp1);
			TestTrue("id should be alive in sp1", flecs_sparse_is_alive(sp1, id));

			// Ensure the same ID in sp2
			flecs_sparse_ensure(sp2, 0, id);
			TestTrue("id should be alive in sp2", flecs_sparse_is_alive(sp2, id));

			// Remove ID from both
			flecs_sparse_remove(sp1, 0, id);
			flecs_sparse_remove(sp2, 0, id);
			TestFalse("id should no longer be alive in sp1", flecs_sparse_is_alive(sp1, id));
			TestFalse("id should no longer be alive in sp2", flecs_sparse_is_alive(sp2, id));

			// Generate a new ID in sp1, which should reuse the old 32-bit ID, but bumped generation
			uint64_t id2 = flecs_sparse_new_id(sp1);
			TestEqual("The 32-bit part of id2 should match the old ID", 
					  static_cast<uint32>(id2), 
					  static_cast<uint32>(id));

			TestTrue("New id2 should be alive in sp1", flecs_sparse_is_alive(sp1, id2));
			TestFalse("id2 should not be alive in sp2", flecs_sparse_is_alive(sp2, id2));

			// Remove from sp1 again
			flecs_sparse_remove(sp1, 0, id2);
			TestFalse("id2 should no longer be alive in sp1", flecs_sparse_is_alive(sp1, id2));

			// Reuse again
			uint64_t id3 = flecs_sparse_new_id(sp1);
			TestEqual("The 32-bit part of id3 should match the old ID again", 
					  static_cast<uint32>(id3), 
					  static_cast<uint32>(id));

			// Ensure sp2 also sees id3 as valid
			flecs_sparse_ensure(sp2, 0, id3);
			TestTrue("id3 should be alive in sp1", flecs_sparse_is_alive(sp1, id3));
			TestTrue("id3 should be alive in sp2", flecs_sparse_is_alive(sp2, id3));

			// Cleanup
			flecs_sparse_free(sp1);
			flecs_sparse_free(sp2);
		});
	});
}

#endif // WITH_AUTOMATION_TESTS
